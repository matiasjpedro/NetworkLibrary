#include <iostream>
#include "Sockets\TCPSocket.h"
#include "Sockets\SocketUtil.h"
#include <vector>

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

int main()
{

}

