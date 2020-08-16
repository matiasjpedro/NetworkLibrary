#include "LinkingContext.h"
#include "../UObject.h"

uint32_t LinkingContext::GetNetworkId(UObject* InObject, bool InShouldCreateIfNotFound)
{
	auto It = mObjectToNetIdMap.find(InObject);
	if (It != mObjectToNetIdMap.end())
	{
		return It->second;
	}
	else if (InShouldCreateIfNotFound)
	{
		uint32_t NewNetId = mNextNetowrkId++;
		AddObject(InObject, NewNetId);
		return NewNetId;
	}
	
	return 0;
}

UObject* LinkingContext::GetObject(uint32_t InNetId)
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

