#pragma once
#include "Sockets/UDPSocket.h"
#include "Sockets/SocketTypes.h"
#include <vector>

enum class EInstanceMode : int{
	EIM_PeerClient = 0,
	EIM_StunServer = 1,
};

enum class EPeerConnectionMode : int {
	EPCM_DirectPeerIp = 0,
	EPCM_StunServer = 1,
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
	SocketAddress stun_server_address;

	EPeerConnectionMode PeerConnectionMode;
	int bytes_per_second;

	int bytes_received;
	int bytes_sent;

	int frequency;

	float accumulated_time;

	void init_instance_Socket(EInstanceMode instance_mode, const int port);

	PeerConnection* get_peer_connection_by_address(const SocketAddress address);
};