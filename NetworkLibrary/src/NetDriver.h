#pragma once
#include <vector>
#include <unordered_set>
#include "Sockets/UDPSocket.h"
#include "Sockets/SocketTypes.h"
#include "NetTypes.h"

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

	std::shared_ptr<class LinkingContext> mLinkingContextPtr;

	void ReceiveReplicatedWorldState(MemoryBitStream& InStream);
	void ReplicateWorldState(MemoryBitStream& InStream, const std::vector<UObject*>& InWorldObjects);
	void ReplicateObjectIntoStream(MemoryBitStream& InStream, UObject* InObject, ReplicationAction InRA);

	UObject* ReceiveReplicatedObject(MemoryBitStream& InStream);

	std::unordered_set<UObject*> mReplicatedObjects;

	UDPSocketPtr NetSocket;
	SocketAddress ServerAddress;
	std::vector<SocketAddress> ClientsAddress;
	ENetMode NetMode;

	void InitNetSocket(ENetMode NetMode);
};