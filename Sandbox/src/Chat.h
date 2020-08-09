#pragma once
#include "WinSock2.h"
#include <iostream>
#include <vector>
#include "Sockets/TCPSocket.h"
#include "Sockets/SocketUtil.h"
#include "Serialization/MemoryStream.h"
#include <thread>

// -2 because those are reserved for the length.
static constexpr int MAX_CHAT_LENGTH = 255 - 2;
static constexpr int DEFAULT_PORT = 27015;

enum EChatMode : int 
{
	ECM_DedicatedServer,
	ECM_ListenServer,
	ECM_Client,
	ECM_P2PClient,
	ECM_STUNService
};

std::string Nickname;

void ProcessNewClient(const TCPSocketPtr& SockPtr, const SocketAddress& SockAddr)
{
	std::cout << "New client! IP:" << SockAddr.ToString() << '\n';
}

void ProcessDataFromClient(const TCPSocketPtr& SockPtr, const char* Segment, int DataReceivedLenght)
{
	std::string Msg;
	MemoryStream Reader = MemoryStream(Segment, DataReceivedLenght);
	Reader.SerializeString(Msg);

	std::cout << Msg << '\n';
}

TCPSocketPtr InitializeListenSocket()
{
	TCPSocketPtr ListenSocket = nullptr;
	int SocketResult = -1;

	while (SocketResult != NO_ERROR)
	{
		ListenSocket = SocketUtil::CreateTCPSocket(INET);
		SocketAddress ReceivingAddress(INADDR_ANY, DEFAULT_PORT);

		if (ListenSocket != nullptr)
		{
			SocketResult = ListenSocket->Bind(ReceivingAddress) == NO_ERROR && ListenSocket->Listen() == NO_ERROR ? NO_ERROR : SOCKET_ERROR;
		}
	}

	std::cout << "Server initialized successfully" << '\n';
	std::cout << "Listening..." << '\n';

	return ListenSocket;
}

TCPSocketPtr InitializeClientSocket(const SocketAddress& SocketAddr)
{
	TCPSocketPtr ClientSocket = nullptr;

	int SocketResult = -1;

	while (SocketResult != NO_ERROR)
	{
		ClientSocket = SocketUtil::CreateTCPSocket(INET);

		if (ClientSocket != nullptr)
		{
			std::cout << "Trying to connect to: " << SocketAddr.ToString() << '\n';
			SocketResult = ClientSocket->Connect(SocketAddr);

			if (SocketResult == SOCKET_ERROR)
			{
				int LastErrorCode = WSAGetLastError();
				std::cout << "Unable to connect, error code: " << LastErrorCode << '\n';
			}
		}
	}

	std::cout << "Client initialized successfully" << '\n';

	SocketUtil::SetNonBlockingMode(ClientSocket, true);

	return ClientSocket;
}

void ProcessServerChat(TCPSocketPtr ListenSocket)
{
	std::vector<TCPSocketPtr> ReadBlockSockets;
	std::vector<TCPSocketPtr> ReadableSockets;

	ReadBlockSockets.push_back(ListenSocket);

	while (true)
	{
		if (SocketUtil::Select(&ReadBlockSockets, &ReadableSockets, nullptr, nullptr, nullptr, nullptr))
		{
			for (const TCPSocketPtr& CurrentSocket : ReadableSockets)
			{
				if (ListenSocket != nullptr && CurrentSocket == ListenSocket)
				{
					//It's the listen socket, accept a new connection
					SocketAddress NewClientAddress;
					auto NewSocket = CurrentSocket->Accept(NewClientAddress);
					ReadBlockSockets.push_back(NewSocket);
					ProcessNewClient(NewSocket, NewClientAddress);
				}
				else
				{
					//It's a regular socket-process the data...
					char Segment[MAX_CHAT_LENGTH];
					int DataReceived = CurrentSocket->Receive(Segment, MAX_CHAT_LENGTH);
					if (DataReceived > 0)
					{
						ProcessDataFromClient(CurrentSocket, Segment, DataReceived);

						for (const TCPSocketPtr& SocketItem : ReadBlockSockets)
						{
							if (SocketItem == ListenSocket || SocketItem == CurrentSocket)
							{
								continue;
							}

							// Send chat to the other clients.
							SocketItem->Send(Segment, DataReceived);
						}
					}

				}
			}
		}
	}
}

void ProcessClientChat(std::vector<TCPSocketPtr>& ClientSockets, TCPSocketPtr ListenServerSocket)
{
	std::cout << "Enter your nickname: ";
	std::cin >> Nickname;

	std::string ChatMsg;

	auto GetMsgLambda = [&ChatMsg]()
	{
		while (true)
		{
			std::string tmp;
			std::getline(std::cin, tmp);

			if (ChatMsg.empty())
			{
				ChatMsg = tmp;
			}
		}
	};

	std::thread T1(GetMsgLambda);

	while (true)
	{
		if (!ListenServerSocket)
		{
			for (const TCPSocketPtr& ClientSocket : ClientSockets)
			{
				char Segment[MAX_CHAT_LENGTH];
				int DataReceived = ClientSocket->Receive(Segment, MAX_CHAT_LENGTH);
				if (DataReceived > 0)
				{
					ProcessDataFromClient(ClientSocket, Segment, DataReceived);
				}
			}
		}

		if (!ChatMsg.empty())
		{
			std::string Msg;
			Msg.append(Nickname);
			Msg.append(": ");
			Msg.append(ChatMsg);

			if (Msg.size() <= MAX_CHAT_LENGTH)
			{
				MemoryStream Writer = MemoryStream();
				Writer.SerializeString(Msg);

				for (const TCPSocketPtr& ClientSocket : ClientSockets)
				{
					if (ListenServerSocket && ListenServerSocket == ClientSocket)
						continue;

					ClientSocket->Send(Writer.GetBufferPtr(), Writer.GetLength());
				}
			}

			ChatMsg.clear();
		}
	}
}

void ProcessListenServer(TCPSocketPtr ListenSocket)
{
	std::vector<TCPSocketPtr> ReadBlockSockets;
	std::vector<TCPSocketPtr> ReadableSockets;

	ReadBlockSockets.push_back(ListenSocket);

	auto SelectLambda = [&ReadBlockSockets, &ReadableSockets, &ListenSocket]()
	{
		while (true)
		{
			if (SocketUtil::Select(&ReadBlockSockets, &ReadableSockets, nullptr, nullptr, nullptr, nullptr))
			{
				for (const TCPSocketPtr& CurrentSocket : ReadableSockets)
				{
					if (ListenSocket != nullptr && CurrentSocket == ListenSocket)
					{
						//It's the listen socket, accept a new connection
						SocketAddress NewClientAddress;
						auto NewSocket = CurrentSocket->Accept(NewClientAddress);
						ReadBlockSockets.push_back(NewSocket);
						ProcessNewClient(NewSocket, NewClientAddress);
					}
					else
					{
						//It's a regular socket-process the data...
						char Segment[MAX_CHAT_LENGTH];
						int DataReceived = CurrentSocket->Receive(Segment, MAX_CHAT_LENGTH);
						if (DataReceived > 0)
						{
							ProcessDataFromClient(CurrentSocket, Segment, DataReceived);

							for (const TCPSocketPtr& SocketItem : ReadBlockSockets)
							{
								if (SocketItem == ListenSocket || SocketItem == CurrentSocket)
								{
									continue;
								}

								// Send chat to the other clients.
								SocketItem->Send(Segment, DataReceived);
							}
						}

					}
				}
			}
		}
	};

	std::thread T1(SelectLambda);

	ProcessClientChat(ReadBlockSockets, ListenSocket);
}

void ProcessP2PClient(TCPSocketPtr STUNSocket)
{
	std::vector<TCPSocketPtr> OtherClientsConnections;

	std::cout << "Waiting other players to join..." << '\n';

	while (true)
	{
		//255.255.255.255:65555 = 22 + 1(nulltermination) + 2(length of the string)
		char Segment[25];
		int DataReceived = STUNSocket->Receive(Segment, 25);
		if (DataReceived > 0)
		{
			std::string OtherClientIP;
			MemoryStream Reader = MemoryStream(Segment, DataReceived);

			Reader.SerializeString(OtherClientIP);

			std::cout << "Client Join MM: " << OtherClientIP << '\n';

			TCPSocketPtr ClientSocket = InitializeClientSocket(SocketAddress(OtherClientIP));

			OtherClientsConnections.push_back(ClientSocket);
		}

		if (OtherClientsConnections.size() > 0)
		{
			ProcessClientChat(OtherClientsConnections, nullptr);
		}
	}
}

void ProcessSTUNService(TCPSocketPtr ListenSocket)
{
	std::vector<TCPSocketPtr> AllSockets;
	std::vector<TCPSocketPtr> SocketsWithChanges;

	AllSockets.push_back(ListenSocket);

	std::vector<std::string> P2PClientsAddrVector;

	while (true)
	{
		if (SocketUtil::Select(&AllSockets, &SocketsWithChanges, nullptr, nullptr, nullptr, nullptr))
		{
			for (const TCPSocketPtr& SocketWithChanges : SocketsWithChanges)
			{
				if (ListenSocket != nullptr && SocketWithChanges == ListenSocket)
				{
					//It's the listen socket, accept a new connection
					SocketAddress NewClientAddress;
					auto NewSocket = SocketWithChanges->Accept(NewClientAddress);

					std::string NewClientAddrStr = NewClientAddress.ToString();

					std::cout << "New Client registered in the STUN: " << NewClientAddrStr << '\n';

					//Send the IP of the new client to all the clients connected
					if (AllSockets.size() > 1)
					{
						auto PortPos = NewClientAddrStr.find_last_of(':');
						int PortNum;
						if (PortPos != std::string::npos)
						{
							std::string PortStr = NewClientAddrStr.substr(PortPos + 1);
							PortNum = std::stoi(PortStr);
						}

						for (const TCPSocketPtr& SocketItem : AllSockets)
						{
							if (SocketItem == ListenSocket)
							{
								continue;
							}

							//Port prediction, assuming the NAT is sequentially assigning ports
							PortNum+=1;

							std::string PortStr = std::to_string(PortNum);

							NewClientAddrStr.replace(PortPos + 1, PortStr.size(), PortStr);

							MemoryStream Writer = MemoryStream();
							Writer.SerializeString(NewClientAddrStr);

							std::cout << "Send new client address to already connected client: " << NewClientAddrStr << '\n';

							SocketItem->Send(Writer.GetBufferPtr(), Writer.GetLength());
						}
					}

					//Send the IP of the already connected clients to the new client
					for (std::string& ClientAddrStr : P2PClientsAddrVector)
					{
						auto PortPos = ClientAddrStr.find_last_of(':');
						if (PortPos != std::string::npos)
						{
							std::string PortStr = ClientAddrStr.substr(PortPos + 1);

							int PortNum = std::stoi(PortStr);

							//Port prediction, assuming the NAT is sequentially assigning ports
							PortNum += 1;

							PortStr = std::to_string(PortNum);
							ClientAddrStr.replace(PortPos + 1, PortStr.size(), PortStr);
						}

						MemoryStream Writer = MemoryStream();
						Writer.SerializeString(ClientAddrStr);

						std::cout << "Send already connected client address to the new client: " << ClientAddrStr << '\n';

						NewSocket->Send(Writer.GetBufferPtr(), Writer.GetLength());
					}

					P2PClientsAddrVector.push_back(NewClientAddrStr);
					AllSockets.push_back(NewSocket);
				}
			}
		}
	}
}


void DoTCPChat()
{
	WSADATA wsaData;
	int Result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (Result == SOCKET_ERROR)
	{
		//Print error code
		return;
	}

	std::cout << "Select Mode: [0] DedicatedServer [1] ListenServer [2] Client [3] ClientP2P [4] STUNService:  ";

	int Mode;
	std::cin >> Mode;

	std::vector<TCPSocketPtr> ReadBlockSockets;
	std::vector<TCPSocketPtr> ReadableSockets;

	if (Mode == ECM_DedicatedServer)
	{
		TCPSocketPtr ListenSocket = InitializeListenSocket();

		if (ListenSocket != nullptr)
		{
			ProcessServerChat(ListenSocket);	
		}
	}
	else if (Mode == ECM_ListenServer)
	{
		TCPSocketPtr ListenSocket = InitializeListenSocket();

		if (ListenSocket != nullptr)
		{
			ProcessListenServer(ListenSocket);
		}
	}
	else if (Mode == ECM_P2PClient)
	{
		std::string STUNIp;
		std::cout << "P2P Connection would require an intermediary to establish the direct connection between clients." << '\n'; 
		std::cout << "Enter STUN Service IP : ";
		std::cin >> STUNIp;

		TCPSocketPtr STUNSocket = InitializeClientSocket(SocketAddress(STUNIp));
		if (STUNSocket != nullptr)
		{
			ProcessP2PClient(STUNSocket);
		}
	}
	else if (Mode == ECM_STUNService)
	{
		TCPSocketPtr ListenSocket = InitializeListenSocket();

		if (ListenSocket != nullptr)
		{
			ProcessSTUNService(ListenSocket);
		}
	}
	else if (Mode == ECM_Client)
	{
		std::string IpToConnect;

		std::cout << "Enter Dedicated/Listen Server IP: ";
		std::cin >> IpToConnect;

		TCPSocketPtr ClientSocket = InitializeClientSocket(SocketAddress(IpToConnect));

		if (ClientSocket != nullptr)
		{
			std::vector<TCPSocketPtr> ClientSockets;
			ClientSockets.push_back(ClientSocket);
			ProcessClientChat(ClientSockets, nullptr);
		}
	}

	Result = WSACleanup();
	if (Result == SOCKET_ERROR)
	{
		//Print error code;
		Result = WSAGetLastError();
	}
}