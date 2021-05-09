#include "TrafficTester.h"
#include "Serialization/MemoryBitStream.h"
#include "NetTypes.h"
#include "Sockets/SocketUtil.h"
#include <chrono>

static constexpr int MTU_LIMIT_BYTES = 1300;

void TrafficTester::init()
{
	bytes_received = 0;
	bytes_sent = 0;

	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result == SOCKET_ERROR)
	{
		std::cout << "Start up error! " << GetLastError();
		return;
	}

	{
		std::cout << "Select mode [0] PeerClient [1] StunServer: " ;

		int get_input;
		std::cin >> get_input;

		instance_mode = (EInstanceMode)get_input;
	}

	std::cout << "Insert port number that you would like to be use by this application: ";

	int port;
	std::cin >> port;


	if (instance_mode == EInstanceMode::EIM_PeerClient) {

		std::cout << "Insert amount of bytes per second that you would like to send:  ";

		bytes_per_second;
		std::cin >> bytes_per_second;

		std::cout << "Insert frequency (times per second):  ";

		frequency;
		std::cin >> frequency;

		{
			std::cout << "How would you like to connect to the other peer? " << '\n';
			std::cout << "[0] Direct peer IP (requires port forward on both peers)" << '\n';
			std::cout << "[1] Stun Server (non port forward required, as the stun server will handle the nat traversal" << '\n';

			int get_input;
			std::cin >> get_input;

			PeerConnectionMode = (EPeerConnectionMode)get_input;
		}
	}

	init_instance_Socket(instance_mode, port);

	if (instance_mode == EInstanceMode::EIM_PeerClient)
	{
		std::string ip_to_connect;

		if (PeerConnectionMode == EPeerConnectionMode::EPCM_DirectPeerIp) {
			std::cout << "Insert Peer IP address (including port) to which you would like to send data: ";
		}
		else if (PeerConnectionMode == EPeerConnectionMode::EPCM_StunServer){
			std::cout << "Insert Stun Server IP address (including port) to which you would like to connect: ";
		}
		
		std::cin >> ip_to_connect;

		SocketAddress ip_address(ip_to_connect);

		if (PeerConnectionMode == EPeerConnectionMode::EPCM_DirectPeerIp) {
			peer_connections.push_back(ip_address);
		}
		else if (PeerConnectionMode == EPeerConnectionMode::EPCM_StunServer) {
			stun_server_address = ip_address;

			MemoryBitStream write_stream = MemoryBitStream();

			PacketType package_type = PacketType::PT_Hello;
			write_stream.SerializeBits(&package_type, 8);

			instance_socket->SendTo(write_stream.GetBufferPtr(), write_stream.GetLengthInBytes(), stun_server_address);
		}
	}
}

void TrafficTester::shutdown()
{
	int result = WSACleanup();
	if (result == SOCKET_ERROR)
	{
		std::cout << "Shutdown up error! " << GetLastError();
		//Print error code;
		result = WSAGetLastError();
	}
}

void TrafficTester::update()
{
	auto previous_time = std::chrono::high_resolution_clock::now();
	float accumulated_byte_amount = 0.f;

	while (true)
	{
		auto current_time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> elapsed_time = current_time - previous_time;
		previous_time = current_time;
			
		if (instance_socket != nullptr)
		{
			if (instance_mode == EInstanceMode::EIM_StunServer)
			{
				char segment[MTU_LIMIT_BYTES];

				//Receive
				{
					SocketAddress incoming_data_addr;
					int incoming_bytes = instance_socket->ReceiveFrom(segment, MTU_LIMIT_BYTES, incoming_data_addr);

					if (incoming_bytes > 0)
					{
						MemoryBitStream read_stream = MemoryBitStream(segment, incoming_bytes * 8);

						PacketType incoming_packet_type = PT_MAX;
						read_stream.SerializeBits(&incoming_packet_type, 8);

						if (incoming_packet_type == PacketType::PT_Hello) {
							std::cout << "Received Hello from: " << incoming_data_addr.ToString() << '\n';
							PeerConnection* peer_connection = get_peer_connection_by_address(incoming_data_addr);
							
							//Make sure that we didn't received this address before
							if (peer_connection == nullptr){

								const size_t peer_connections_count = peer_connections.size();

								if (peer_connections_count > 0) {
									
									//Send all addresses to the new peer
									{
										MemoryBitStream write_stream = MemoryBitStream();

										PacketType package_type = PacketType::PT_PeerAddresses;
										write_stream.SerializeBits(&package_type, 8);

										std::vector<std::string> addresses;
										addresses.reserve(peer_connections_count);

										for (const auto& peer_connection : peer_connections) {
											std::cout << "Send this peer address: " << peer_connection.peer_address.ToString() << " to the new peer: " << incoming_data_addr.ToString() << '\n';
											addresses.push_back(peer_connection.peer_address.ToString());
										}

										write_stream << addresses;

										instance_socket->SendTo(write_stream.GetBufferPtr(), write_stream.GetLengthInBytes(), incoming_data_addr);
									}

									//Send the new peer to all the addresses
									{
										MemoryBitStream write_stream = MemoryBitStream();

										PacketType package_type = PacketType::PT_PeerAddresses;
										write_stream.SerializeBits(&package_type, 8);

										std::vector<std::string> addresses = { incoming_data_addr.ToString() };
										write_stream << addresses;

										for (const auto& peer_connection : peer_connections)
										{
											std::cout << "Send new peer address: " << incoming_data_addr.ToString() << " to this peer address: " << peer_connection.peer_address.ToString() << '\n';
											instance_socket->SendTo(write_stream.GetBufferPtr(), write_stream.GetLengthInBytes(), peer_connection.peer_address);
										}
									}
								}
								
								peer_connections.push_back(incoming_data_addr);
							}

						}
					}
				}


				//#TODO StunServer
			}
			else if (instance_mode == EInstanceMode::EIM_PeerClient)
			{
				//Send
				{
					accumulated_time += elapsed_time.count();
					const float times_per_second = 1.f / (float)frequency;

					while (accumulated_time >= times_per_second)
					{
						const float bytes_to_deliver_this_frame = (float)bytes_per_second * times_per_second;
						accumulated_byte_amount += bytes_to_deliver_this_frame;

						const int bytes_to_deliver = accumulated_byte_amount;
						accumulated_byte_amount -= bytes_to_deliver;

						for (const auto& peer_connection : peer_connections)
						{
							MemoryBitStream write_stream = MemoryBitStream();

							if (!peer_connection.connected) {
								PacketType package_type = PacketType::PT_Hello;
								write_stream.SerializeBits(&package_type, 8);

								std::cout << "Sending Hello to : " << peer_connection.peer_address.ToString() << std::endl;
							}
							else {
								PacketType package_type = PacketType::PT_ReplicationData;
								write_stream.SerializeBits(&package_type, 8);

								std::vector<std::uint8_t> bunch;
								bunch.resize(bytes_to_deliver, 255);

								write_stream << bunch;
							}

							bytes_sent += write_stream.GetLengthInBytes();

							instance_socket->SendTo(write_stream.GetBufferPtr(), write_stream.GetLengthInBytes(), peer_connection.peer_address);
						}

						accumulated_time -= times_per_second;

						std::cout << "Bytes sent: " << bytes_sent << std::endl;
						std::cout << "Bytes recieved: " << bytes_received << std::endl;
					}
				}

				char segment[MTU_LIMIT_BYTES];

				//Receive
				{
					SocketAddress incoming_data_addr;
					int incoming_bytes = instance_socket->ReceiveFrom(segment, MTU_LIMIT_BYTES, incoming_data_addr);

					if (incoming_bytes > 0)
					{
						MemoryBitStream read_stream = MemoryBitStream(segment, incoming_bytes * 8);

						PacketType incoming_packet_type = PT_MAX;
						read_stream.SerializeBits(&incoming_packet_type, 8);

						//If I receive data from a peer that I was waiting then it means that I'm connected to that peer.
						for (auto& peer_connection : peer_connections)
						{
							if (peer_connection.peer_address.ToString() == incoming_data_addr.ToString()) {
								peer_connection.connected = true;
								break;
							}
							if (peer_connection.peer_address.ToString(false) == incoming_data_addr.ToString(false)) {
								//Replace peer address as the port assigned was different to the one that we though due to the router assigning other port.
								peer_connection.peer_address = incoming_data_addr;
								peer_connection.connected = true;
								break;
							}
						}

						if (incoming_packet_type == PacketType::PT_PeerAddresses) {
							//Peer address receive from the stun server, store it so we can start sending Hello packets
							std::vector<std::string> new_peer_addresses;
							read_stream << new_peer_addresses;

							MemoryBitStream write_stream = MemoryBitStream();

							PacketType package_type = PacketType::PT_Hello;
							write_stream.SerializeBits(&package_type, 8);

							for (const auto& new_peer_address : new_peer_addresses)
							{
								SocketAddress peer_socket_address = SocketAddress(new_peer_address);

								peer_connections.push_back(peer_socket_address);

								//Send first packet to make the entry in the NAT table
								instance_socket->SendTo(write_stream.GetBufferPtr(), write_stream.GetLengthInBytes(), peer_socket_address);
								std::cout << "Send hello to: " << new_peer_address << std::endl; 
							}						
						}
						else if (incoming_packet_type == PacketType::PT_ReplicationData) {

							const int bunch_remaining_bytes = read_stream.GetRemainingDataSize() / 8;

							std::vector<std::uint8_t> bunch;
							bunch.reserve(bunch_remaining_bytes);

							bytes_received += bunch_remaining_bytes;
							read_stream << bunch;
						}
					}
				}

				
			}
		}
	}

	shutdown();
}

void TrafficTester::init_instance_Socket(EInstanceMode instance_mode, const int port)
{
	int socket_result = -1;

	while (socket_result != NO_ERROR)
	{
		instance_socket = SocketUtil::CreateUDPSocket(INET);

		SocketAddress receiving_address(INADDR_ANY, port);

		if (instance_socket != nullptr)
		{
			socket_result = instance_socket->Bind(receiving_address) == NO_ERROR ? NO_ERROR : SOCKET_ERROR;

			if (socket_result == SOCKET_ERROR)
			{
				std::cout << "SocketError! " << GetLastError();
			}
		}
	}

	SocketUtil::SetNonBlockingMode(instance_socket, true);

	std::cout << "UDP socket initialized..." << '\n';
}

PeerConnection* TrafficTester::get_peer_connection_by_address(const SocketAddress address)
{
	for (std::vector<PeerConnection>::iterator it = peer_connections.begin(); it != peer_connections.end(); ++it)
	{
		if (it->peer_address.ToString() == address.ToString())
		{
			return &(*it);
		}
	}

	return nullptr;
}

