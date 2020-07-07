#include <iostream>
#include "Sockets\TCPSocket.h"
#include "Sockets\SocketUtil.h"
#include <vector>


#define IS_SERVER true

bool bIsGameRunning = true;

void DoUDPLoop()
{
	/*UDPSocketPtr MySock = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);
	MySock->SetNonBlockingMode(true);


	while (bIsGameRunning)
	{
		char data[1500]; // MTU LIMIT?
		SocketAddress SockAddr;

		int BytesReceived = MySock->ReceiveFrom(data, sizeof(data), SockAddr);
		if (BytesReceived > 0)
		{
			//ProcessReceivedData(data, bytesReceived, socketAddress);
		}

		DoGameFrame();
	}*/
}

/*void ProcessNewClient(const TCPSocketPtr& SockPtr, const SocketAddress& SockAddr)
{
	std::cout << "New client!" << std::endl;

}

void ProcessDataFromClient(const TCPSocketPtr& SockPtr, const char* Segment, int DataReceivedLenght)
{
	std::string Msg;
	MemoryStream Reader = MemoryStream(Segment, DataReceivedLenght);
	Reader.SerializeString(Msg);

	std::cout << "Data received: " << Msg << std::endl;
}

void DoTCPLoop()
{
	NetDriver NTDriver = NetDriver();
	NTDriver.Startup();

	std::vector<TCPSocketPtr> ReadBlockSockets;
	std::vector<TCPSocketPtr> ReadableSockets;

	std::cout << "Select Mode: [0] OnlyListen [1] OnlyConnect [2] Both" << std::endl;

	int Mode;
	std::cin >> Mode;

	// Server
	TCPSocketPtr ListenSocket = nullptr;
	if (Mode == 0 || Mode == 2)
	{
		ListenSocket = SocketUtil::CreateTCPSocket(INET);
		SocketAddress ReceivingAddress(INADDR_ANY, 48000);
		if (ListenSocket->Bind(ReceivingAddress) == SOCKET_ERROR)
		{
			return;
		}

		if (ListenSocket->Listen() == SOCKET_ERROR)
		{
			return;
		}

		ReadBlockSockets.push_back(ListenSocket);
	}

	TCPSocketPtr ConnectionSocket = nullptr;
	if (Mode == 1 || Mode == 2)
	{
		std::string IpToConnect;
		std::cout << "Enter HOST IP: ";
		std::cin >> IpToConnect;

		std::cout << IpToConnect << std::endl;

		if (!IpToConnect.empty())
		{
			ConnectionSocket = SocketUtil::CreateTCPSocket(INET);
			ConnectionSocket->Connect(SocketAddress(IpToConnect.data(), 48000));

			SocketUtil::SetNonBlockingMode(ConnectionSocket, true);
		}
	}

	if (!SocketUtil::GetLastError().empty())
	{
		std::cout << SocketUtil::GetLastError() << std::endl;
	}

	std::cin.get();

	while (bIsGameRunning)
	{
		//[TODO] Another thread for input
		if (Mode == 1 || Mode == 2)
		{
			char Segment[GOOD_SEGMENT_SIZE];
			int DataReceived = ConnectionSocket->Receive(Segment, GOOD_SEGMENT_SIZE);
			if (DataReceived > 0)
			{
				ProcessDataFromClient(ConnectionSocket, Segment, DataReceived);
			}

			std::cout << "Select Action: ProcessFrame [0] Chat [1]" << std::endl;

			int Action;
			std::cin >> Action;

			if (Action == 1)
			{
				std::string Msg;
				std::cin >> Msg;
				std::cout << std::endl;

				if (!Msg.empty())
				{
					MemoryStream Writer = MemoryStream();
					Writer.SerializeString(Msg);

					std::cout << "Sending: " << Msg << std::endl;

					// +1 because I want to send the null termination of the string.
					ConnectionSocket->Send(Writer.GetBufferPtr(), Writer.GetLength());
				}
			}
		}

		std::cout << "PROCESS FRAME" << std::endl;
		
		//[TODO] Another thread for select, remove 0 timeout.
		if (SocketUtil::Select(&ReadBlockSockets, &ReadableSockets, nullptr, nullptr, nullptr, nullptr))
		{
			for (const TCPSocketPtr& Socket : ReadableSockets)
			{
				if (ListenSocket != nullptr && Socket == ListenSocket)
				{
					//It's the listen socket, accept a new connection
					SocketAddress NewClientAddress;
					auto NewSocket = Socket->Accept(NewClientAddress);
					ReadBlockSockets.push_back(NewSocket);
					ProcessNewClient(NewSocket, NewClientAddress);
				}
				else
				{
					//It's a regular socket-process the data...
					char Segment[GOOD_SEGMENT_SIZE];
					int DataReceived = Socket->Receive(Segment, GOOD_SEGMENT_SIZE);
					if (DataReceived > 0)
					{
						ProcessDataFromClient(Socket, Segment, DataReceived);

						for (const TCPSocketPtr& Socket : ReadableSockets)
						{
							if (Socket == ListenSocket)
							{
								continue;
							}

							Socket->Send(Segment, DataReceived);
						}
					}

				}
			}
		}
	}

	NTDriver.Shutdown();
}*/

int main()
{

}

