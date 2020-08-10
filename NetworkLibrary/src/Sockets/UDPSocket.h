#pragma once
#include "SocketTypes.h"

class UDPSocket
{
public:
	~UDPSocket()
	{
		closesocket(mSocket);
	}
	int Bind(const SocketAddress& InToAddress);
	int SendTo(const void* InData, int InLen, const SocketAddress& InTo);
	int ReceiveFrom(void* InBuffer, int InLen, SocketAddress& OutFrom);

private:
	SOCKET mSocket;

	UDPSocket(SOCKET InSocket) : 
		mSocket(InSocket)
	{

	}

	friend class SocketUtil;
};
typedef std::shared_ptr<UDPSocket> UDPSocketPtr;
