#pragma once
#include "Sockets/UDPSocket.h"
#include "Sockets/SocketTypes.h"
#include <vector>

enum class EInstanceMode : int{
	EIM_PeerClient_Direct = 0,
	EIM_PeerClient_Stun = 1,
	EIM_StunServer = 2
};

struct PeerConnection {

	PeerConnection(){}
	PeerConnection(const SocketAddress in_peer_address) 
		: peer_address(in_peer_address)
	{
		connected = false;
	}

	SocketAddress peer_address;
	bool connected;
};

struct TrafficTester {
	void init();
	void shutdown();

	void update();

	UDPSocketPtr instance_socket;
	EInstanceMode instance_mode;

	std::vector<PeerConnection> peer_connections;

	int bytes_per_second;

	int bytes_received;
	int bytes_sent;

	int frequency;

	float accumulated_time;

	void init_instance_Socket(EInstanceMode instance_mode, const int port);
};