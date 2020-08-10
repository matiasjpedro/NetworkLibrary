#include "SocketUtil.h"

std::string SocketUtil::LastError;

UDPSocketPtr SocketUtil::CreateUDPSocket(SocketAddressFamily InFamily)
{
	SOCKET Socket = socket(InFamily, SocketType::UDP, IPPROTO_UDP);
	if (Socket != INVALID_SOCKET)
	{
		return UDPSocketPtr(new UDPSocket(Socket));
	}
	else
	{
		ReportError("SocketUtil::CreateUDPSocket");
		return nullptr;
	}	
}

TCPSocketPtr SocketUtil::CreateTCPSocket(SocketAddressFamily InFamily)
{
	SOCKET Socket = socket(InFamily, SocketType::TCP, IPPROTO_TCP);
	if (Socket != INVALID_SOCKET)
	{
		return TCPSocketPtr(new TCPSocket(Socket));
	}
	else
	{
		ReportError("SocketUtil::CreateTCPSocket");
		return nullptr;
	}
}

fd_set* SocketUtil::FillSetFromVector(fd_set& OutSet, const std::vector<TCPSocketPtr>* InSockets)
{
	if (InSockets)
	{
		//Initialize and empty fd_set before fill it.
		FD_ZERO(&OutSet);
		for (const TCPSocketPtr& Socket : *InSockets)
		{
			FD_SET(Socket->mSocket, &OutSet);
		}

		return &OutSet;
	}
	else
	{
		return nullptr;
	}
}

void SocketUtil::FillVectorFromSet(std::vector<TCPSocketPtr>* OutSockets, const std::vector<TCPSocketPtr>* InSockets, const fd_set&InSet)
{
	if (InSockets && OutSockets)
	{
		OutSockets->clear();
		for (const TCPSocketPtr& Socket : *InSockets)
		{
			if (FD_ISSET(Socket->mSocket, &InSet))
			{
				OutSockets->push_back(Socket);
			}
		}
	}
}

int SocketUtil::Select(const std::vector<TCPSocketPtr>* InReadSet, std::vector<TCPSocketPtr>* OutReadSet, const std::vector<TCPSocketPtr>* InWriteSet, std::vector<TCPSocketPtr>* OutWriteSet, const std::vector<TCPSocketPtr>* InExecptSet, std::vector<TCPSocketPtr>* OutExceptSet)
{
	//Build up some sets from our vectors;
	fd_set ReadSet, WriteSet, ExceptSet;

	fd_set* ReadSetPtr = FillSetFromVector(ReadSet, InReadSet);
	fd_set* WriteSetPtr = FillSetFromVector(WriteSet, InWriteSet);
	fd_set* ExceptSetPtr = FillSetFromVector(ExceptSet, InExecptSet);

	timeval timeout;
	timeout.tv_sec = 0;
	int Result = select(0, ReadSetPtr, WriteSetPtr, ExceptSetPtr, nullptr);

	if (Result > 0)
	{
		FillVectorFromSet(OutReadSet, InReadSet, ReadSet);
		FillVectorFromSet(OutWriteSet, InWriteSet, WriteSet);
		FillVectorFromSet(OutExceptSet, InExecptSet, ExceptSet);
	}
	
	return Result;
}

