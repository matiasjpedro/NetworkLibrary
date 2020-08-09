#pragma once
#include <stdint.h>
#include <unordered_map>
#include "../UObject.h"


class LinkingContext
{
public:
	uint32_t GetNetworkId(UObject* InObject, bool InShouldCreateIfNotFound)
	{
		auto It = mObjectToNetIdMap.find(InObject);
		if (It != mObjectToNetIdMap.end())
		{
			return It->second;
		}
		else if(InShouldCreateIfNotFound)
		{
			uint32_t NewNetId = mNextNetowrkId++;
			AddObject(InObject, NewNetId);
			return NewNetId;
		}
	}

	UObject* GetObject(uint32_t InNetId)
	{
		auto It = mNetIdToObjectMap.find(InNetId);
		if (It != mNetIdToObjectMap.end())
		{
			return It->second;
		}
		else
		{
			return nullptr;
		}
	}

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

	uint32_t mNextNetowrkId;
};
