#pragma once
#include "SocketTypes.h"

class TCPSocket
{
public:
	~TCPSocket()
	{
		closesocket(mSocket);
	}

	int Connect(const SocketAddress& InAddress);
	int Bind(const SocketAddress& InToAddress);
	int Listen(int InBacklog = 32);
	std::shared_ptr<TCPSocket> Accept(SocketAddress& InFromAddress);
	int Send(const void* InData, int InLen);
	int Receive(void* InBuffer, int InLen);
private:

	friend class SocketUtil;
	TCPSocket(SOCKET InSocket) :
		mSocket(InSocket)	
	{

	}

	SOCKET mSocket; 
};
typedef std::shared_ptr<TCPSocket> TCPSocketPtr;
