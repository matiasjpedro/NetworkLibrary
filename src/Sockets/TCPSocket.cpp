#include "TCPSocket.h"
#include "SocketUtil.h"

int TCPSocket::Connect(const SocketAddress& InAddress)
{
	int Result = connect(mSocket, &InAddress.mSockAddr, InAddress.GetSize());	

	if (Result == SOCKET_ERROR)
	{
		SocketUtil::ReportError("TCPSocket::Connect");
		return SOCKET_ERROR;
	}

	return NO_ERROR;
}

int TCPSocket::Bind(const SocketAddress& InToAddress)
{
	int Result = bind(mSocket, &InToAddress.mSockAddr, InToAddress.GetSize());
	if (Result == SOCKET_ERROR)
	{
		SocketUtil::ReportError("TCPSocket::Bind");
		return SOCKET_ERROR;
	}

	return NO_ERROR;
}

int TCPSocket::Listen(int InBacklog /*= 32*/)
{
	int Result = listen(mSocket, InBacklog);
	if (Result == SOCKET_ERROR)
	{
		SocketUtil::ReportError("TCPSocket::Listen");
		return SOCKET_ERROR;
	}

	return NO_ERROR;
}

std::shared_ptr<TCPSocket> TCPSocket::Accept(SocketAddress& InFromAddress)
{
	int Lenght = InFromAddress.GetSize();
	SOCKET NewSocket = accept(mSocket, &InFromAddress.mSockAddr, &Lenght);

	if (NewSocket != INVALID_SOCKET)
	{
		return TCPSocketPtr(new TCPSocket(NewSocket));
	}
	else 
	{
		SocketUtil::ReportError("TCPSocket::Accept");
		return nullptr;
	}
}

int TCPSocket::Send(const void* InData, int InLen)
{
	int BytesSentCount = send(mSocket, static_cast<const char*>(InData), InLen, 0);
	if (BytesSentCount == SOCKET_ERROR)
	{
		SocketUtil::ReportError("TCPSocket::Send");
		return SOCKET_ERROR;
	}

	return BytesSentCount;
}

int TCPSocket::Receive(void* InBuffer, int InLen)
{
	int BytesReceivedCount = recv(mSocket, static_cast<char*>(InBuffer), InLen, 0);

	if (BytesReceivedCount == SOCKET_ERROR)
	{
		SocketUtil::ReportError("TCPSocket::Receive");
		return SOCKET_ERROR;
	}
	
	return BytesReceivedCount;
}
