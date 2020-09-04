#pragma once
#include "WinSock2.h"
#include <ws2tcpip.h>
#include <stdint.h>
#include <string>
#include <memory>

//TODO: Move these to their independent files

static const int MaxLenghtPacket = 1500;

enum SocketAddressFamily
{
	INET = AF_INET,
	INET6 = AF_INET6
};

enum SocketType
{
	UDP = SOCK_DGRAM,
	TCP = SOCK_STREAM
};

class SocketAddressFactory
{
public:
	static std::shared_ptr<sockaddr> CreateIPv4DNSNameString(const std::string& inString)
	{
		auto pos = inString.find_last_of(':');
		std::string host, service;
		if (pos != std::string::npos)
		{
			host = inString.substr(0, pos);
			service = inString.substr(pos + 1);
		}
		else
		{
			host = inString;
			//use default port...
			service = "80";
		}

		addrinfo hint;
		memset(&hint, 0, sizeof(hint));
		hint.ai_family = AF_INET;
		addrinfo* result;

		//TODO GetAddrInfoEx to make this call async without manually creating a thread.
		int error = getaddrinfo(host.c_str(), service.c_str(),
			&hint, &result);
		if (error != 0 && result != nullptr)
		{
			freeaddrinfo(result);
			return nullptr;
		}

		while (!result->ai_addr && result->ai_next)
		{
			result = result->ai_next;
		}

		if (!result->ai_addr)
		{
			freeaddrinfo(result);
			return nullptr;
		}
		auto toRet = std::make_shared< sockaddr >(*result->ai_addr);
		freeaddrinfo(result);
		return toRet;
	}
};

class SocketAddress
{
public:

	//[TODO] These constructors should be private only and used by the SocketAddress  Factory
	SocketAddress(){}

	// Set address by ip and port
	SocketAddress(u_long inAddress, uint16_t inPort)
	{
		// Initialize that address with 0 otherwise some platforms could
		// have different behavior with that non initialized data.
		memset(GetAsSockAddrIn()->sin_zero, 0, sizeof(GetAsSockAddrIn()->sin_zero));

		// IPV4
		GetAsSockAddrIn()->sin_family = AF_INET;
		// Mutiplatform ip address
		GetAsSockAddrIn()->sin_addr.S_un.S_addr = htonl(inAddress);
		// Multiplatform port
		GetAsSockAddrIn()->sin_port = htons(inPort);
	}

	// Set address by string which is dotted notation
	SocketAddress(const char* ipString, uint16_t inPort)
	{
		// Initialize that address with 0 otherwise some platforms could
		// have different behavior with that non initialized data.
		memset(GetAsSockAddrIn()->sin_zero, 0, sizeof(GetAsSockAddrIn()->sin_zero));

		GetAsSockAddrIn()->sin_family = AF_INET;
		inet_pton(AF_INET, ipString, &GetAsSockAddrIn()->sin_addr);
		GetAsSockAddrIn()->sin_port = htons(inPort);
	}

	// Set address by DNSName
	SocketAddress(const std::string& DNSName)
	{
		std::shared_ptr<sockaddr> localSockAddrPtr = SocketAddressFactory::CreateIPv4DNSNameString(DNSName);

		if (localSockAddrPtr != nullptr)
		{
			mSockAddr = *SocketAddressFactory::CreateIPv4DNSNameString(DNSName);
		}
	}

	SocketAddress(const sockaddr& inSockAddr)
	{
		memcpy(&mSockAddr, &inSockAddr, sizeof(sockaddr));
	}

	bool operator==(const SocketAddress& Other)
	{
		return ToString() == Other.ToString();
	}

	int GetSize() const { return sizeof(sockaddr); }

	std::string ToString() const
	{
		const sockaddr_in* AddrIn = (const sockaddr_in*)&mSockAddr;
		
		const int MaxLen = 16; //255.255.255.255
		char s[MaxLen];

		inet_ntop(AF_INET, &(AddrIn->sin_addr),
			s, MaxLen);

		USHORT port = ntohs(AddrIn->sin_port);

		int NullTermination = 0;
		for (int i = 0; i < MaxLen; i++) {
			if (s[i] == '\00') {
				break;
			}
			NullTermination++;
		}

		std::string r(s, NullTermination);
		r += ":" + std::to_string(port);

		return r;
	}

private:
	sockaddr mSockAddr;
	sockaddr_in* GetAsSockAddrIn()
	{
		return reinterpret_cast<sockaddr_in*>(&mSockAddr);
	}

	friend class UDPSocket;
	friend class TCPSocket;
};
typedef std::shared_ptr<SocketAddress> SocketAddressPtr;


