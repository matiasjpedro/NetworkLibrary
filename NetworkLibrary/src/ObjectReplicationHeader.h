#pragma once
#include "NetTypes.h"
#include "Serialization/MemoryBitStream.h"

class ObjectReplicationHeader
{
public:
	ObjectReplicationHeader() :
		mReplicationAction(RA_MAX),
		mNetId(0),
		mClassId(0)
	{}

	ObjectReplicationHeader(ReplicationAction InRa, uint32_t InNetId, uint32_t InClassId = 0) :
		mReplicationAction(InRa),
		mNetId(InNetId),
		mClassId(InClassId)
	{
		
	}

	ReplicationAction mReplicationAction;
	uint32_t mNetId;
	uint32_t mClassId;

	friend MemoryBitStream& operator<<(MemoryBitStream& Ar, ObjectReplicationHeader& Obj)
	{
		Ar.SerializeBits(&Obj.mReplicationAction, GetRequiredBits<RA_MAX>::Value);
		Ar << Obj.mNetId;
		
		if (Obj.mReplicationAction != RA_Destroy)
		{
			Ar << Obj.mClassId;
		}

		return Ar;
	}
};