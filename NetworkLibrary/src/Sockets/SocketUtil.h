#pragma once
#include "SocketTypes.h"
#include "UDPSocket.h"
#include "TCPSocket.h"
#include <vector>



class SocketUtil
{
public:
	static UDPSocketPtr CreateUDPSocket(SocketAddressFamily InFamily);
	static TCPSocketPtr CreateTCPSocket(SocketAddressFamily InFamily);
	static fd_set* FillSetFromVector(fd_set& OutSet, const std::vector<TCPSocketPtr>* InSockets);
	static void FillVectorFromSet(std::vector<TCPSocketPtr>* OutSockets, const std::vector<TCPSocketPtr>* InSockets, const fd_set&InSet);
	
	static int Select(const std::vector<TCPSocketPtr>* InReadSet,
		std::vector<TCPSocketPtr>* OutReadSet,
		const std::vector<TCPSocketPtr>* InWriteSet,
		std::vector<TCPSocketPtr>* OutWriteSet,
		const std::vector<TCPSocketPtr>* InExecptSet,
		std::vector<TCPSocketPtr>* OutExceptSet);

	template<typename T>
	static int SetNonBlockingMode(T SocketPtr, bool InShouldBeNonBlocking);

	static std::string GetLastError() { return LastError; }
	
private:
	static std::string LastError;

	static void ReportError(std::string InError) { LastError = InError; }

	friend class UDPSocket;
	friend class TCPSocket;
};

template<typename T>
int SocketUtil::SetNonBlockingMode(T SocketPtr, bool InShouldBeNonBlocking)
{
#if _WIN32
	u_long Arg = InShouldBeNonBlocking ? 1 : 0;
	int Result = ioctlsocket(SocketPtr->mSocket, FIONBIO, &Arg);
#else
	int Flags = fcntl(mSocket, F_GETFL, 0);
	Flags = InShouldBeNonBlocking ? (Flags | O_NONBLOCK) : (Flags & ~O_NONBLOCK);
	fcntl(SocketPtr->mSocket, F_SETFL, Flags);
#endif

	if (Result == SOCKET_ERROR)
	{
		SocketUtil::ReportError("SocketUtil::SetNonBlockingMode");
		return SOCKET_ERROR;
	}
	else
	{
		return NO_ERROR;
	}
}
