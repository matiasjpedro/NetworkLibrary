#pragma once
#include "Reflection.h"

class SerializableExample 
{
public:
	std::string mName;
	int mSomeCount;
	float mHealth;
	bool mIsAlive;

	static DataType* sDataType;
	
	static void InitDataType()
	{
		sDataType = new DataType(
		{
			MemberVariable("mName", EPT_String, OffsetOf(SerializableExample, mName)),
			MemberVariable("mSomeCount", EPT_Int, OffsetOf(SerializableExample, mSomeCount)),
			MemberVariable("mHealth", EPT_Float, OffsetOf(SerializableExample, mHealth)),
			MemberVariable("mIsAlive", EPT_Bool, OffsetOf(SerializableExample, mIsAlive))
		});
	}

};