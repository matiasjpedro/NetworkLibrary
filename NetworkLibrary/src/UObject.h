#pragma once
#include "Serialization/ISerializableObject.h"

#define CLASS_ID(InCode,InClass)\
public: \
enum{kClassId = InCode};\
virtual uint32_t GetClassId() const { return kClassId; }\
static UObject* CreateInstance() { return new InClass(); }

class UObject : public ISerializableObject
{
	CLASS_ID('UOBJ', UObject)

public:
	virtual void Destroy() { bIsPendingToKill = true; }
	inline bool IsPendingToKill() { return bIsPendingToKill; }

	void Serialize(class MemoryStream& Stream) override {}

private:
	bool bIsPendingToKill = false;
};