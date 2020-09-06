#include "NetDriver.h"
#include "NetTypes.h"
#include "Serialization/MemoryBitStream.h"
#include "Serialization/ISerializableObject.h"
#include "Serialization/LinkingContext.h"
#include "UObject.h"
#include "ObjectCreationRegistry.h"
#include "WinSock2.h"
#include "Sockets/UDPSocket.h"
#include "Sockets/SocketUtil.h"
#include "ObjectReplicationHeader.h"

static constexpr int DEFAULT_CLIENT = 27015;
static constexpr int DEFAULT_SERVER = 27016;

static constexpr int MTU_LIMIT_BYTES = 1300;

void NetDriver::Init()
{
	mLinkingContextPtr = std::make_shared<LinkingContext>();

	ObjectCreationRegistry::Get().RegisterCreationFunction<UObject>();

	WSADATA wsaData;
	int Result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (Result == SOCKET_ERROR)
	{
		//Print error code
		return;
	}

	std::cout << "Select Mode: [0] DedicatedServer [1] Client ";

	int GetInput;
	std::cin >> GetInput;

	NetMode = (ENetMode)GetInput;

	InitNetSocket(NetMode);

	if (NetMode == ENetMode::ENM_DedicatedServer)
	{
		// Just For testing proposes
		UObject* TestObj = new UObject();
		mReplicatedObjects.insert(TestObj);
		// Just For testing proposes

	}
	else if (NetMode == ENetMode::ENM_Client)
	{
		std::string IpToConnect;

		std::cout << "Enter Dedicated/Listen Server IP: ";
		std::cin >> IpToConnect;
		
		ServerAddress = SocketAddress(IpToConnect);

		MemoryBitStream WriteStream;

		PacketType Package = PacketType::PT_Hello;
		WriteStream.SerializeBits(&Package, GetRequiredBits<(int)PacketType::PT_MAX>::Value);
		
		NetSocket->SendTo(WriteStream.GetBufferPtr(), WriteStream.GetLengthInBytes(), ServerAddress);
	}
}

void NetDriver::Shutdown()
{
	int Result = WSACleanup();
	if (Result == SOCKET_ERROR)
	{
		//Print error code;
		Result = WSAGetLastError();
	}
}

void NetDriver::ReplicateWorldState(MemoryBitStream& InStream, const std::vector<UObject*>& InWorldObjects)
{
	PacketType Package = PacketType::PT_ReplicationData; 
	InStream.SerializeBits(&Package, GetRequiredBits<(int)PacketType::PT_MAX>::Value);

	InStream << (uint32_t)InWorldObjects.size();

	//TODO: Implement GridGraph Here
	for (UObject* WorldObject : InWorldObjects)
	{
		ReplicateObjectIntoStream(InStream, WorldObject, ReplicationAction::RA_Create);	
	}
}

void NetDriver::ReceiveReplicatedWorldState(MemoryBitStream& InStream)
{

	int Amount = 0;
	InStream << Amount;

	std::unordered_set<UObject*> ReceivedObjects;
	ReceivedObjects.reserve(Amount);

	while (Amount > 0)
	{
		UObject* ReceivedObject = ReceiveReplicatedObject(InStream);
		if (ReceivedObject != nullptr)
		{
			ReceivedObjects.insert(ReceivedObject);
		}
		Amount--;
	}

	// [CHANGE] Really naive and basic approach, assuming I'll receive the state of all the actors
	for (UObject* Obj : mReplicatedObjects)
	{
		if (ReceivedObjects.find(Obj) != ReceivedObjects.end() && !Obj->IsPendingToKill())
		{
			mLinkingContextPtr->RemoveObject(Obj);
			Obj->Destroy();
		}
	}

	mReplicatedObjects = ReceivedObjects;
}

void NetDriver::ReplicateObjectIntoStream(MemoryBitStream& InStream, UObject* InObject, ReplicationAction InRA)
{
	const bool bShouldCreate = InRA == ReplicationAction::RA_Create;
	ObjectReplicationHeader ReplHeader = ObjectReplicationHeader(ReplicationAction::RA_Create, mLinkingContextPtr->GetNetworkId(InObject, bShouldCreate), InObject->GetClassId());

	InStream << ReplHeader;

	if (InRA != RA_Destroy)
	{
		InStream << *(static_cast<ISerializableObject*>(InObject));
	}
}

UObject* NetDriver::ReceiveReplicatedObject(MemoryBitStream& InStream)
{
	ObjectReplicationHeader ReplHeader;
	InStream << ReplHeader;

	std::cout << "Received " << ReplHeader.mNetId << " " << ReplHeader.mClassId << '\n';

	switch (ReplHeader.mReplicationAction)
	{
	case RA_Create:
	{
		UObject* Obj = ObjectCreationRegistry::Get().CreateObject(ReplHeader.mClassId);
		mLinkingContextPtr->AddObject(Obj, ReplHeader.mNetId);

		InStream << *(static_cast<ISerializableObject*>(Obj));

		return Obj;
	}
	case RA_Update:
	{
		if (UObject* Obj = mLinkingContextPtr->GetObjectById(ReplHeader.mNetId))
		{
			InStream << *(static_cast<ISerializableObject*>(Obj));
			return Obj;
		}
		else
		{
			// For now just create a temporary object to read the buffer so we can continue the reading.
			UObject* TmpObj = ObjectCreationRegistry::Get().CreateObject(ReplHeader.mClassId);

			InStream << *(static_cast<ISerializableObject*>(TmpObj));
			delete TmpObj;
		}

		break;
	}
	case RA_Destroy:
	{
		if (UObject* Obj = mLinkingContextPtr->GetObjectById(ReplHeader.mNetId))
		{
			mLinkingContextPtr->RemoveObject(Obj);
			
			//TODO: I shouldn't delete the object here
			//Iterate pending to kill and delete at the end of the tick.
			//Or Make those objects SharedPtrs to control their lifetime.

			//Obj->Destroy();
			
			delete Obj;
		}

		break;
	}

	default:	
		break;
	}


	return nullptr;
}

void NetDriver::InitNetSocket(ENetMode NetMode)
{
	int SocketResult = -1;

	while (SocketResult != NO_ERROR)
	{
		NetSocket = SocketUtil::CreateUDPSocket(INET);
		const int Port = NetMode == ENetMode::ENM_DedicatedServer ? DEFAULT_SERVER : DEFAULT_CLIENT;

		SocketAddress ReceivingAddress(INADDR_ANY, Port);

		if (NetSocket != nullptr)
		{
			SocketResult = NetSocket->Bind(ReceivingAddress) == NO_ERROR ? NO_ERROR : SOCKET_ERROR;

			if (SocketResult == SOCKET_ERROR)
			{
				std::cout << "SocketError! " << GetLastError();
			}
		}
	}

	SocketUtil::SetNonBlockingMode(NetSocket, true);

	std::cout << "NetSocketInitialized..." << '\n';
}

void NetDriver::Tick()
{
	while (true)
	{
		if (NetSocket != nullptr)
		{
			if (NetMode == ENetMode::ENM_DedicatedServer)
			{
				//Receive
				char Segment[MTU_LIMIT_BYTES];
				
				SocketAddress IncomingDataAddr;
				int BytesReceived = NetSocket->ReceiveFrom(Segment, MTU_LIMIT_BYTES, IncomingDataAddr);

				if (BytesReceived > 0)
				{
					std::cout << "Packet Received from " << IncomingDataAddr.ToString();
					MemoryBitStream ReadStream = MemoryBitStream(Segment, BytesReceived * 8);

					PacketType IncomingPackageType = PT_MAX;
					ReadStream.SerializeBits(&IncomingPackageType, GetRequiredBits<(int)PacketType::PT_MAX>::Value);

					if (IncomingPackageType == PacketType::PT_Hello)
					{
						ClientsAddress.push_back(IncomingDataAddr);
					}
					else if(IncomingPackageType == PacketType::PT_Disconnect)
					{
						ClientsAddress.erase(std::remove(ClientsAddress.begin(), ClientsAddress.end(), IncomingDataAddr));
					}
				}

				if (ClientsAddress.size() > 0)
				{
					//Send
					MemoryBitStream WriteStream;

					//Just For testing proposes
					std::vector<UObject*> Objects = std::vector<UObject*>(mReplicatedObjects.size()-1);
					for (auto Obj : mReplicatedObjects)
					{
						Objects.push_back(Obj);
					}
					//Just for testing proposes

					ReplicateWorldState(WriteStream, Objects);

					if (MTU_LIMIT_BYTES < WriteStream.GetLengthInBytes())
					{
						//Log error;
						//TODO place to handle packet split
						return;
					}

					for (const auto& Address : ClientsAddress)
					{
						NetSocket->SendTo(WriteStream.GetBufferPtr(), WriteStream.GetLengthInBytes(), Address);
					}
				}
			}
			else if (NetMode == ENetMode::ENM_Client)
			{
				char Segment[MTU_LIMIT_BYTES];

				SocketAddress IncomingDataAddr;
				int BytesReceived = NetSocket->ReceiveFrom(Segment, MTU_LIMIT_BYTES, IncomingDataAddr);

				if (BytesReceived > 0)
				{
					MemoryBitStream ReadStream = MemoryBitStream(Segment, BytesReceived * 8);

					PacketType IncomingPackageType = PT_MAX;
					ReadStream.SerializeBits(&IncomingPackageType, GetRequiredBits<(int)PacketType::PT_MAX>::Value);

					if (IncomingPackageType == PacketType::PT_ReplicationData)
					{
						ReceiveReplicatedWorldState(ReadStream);
					}
				}

			}
		}
	}
}
