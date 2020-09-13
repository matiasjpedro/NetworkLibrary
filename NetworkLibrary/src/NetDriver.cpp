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
		mWorldObjects.insert(std::make_unique<UObject>());
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

void NetDriver::ReplicateWorldState(MemoryBitStream& InStream)
{
	PacketType Package = PacketType::PT_ReplicationData; 
	InStream.SerializeBits(&Package, GetRequiredBits<(int)PacketType::PT_MAX>::Value);

	//TODO: Implement GridGraph Here to know which actor we want to replicate to which actor.

	InStream << (uint32_t)mWorldObjects.size();
	
	for (const std::unique_ptr<UObject>& Obj : mWorldObjects)
	{
		//TODO: Replicate update instead of create if those are were already created
		//I could use the concept of actor channels to ack the creation in the client.
		ReplicateObjectIntoStream(InStream, Obj.get(), ReplicationAction::RA_Create);
	}
}

void NetDriver::ReceiveReplicatedWorldState(MemoryBitStream& InStream)
{
	int Amount = 0;
	InStream << Amount;

	while (Amount > 0)
	{
		ReceiveReplicatedObject(InStream);
		Amount--;
	}
}

void NetDriver::ReplicateObjectIntoStream(MemoryBitStream& InStream, UObject* InObject, ReplicationAction InRA)
{
	const bool bShouldCreate = InRA == ReplicationAction::RA_Create;
	ObjectReplicationHeader ReplHeader = ObjectReplicationHeader(ReplicationAction::RA_Create, mLinkingContext.GetNetId(InObject, bShouldCreate), InObject->GetClassId());

	InStream << ReplHeader;

	if (InRA != RA_Destroy)
	{
		InStream << *(static_cast<ISerializableObject*>(InObject));
	}
}

void NetDriver::ReceiveReplicatedObject(MemoryBitStream& InStream)
{
	ObjectReplicationHeader ReplHeader;
	InStream << ReplHeader;

	std::cout << "Received " << ReplHeader.mNetId << " " << ReplHeader.mClassId << '\n';

	switch (ReplHeader.mReplicationAction)
	{
	case RA_Create:
	{
		if (mLinkingContext.GetObjectByNetId(ReplHeader.mNetId) == nullptr)
		{
			UObject* Obj = ObjectCreationRegistry::Get().CreateObject(ReplHeader.mClassId);
			mLinkingContext.AddObject(Obj, ReplHeader.mNetId);

			InStream << *(static_cast<ISerializableObject*>(Obj));
		}

		break;
	}
	case RA_Update:
	{
		if (UObject* Obj = mLinkingContext.GetObjectByNetId(ReplHeader.mNetId))
		{
			InStream << *(static_cast<ISerializableObject*>(Obj));
		}
		else
		{
			// For now just create a temporary object to read the buffer so we can continue the reading.
			// Unless we have a size function to know when this object is finish.
			std::unique_ptr<UObject> TmpObj = std::unique_ptr<UObject>(ObjectCreationRegistry::Get().CreateObject(ReplHeader.mClassId));
			InStream << *(static_cast<ISerializableObject*>(TmpObj.get()));
		}

		break;
	}
	case RA_Destroy:
	{
		mLinkingContext.RemoveObjectById(ReplHeader.mNetId);
		//TODO: We should only mark it for delete so we can give time other system to clean logic up
	}

	default:	
		break;
	}
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
					ReplicateWorldState(WriteStream);

					if (WriteStream.GetLengthInBytes() < MTU_LIMIT_BYTES)
					{
						for (const auto& Address : ClientsAddress)
						{
							NetSocket->SendTo(WriteStream.GetBufferPtr(), WriteStream.GetLengthInBytes(), Address);
						}
						
					}
					else
					{
						//Log error;
						//TODO place to handle packet split
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

