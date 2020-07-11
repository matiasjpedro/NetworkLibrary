#pragma once
#include "UnitTesting/Testeable.h"
#include "../Serialization/MemoryStream.h"
#include "../Serialization/ICustomSerializable.h"
#include "../Serialization/Reflection/Reflection.h"
#include "../Serialization/Compression.h"
#include "../Serialization/MemoryBitStream.h"
#include "glm/vec3.hpp"
#include "glm/ext/quaternion_float.hpp"

#pragma region ITesteable
#define SERIALIZATION_TESTS_ENUM(DO) \
    DO(TEST_SERIALIZE_PRIMITIVE) \
    DO(TEST_SERIALIZE_PRIMITIVE_VECTOR) \
	DO(TEST_SERIALIZE_STRING) \
    DO(TEST_SERIALIZE_STRING_VECTOR) \
	DO(TEST_SERIALIZE_CUSTOMSERIALIZABLE) \
	DO(TEST_SERIALIZE_CUSTOMSERIALIZABLE_VECTOR) \
	DO(TEST_SERIALIZE_GENERICSERIALIZABLE) \
	DO(TEST_SERIALIZE_VECTOR3) \
	DO(TEST_SERIALIZE_QUATERNION) 


MAKE_LOGGABLE_ENUM(SERIALIZATION_TESTS_ENUM, ESerializationTests)
#pragma endregion

class CustomSerializableObject : public ICustomSerializable
{

public:

	uint32_t Health;

	CustomSerializableObject() :
		Health(5)
	{

	}

	void Serialize(class MemoryStream* Stream) override
	{
		Stream->SerializePrim(Health);
	}
};

// This could be code generated adding some tags in the class that I want to serialize
class GenericSerializableObject
{
public:
	//std::string mName;
	int mSomeCount;
	float mHealth;
	bool mIsAlive;

	static DataType* sDataType;

	static void InitDataType()
	{
		sDataType = new DataType(
			{
				/*MemberVariable("mName", EPT_String, OffsetOf(GenericSerializableObject, mName)),*/
				MemberVariable("mSomeCount", EPT_Int, OffsetOf(GenericSerializableObject, mSomeCount)),
				MemberVariable("mHealth", EPT_Float, OffsetOf(GenericSerializableObject, mHealth)),
				MemberVariable("mIsAlive", EPT_Bool, OffsetOf(GenericSerializableObject, mIsAlive))
			});
	}
};

DataType* GenericSerializableObject::sDataType;

template<typename T>
bool TestPrimitive()
{
	uint32_t WriteInt = 13;
	T WriteStream = T();
	WriteStream.SerializePrim(WriteInt, 2);
	WriteStream.SerializePrim(WriteInt, 4);

	uint32_t ReadInt = 0;
	uint32_t ReadInt2 = 0;
	T ReadStream = T(WriteStream.GetBufferPtr(), WriteStream.GetLength());
	ReadStream.SerializePrim(ReadInt, 2);
	ReadStream.SerializePrim(ReadInt2, 4);

	return ReadInt == WriteInt && ReadInt2 == WriteInt;
}

template<typename T>
bool TestPrimitiveVector()
{
	std::vector<uint32_t> WriteVector;
	WriteVector.push_back(13);
	WriteVector.push_back(13);

	T WriteStream = T();
	WriteStream.SerializePrimArr(WriteVector, 2);

	std::vector<uint32_t> ReadVector;

	T ReadStream = T(WriteStream.GetBufferPtr(), WriteStream.GetLength());
	ReadStream.SerializePrimArr(ReadVector, 2);

	return ReadVector[0] == WriteVector[0] && ReadVector[1] == WriteVector[1];
}

template<typename T>
bool TestString()
{
	std::string WriteString = std::string("Test");
	T WriteStream = T();
	WriteStream.SerializeString(WriteString);

	std::string ReadString;
	T ReadStream = T(WriteStream.GetBufferPtr(), WriteStream.GetLength());
	ReadStream.SerializeString(ReadString);

	return WriteString.compare(ReadString);
}

template<typename T>
bool TestStringVector()
{
	std::vector<std::string> WriteVector;
	WriteVector.push_back(std::string("Test1"));
	WriteVector.push_back(std::string("Test2"));

	T WriteStream = T();
	WriteStream.SerializeStringArr(WriteVector);

	std::vector<std::string> ReadVector;
	T ReadStream = T(WriteStream.GetBufferPtr(), WriteStream.GetLength());
	ReadStream.SerializeStringArr(ReadVector);

	return WriteVector[0].compare(ReadVector[0]) && WriteVector[1].compare(ReadVector[1]);
}

template<typename T>
bool TestCustomSerializable()
{
	CustomSerializableObject WriteSerializableActor;
	T WriteStream = T();
	WriteStream.SerializeCustom(WriteSerializableActor);

	CustomSerializableObject ReadSerializableActor;
	T ReadStream = T(WriteStream.GetBufferPtr(), WriteStream.GetLength());
	ReadStream.SerializeCustom(ReadSerializableActor);

	return WriteSerializableActor.Health == ReadSerializableActor.Health;
}

template<typename T>
bool TestCustomSerializableVector()
{
	std::vector<CustomSerializableObject> WriteVector;
	WriteVector.push_back(CustomSerializableObject());
	WriteVector.push_back(CustomSerializableObject());

	T WriteStream = T();
	WriteStream.SerializeCustomArr(WriteVector);

	std::vector<CustomSerializableObject> ReadVector;
	T ReadStream = T(WriteStream.GetBufferPtr(), WriteStream.GetLength());
	ReadStream.SerializeCustomArr(ReadVector);

	return ReadVector[0].Health == WriteVector[0].Health && ReadVector[1].Health == WriteVector[1].Health;
}

template<typename T>
bool TestGenericSerializable()
{
	GenericSerializableObject WriteObj;

	if (WriteObj.sDataType == nullptr)
	{
		WriteObj.InitDataType();
	}

	WriteObj.mHealth = 10.f;
	WriteObj.mIsAlive = true;
	WriteObj.mSomeCount = 3;

	T WriteStream = T();
	WriteStream.SerializeGeneric(WriteObj.sDataType, (uint8_t*)&WriteObj);

	GenericSerializableObject ReadObj;
	T ReadStream = T(WriteStream.GetBufferPtr(), WriteStream.GetLength());
	ReadStream.SerializeGeneric(ReadObj.sDataType, (uint8_t*)&ReadObj);

	return WriteObj.mHealth == ReadObj.mHealth
		&& WriteObj.mIsAlive == ReadObj.mIsAlive
		&& WriteObj.mSomeCount == ReadObj.mSomeCount;
}

template<typename T>
bool TestVector3()
{
	glm::vec3 WriteVector3 = glm::vec3(5.f, 3.f, 5.f);
	T WriteStream = T();

	WriteStream.SerializeVector3(WriteVector3);

	glm::vec3 ReadVector3;
	T ReadStream = T(WriteStream.GetBufferPtr(), WriteStream.GetLength());
	ReadStream.SerializeVector3(ReadVector3);

	return WriteVector3.x == ReadVector3.x && WriteVector3.y == ReadVector3.y && ReadVector3.z == WriteVector3.z;
}

template<typename T>
bool TestQuaternion()
{
	glm::quat WriteQuat = glm::quat(-0.890f, 0.001f, 0.432f, -0.144f);
	T WriteStream = T();

	WriteStream.SerializeQuaternion(WriteQuat);

	glm::quat ReadQuat;
	T ReadStream = T(WriteStream.GetBufferPtr(), WriteStream.GetLength());
	ReadStream.SerializeQuaternion(ReadQuat);
	
	constexpr float Precision = 0.01f;

	if (glm::abs(WriteQuat.x - ReadQuat.x) > Precision)
	{
		LastErrorStr = printf("X Axis fail: Original %f Result %f", WriteQuat.x, ReadQuat.x);
		return false;
	}
	else if (glm::abs(WriteQuat.y - ReadQuat.y) > Precision)
	{
		LastErrorStr = printf("Y Axis fail: Original %f Result %f", WriteQuat.y, ReadQuat.y);
		return false;
	}
	else if (glm::abs(WriteQuat.z - ReadQuat.z) > Precision)
	{
		LastErrorStr = printf("Z Axis fail: Original %f Result %f", WriteQuat.z, ReadQuat.z);
		return false;
	}
	else if (glm::abs(WriteQuat.w - ReadQuat.w) > Precision)
	{
		LastErrorStr = printf("W Axis fail: Original %f Result %f", WriteQuat.w, ReadQuat.w);
		return false;
	}

	// As these are compressed I should allow the compression margin as true, I should override the operator
	return true;
}

class SerializationTests : public ITesteable<ESerializationTests>
{
	bool RunTest(ESerializationTests TestToRun)
	{
		switch (TestToRun)
		{
		case ESerializationTests::TEST_SERIALIZE_PRIMITIVE:
		{
			return TestPrimitive<MemoryStream>() && TestPrimitive<MemoryBitStream>();
		}
			break;
		case ESerializationTests::TEST_SERIALIZE_PRIMITIVE_VECTOR:
		{
			return TestPrimitiveVector<MemoryStream>() && TestPrimitiveVector<MemoryBitStream>();
		}
			break;
		case ESerializationTests::TEST_SERIALIZE_STRING:
		{
			return TestString<MemoryStream>() && TestString<MemoryBitStream>();
		}
			break;
		case ESerializationTests::TEST_SERIALIZE_STRING_VECTOR:
		{
			return TestStringVector<MemoryStream>() && TestStringVector<MemoryBitStream>();
		}
			break;
		case ESerializationTests::TEST_SERIALIZE_CUSTOMSERIALIZABLE:
		{
			return TestCustomSerializable<MemoryStream>() && TestCustomSerializable<MemoryBitStream>();
		}
			break;
		case ESerializationTests::TEST_SERIALIZE_CUSTOMSERIALIZABLE_VECTOR:
		{
			return TestCustomSerializableVector<MemoryStream>() && TestCustomSerializableVector<MemoryBitStream>();
		}
			break;
		case ESerializationTests::TEST_SERIALIZE_GENERICSERIALIZABLE:
		{
			return TestGenericSerializable<MemoryStream>() && TestGenericSerializable<MemoryBitStream>();
		}
			break;
		case ESerializationTests::TEST_SERIALIZE_VECTOR3:
		{
			return TestVector3<MemoryStream>() && TestVector3<MemoryBitStream>();
		}
			break;
		case ESerializationTests::TEST_SERIALIZE_QUATERNION:
		{
			return TestQuaternion<MemoryStream>() && TestQuaternion<MemoryBitStream>();
		}
			break;
		default:
			return true;
			break;
		}

		return false;
	}
};



