#pragma once
#include "WinSock2.h"
#include <iostream>
#include <vector>
#include "Sockets/TCPSocket.h"
#include "Sockets/SocketUtil.h"
#include "Serialization/MemoryStream.h"
#include <thread>
#include <unordered_map>

// -2 because those are reserved for the length.
static const int MAX_CHAT_LENGTH = 255 - 2;
const std::string MATCHMAKING_ADDRESS_STR = "190.191.234.248:27015";

enum EChatMode : int 
{
	ECM_DedicatedServer,
	ECM_P2PClient,
	ECM_MatchmakingService,
	ECM_Client
	/*
	ECM_STUNServer,
	ECM_ListenServer,
	*/
};

std::string Nickname;

static std::string GetInput()
{
	std::cout << "Chat: ";

	std::string Input;
	std::cin >> Input;
	return Input;
}

void GetMsg(std::string& OutMsg)
{
	while (true)
	{
		std::string tmp;
		std::getline(std::cin, tmp);

		if (OutMsg.empty())
		{
			OutMsg = tmp;
		}
	}
}

void ProcessNewClient(const TCPSocketPtr& SockPtr, const SocketAddress& SockAddr)
{
	std::cout << "New client! IP:" << SockAddr.ToString() << std::endl;
}

void ProcessDataFromClient(const TCPSocketPtr& SockPtr, const char* Segment, int DataReceivedLenght)
{
	std::string Msg;
	MemoryStream Reader = MemoryStream(Segment, DataReceivedLenght);
	Reader.SerializeString(Msg);

	std::cout << Msg << std::endl;
}

TCPSocketPtr InitializeListenSocket()
{
	TCPSocketPtr ListenSocket = nullptr;
	int SocketResult = -1;

	while (SocketResult != NO_ERROR)
	{
		std::cout << "Select listening port: ";

		int ListeningPort;
		std::cin >> ListeningPort;

		ListenSocket = SocketUtil::CreateTCPSocket(INET);
		SocketAddress ReceivingAddress(INADDR_ANY, ListeningPort);

		if (ListenSocket != nullptr)
		{
			SocketResult = ListenSocket->Bind(ReceivingAddress) == NO_ERROR && ListenSocket->Listen() == NO_ERROR ? NO_ERROR : SOCKET_ERROR;
		}
	}

	std::cout << "Server initialized successfully" << std::endl;
	std::cout << "Listening..." << std::endl;

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
			std::cout << "Trying to connect to: " << SocketAddr.ToString() << std::endl;
			SocketResult = ClientSocket->Connect(SocketAddr);

			if (SocketResult == SOCKET_ERROR)
			{
				int LastErrorCode = WSAGetLastError();
				std::cout << "Unable to connect, error code: " << LastErrorCode << std::endl;
			}
		}
	}

	std::cout << "Client initialized successfully" << std::endl;

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

void ProcessClientChat(std::vector<TCPSocketPtr> ClientSockets)
{
	std::cout << "Enter your nickname: ";
	std::cin >> Nickname;

	std::string ChatMsg;
	std::thread T1(GetMsg, std::ref(ChatMsg));

	while (true)
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

		if (!ChatMsg.empty())
		{
			std::string Msg;
			Msg.append(Nickname);
			Msg.append(": ");
			Msg.append(ChatMsg);

			MemoryStream Writer = MemoryStream();
			Writer.SerializeString(Msg);

			for (const TCPSocketPtr& ClientSocket : ClientSockets)
			{
				// +1 because I want to send the null termination of the string.
				ClientSocket->Send(Writer.GetBufferPtr(), Writer.GetLength());
			}
			
			ChatMsg.clear();
		}
	}
}

void ProcessP2PClient(TCPSocketPtr MatchmakingSocket)
{
	std::vector<TCPSocketPtr> OtherClientsConnections;

	std::cout << "Waiting other players to join..." << std::endl;

	while (true)
	{
		//255.255.255.255:65555 = 22 + 1(nulltermination) + 2(length of the string)
		char Segment[25];
		int DataReceived = MatchmakingSocket->Receive(Segment, 25);
		if (DataReceived > 0)
		{
			std::string OtherClientIP;
			MemoryStream Reader = MemoryStream(Segment, DataReceived);

			Reader.SerializeString(OtherClientIP);

			std::cout << "Client Join MM: " << OtherClientIP << std::endl;

			TCPSocketPtr ClientSocket = InitializeClientSocket(SocketAddress(OtherClientIP));

			OtherClientsConnections.push_back(ClientSocket);
		}

		if (OtherClientsConnections.size() > 0)
		{
			ProcessClientChat(OtherClientsConnections);
		}
	}
}

void ProcessMatchmakingService(TCPSocketPtr ListenSocket)
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

					std::cout << "New Client registered in the Matchmaking: " << NewClientAddrStr << std::endl;

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

							std::cout << "Send new client address to already connected client: " << NewClientAddrStr << std::endl;

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

						std::cout << "Send already connected client address to the new client: " << ClientAddrStr << std::endl;

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

	std::cout << "Select Mode: [0] DedicatedServer [1] P2PClient [2] Matchmaking Service [3] Client:  ";

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
	else if (Mode == ECM_P2PClient)
	{
		std::string MatchmakingIp;
		std::cout << "Enter Matchmaking Service IP: ";
		std::cin >> MatchmakingIp;

		TCPSocketPtr MatchmakingSocket = InitializeClientSocket(SocketAddress(MatchmakingIp));
		if (MatchmakingSocket != nullptr)
		{
			ProcessP2PClient(MatchmakingSocket);
		}
	}
	else if (Mode == ECM_MatchmakingService)
	{
		TCPSocketPtr ListenSocket = InitializeListenSocket();

		if (ListenSocket != nullptr)
		{
			ProcessMatchmakingService(ListenSocket);
		}
	}
	else if (Mode == ECM_Client)
	{
		std::string IpToConnect;

		std::cout << "Enter Dedicated Server IP: ";
		std::cin >> IpToConnect;

		TCPSocketPtr ClientSocket = InitializeClientSocket(SocketAddress(IpToConnect));

		if (ClientSocket != nullptr)
		{
			std::vector<TCPSocketPtr> ClientSockets;
			ClientSockets.push_back(ClientSocket);
			ProcessClientChat(ClientSockets);
		}
	}

	Result = WSACleanup();
	if (Result == SOCKET_ERROR)
	{
		//Print error code;
		Result = WSAGetLastError();
	}
}