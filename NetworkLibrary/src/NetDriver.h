#pragma once
#include <vector>
#include <unordered_set>
#include "Sockets/UDPSocket.h"
#include "Sockets/SocketTypes.h"
#include "NetTypes.h"
#include <memory>
#include "Serialization/LinkingContext.h"

class MemoryBitStream;
class UObject;

enum class ENetMode : int
{
	ENM_DedicatedServer = 0 ,
	ENM_Client = 1
};

class NetDriver
{
public: 

	void Init();
	void Shutdown();

	void Tick();

private:

	LinkingContext mLinkingContext;

	void ReceiveReplicatedWorldState(MemoryBitStream& InStream);
	void ReplicateWorldState(MemoryBitStream& InStream);
	void ReplicateObjectIntoStream(MemoryBitStream& InStream, UObject* InObject, ReplicationAction InRA);

	void ReceiveReplicatedObject(MemoryBitStream& InStream);

	std::unordered_set<std::unique_ptr<UObject>> mWorldObjects;

	UDPSocketPtr NetSocket;
	SocketAddress ServerAddress;
	std::vector<SocketAddress> ClientsAddress;
	ENetMode NetMode;

	void InitNetSocket(ENetMode NetMode);
};