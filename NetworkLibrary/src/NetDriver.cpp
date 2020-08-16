#include "NetDriver.h"
#include "NetTypes.h"
#include "Serialization/MemoryBitStream.h"
#include "Serialization/ISerializableObject.h"
#include "Serialization/LinkingContext.h"
#include "UObject.h"
#include "ObjectCreationRegistry.h"

void NetDriver::ReplicateWorldState(MemoryBitStream& InStream, const std::vector<UObject*>& InWorldObjects)
{
	const PacketType Package = PacketType::PT_ReplicationData; 
	InStream.SerializeBits((void*)&Package, GetRequiredBits<(int)PacketType::PT_MAX>::Value);

	for (UObject* WorldObject : InWorldObjects)
	{
		ReplicateObjectIntoStream(InStream, WorldObject);
	}
}

void NetDriver::ReceiveReplicatedWorldState(MemoryBitStream& InStream)
{
	std::unordered_set<UObject*> ReceivedObjects;

	while (InStream.GetRemainingDataSize() > 0)
	{
		UObject* ReceivedObject = ReceiveReplicatedObject(InStream);
		ReceivedObjects.insert(ReceivedObject);
	}

	// [CHANGE] Really naive and basic approach, assuming I'll receive the state of all the actors
	for (UObject* Obj : mReplicatedObjects)
	{
		if (ReceivedObjects.find(Obj) != ReceivedObjects.end() && !Obj->IsPendingToKill())
		{
			mLinkingContextPtr->RemoveObject(Obj);
			Obj->Destroy();
		}
	}

	mReplicatedObjects = std::move(ReceivedObjects);
}

void NetDriver::ReplicateObjectIntoStream(MemoryBitStream& InStream, UObject* InObject)
{
	//Write unique id
	InStream << mLinkingContextPtr->GetNetworkId(InObject, true);

	//Write class
	InStream << InObject->GetClassId();

	//Write data
	InStream << *static_cast<ISerializableObject*>(InObject);
}

UObject* NetDriver::ReceiveReplicatedObject(MemoryBitStream& InStream)
{
	uint32_t NetId;
	uint32_t ClassId;

	InStream << NetId;
	InStream << ClassId;

	UObject* Obj = mLinkingContextPtr->GetObject(NetId);
	if (!Obj)
	{
		Obj = ObjectCreationRegistry::Get().CreateObject(ClassId);
		mLinkingContextPtr->AddObject(Obj, NetId);
	}

	InStream << *static_cast<ISerializableObject*>(Obj);

	return Obj;
}

