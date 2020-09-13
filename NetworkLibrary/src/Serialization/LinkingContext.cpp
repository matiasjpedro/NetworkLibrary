#include "LinkingContext.h"
#include <iostream>

uint32_t LinkingContext::GetNetId(UObject* InObject, bool InShouldCreateIfNotFound)
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

UObject* LinkingContext::GetObjectByNetId(uint32_t InNetId)
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

void LinkingContext::RemoveObjectById(uint32_t InNetId)
{
	auto It = mNetIdToObjectMap.find(InNetId);
	if (It != mNetIdToObjectMap.end())
	{
		mObjectToNetIdMap.erase(It->second);
		mNetIdToObjectMap.erase(InNetId);
	}
	else
	{
		std::cout << "Didn't found any Object by Id: " << InNetId << '\n';
	}
}

