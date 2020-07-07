#pragma once

#define CLASS_ID(InCode,InClass)\
enum{kClassId = InCode};\
virtual uint32_t GetClassId() const { return kClassId; }\
static UObject* CreateInstance() { return new InClass(); }

class UObject
{
	CLASS_ID('UOBJ', UObject)

};