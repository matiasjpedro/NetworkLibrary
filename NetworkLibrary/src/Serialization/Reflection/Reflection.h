#pragma once
#include "glm/fwd.hpp"
#include <string>

#define OffsetOf(ClassType, MemberVar) ((size_t) & ((static_cast<ClassType*>(nullptr))->MemberVar))

enum EPrimitiveType
{
	EPT_Int,
	EPT_String,
	EPT_Float,
	EPT_Bool
};

class MemberVariable
{
public:

	MemberVariable(const char* InName, EPrimitiveType InPrimitiveType, uint32_t InOffset) 
		:	mName(InName),
			mPrimitiveType(InPrimitiveType),
			mOffset(InOffset)
	{
			
	}

	EPrimitiveType	GetPrimitiveType()	const { return mPrimitiveType; }
	uint32_t		GetOffset()			const { return mOffset; }

private:

	std::string		mName;
	EPrimitiveType	mPrimitiveType;
	uint32_t		mOffset;

};

class DataType
{
public:
	DataType(std::initializer_list<MemberVariable> InMVs)
		: mMemberVariables(InMVs)
	{

	}

	const std::vector<MemberVariable>& GetMemberVariables() const 
	{
		return mMemberVariables;
	}

private:

	std::vector<MemberVariable> mMemberVariables;
};