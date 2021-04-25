#include "TrafficTester.h"
#include "Serialization/MemoryBitStream.h"
#include "NetTypes.h"
#include "Sockets/SocketUtil.h"
#include <chrono>

static constexpr int MTU_LIMIT_BYTES = 1300;

void TrafficTester::init()
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result == SOCKET_ERROR)
	{
		std::cout << "Start up error! " << GetLastError();
		return;
	}

	/*std::cout << "Select mode: [0] PeerClient_Direct (uses direct peer ip) [1] PeerClient_Stun (uses stun server ip, not implemented)";

	int get_input;
	std::cin >> get_input;*/

	instance_mode = EInstanceMode::EIM_PeerClient_Direct; //(EInstanceMode)get_input;

	if (instance_mode != EInstanceMode::EIM_StunServer) {
		std::cout << "Insert amount of bytes per second that you would like to send:  ";

		bytes_per_second;
		std::cin >> bytes_per_second;

		std::cout << "Insert frequency (times per second):  ";

		frequency;
		std::cin >> frequency;
	}

	std::cout << "Insert port number that you would like to be use by this application: ";

	int port;
	std::cin >> port;

	init_instance_Socket(instance_mode, port);

	if (instance_mode == EInstanceMode::EIM_PeerClient_Direct)
	{
		std::string ip_to_connect;

		std::cout << "Insert peer ip address (including port, requires port forwarding) to which you would like to send data: ";
		std::cin >> ip_to_connect;

		SocketAddress peer_address(ip_to_connect);

		peer_connections.push_back(peer_address);

		MemoryBitStream write_stream = MemoryBitStream();

		PacketType package_type = PacketType::PT_Hello;
		write_stream.SerializeBits(&package_type, 8);

		instance_socket->SendTo(write_stream.GetBufferPtr(), write_stream.GetLengthInBytes(), peer_address);
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

	std::cout << "Waiting for peer connection" << std::endl;

	while (true)
	{
		auto current_time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> elapsed_time = current_time - previous_time;
		previous_time = current_time;
			
		if (instance_socket != nullptr)
		{
			if (instance_mode == EInstanceMode::EIM_StunServer)
			{
				//#TODO StunServer
			}
			else if (instance_mode == EInstanceMode::EIM_PeerClient_Direct)
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
							for (auto& peer_connection : peer_connections)
							{
								if (peer_connection.peer_address.ToString(false) == incoming_data_addr.ToString(false)) {
									peer_connection.connected = true;
									break;
								}
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

