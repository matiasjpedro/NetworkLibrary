#pragma once
#include <vector>
#include <unordered_set>

class MemoryBitStream;
class UObject;

class NetDriver
{
public: 
	void ReplicateWorldState(MemoryBitStream& InStream, const std::vector<UObject*>& InWorldObjects);
	void ReceiveReplicatedWorldState(MemoryBitStream& InStream);

private:

	class LinkingContext* mLinkingContextPtr;

	void ReplicateObjectIntoStream(MemoryBitStream& InStream, UObject* InObject);
	UObject* ReceiveReplicatedObject(MemoryBitStream& InStream);

	std::unordered_set<UObject*> mReplicatedObjects;
};