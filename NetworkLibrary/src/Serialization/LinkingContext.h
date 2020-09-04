#pragma once
#include <stdint.h>
#include <unordered_map>

class UObject;

class LinkingContext
{
public:

	uint32_t GetNetworkId(UObject* InObject, bool InShouldCreateIfNotFound);
	UObject* GetObjectById(uint32_t InNetId);

	void AddObject(UObject* InObject, uint32_t InNetId)
	{
		mNetIdToObjectMap[InNetId] = InObject;
		mObjectToNetIdMap[InObject] = InNetId;
	}

	void RemoveObject(UObject* InObject)
	{
		uint32_t NetId = mObjectToNetIdMap[InObject];
		mObjectToNetIdMap.erase(InObject);
		mNetIdToObjectMap.erase(NetId);
	}

private:
	std::unordered_map<uint32_t, UObject*> mNetIdToObjectMap;
	std::unordered_map<UObject*, uint32_t> mObjectToNetIdMap;

	uint32_t mNextNetowrkId = 1;
};
