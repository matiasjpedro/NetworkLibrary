#include "UDPSocket.h"
#include "SocketUtil.h"

int UDPSocket::Bind(const SocketAddress& InToAddress)
{
	int Result = bind(mSocket, &InToAddress.mSockAddr, InToAddress.GetSize());

	if (Result == SOCKET_ERROR)
	{
		SocketUtil::ReportError("UDPSocket::Bind");
		return SOCKET_ERROR;
	}

	return NO_ERROR;
}

int UDPSocket::SendTo(const void* InData, int InLen, const SocketAddress& InTo)
{
	int ByteSentCount = sendto(mSocket, static_cast<const char*>(InData), InLen, 0, &InTo.mSockAddr, InTo.GetSize());
	if (ByteSentCount == SOCKET_ERROR)
	{
		SocketUtil::ReportError("UDPSocket::SendTo");
		return SOCKET_ERROR;
	}

	return ByteSentCount;
}

int UDPSocket::ReceiveFrom(void* InBuffer, int InLen, SocketAddress& OutFrom)
{
	int FromLenght = OutFrom.GetSize();
	int ReadByteCount = recvfrom(mSocket, static_cast<char*>(InBuffer), MaxLenghtPacket, 0, &OutFrom.mSockAddr, &FromLenght);

	if (ReadByteCount == SOCKET_ERROR)
	{
		SocketUtil::ReportError("UDPSocket::ReceiveFrom");
		return SOCKET_ERROR;
	}
	
	return ReadByteCount;
}