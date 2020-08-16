#pragma once
#include <unordered_map>
#include "UObject.h"
#include <assert.h>

typedef UObject* (*ObjectCreationFunc)();

class ObjectCreationRegistry
{
public:
	static ObjectCreationRegistry& Get()
	{
		static ObjectCreationRegistry sInstance;
		return sInstance;
	}

	template<typename T>
	void RegisterCreationFunction()
	{
		// ensure no duplicate class id
		assert(mNameToObjectCreationFunctionMap.find(T::kClassId) == mNameToObjectCreationFunctionMap.end());

		mNameToObjectCreationFunctionMap[T::kClassId] = T::CreateInstance;
	}

	UObject* CreateObject(uint32_t InClassId)
	{
		//Add error checking if desire for now crash if not found.
		ObjectCreationFunc CreationFunc = mNameToObjectCreationFunctionMap[InClassId];
		UObject* NewObject = CreationFunc();
		return NewObject;
	}

private:

	ObjectCreationRegistry() {}

	std::unordered_map<uint32_t, ObjectCreationFunc> mNameToObjectCreationFunctionMap;
};

void RegisterObjectCreation()
{
	ObjectCreationRegistry::Get().RegisterCreationFunction<UObject>();
}