#pragma once
#include "UnitTestingTypes.h"
#include "Serialization/MemoryStream.h"
#include "Serialization/INetSerializable.h"
#include "Serialization/Compression.h"
#include "Serialization/MemoryBitStream.h"

#pragma region ITesteable
#define SERIALIZATION_TESTS_ENUM(DO) \
    DO(TEST_SERIALIZE_PRIMITIVE) \
    DO(TEST_SERIALIZE_PRIMITIVE_VECTOR) \
	DO(TEST_SERIALIZE_STRING) \
    DO(TEST_SERIALIZE_STRING_VECTOR) \
	DO(TEST_SERIALIZE_NETSERIALIZABLE) \
	DO(TEST_SERIALIZE_NETSERIALIZABLE_VECTOR) \
	DO(TEST_SERIALIZE_VECTOR3) \
	DO(TEST_SERIALIZE_QUATERNION)


MAKE_LOGGABLE_ENUM(SERIALIZATION_TESTS_ENUM, ESerializationTests)
#pragma endregion

class SerializableTestActor : public INetSerializable
{

public:

	uint32_t Health;

	SerializableTestActor() :
		Health(5)
	{

	}

	void Serialize(class MemoryStream* Stream) override
	{
		Stream->SerializePrim(Health);
	}
};

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
bool TestNetSerializable()
{
	SerializableTestActor WriteSerializableActor;
	T WriteStream = T();
	WriteStream.SerializeNet(WriteSerializableActor);

	SerializableTestActor ReadSerializableActor;
	T ReadStream = T(WriteStream.GetBufferPtr(), WriteStream.GetLength());
	ReadStream.SerializeNet(ReadSerializableActor);

	return WriteSerializableActor.Health == ReadSerializableActor.Health;
}

template<typename T>
bool TestNetSerializableVector()
{
	std::vector<SerializableTestActor> WriteVector;
	WriteVector.push_back(SerializableTestActor());
	WriteVector.push_back(SerializableTestActor());

	T WriteStream = T();
	WriteStream.SerializeNetArr(WriteVector);

	std::vector<SerializableTestActor> ReadVector;
	T ReadStream = T(WriteStream.GetBufferPtr(), WriteStream.GetLength());
	ReadStream.SerializeNetArr(ReadVector);

	return ReadVector[0].Health == WriteVector[0].Health && ReadVector[1].Health == WriteVector[1].Health;
}

template<typename T>
bool TestVector3()
{
	Vector3 WriteVector3 = Vector3(5, 3, 5);
	T WriteStream = T();

	WriteStream.SerializeVector3(WriteVector3, EAxisToSkip::Z);

	Vector3 ReadVector3;
	T ReadStream = T(WriteStream.GetBufferPtr(), WriteStream.GetLength());
	ReadStream.SerializeVector3(ReadVector3);

	// As these are compressed I should allow the compression margin as true, I should override the operator
	return WriteVector3.X == ReadVector3.X && WriteVector3.Y == ReadVector3.Y && ReadVector3.Z == 0;
}

template<typename T>
bool TestQuaternion()
{
	Quaternion WriteQuat = Quaternion(-0.890f, 0.001f, 0.432f, -0.144f);
	T WriteStream = T();

	WriteStream.SerializeQuaternion(WriteQuat);

	Quaternion ReadQuat;
	T ReadStream = T(WriteStream.GetBufferPtr(), WriteStream.GetLength());
	ReadStream.SerializeQuaternion(ReadQuat);

	// As these are compressed I should allow the compression margin as true, I should override the operator
	return WriteQuat.X == ReadQuat.X && WriteQuat.Y == ReadQuat.Y && WriteQuat.Z == ReadQuat.Z && WriteQuat.W == ReadQuat.W;
}

class SerializationTests : public ITesteable<ESerializationTests>
{
	bool RunTest(ESerializationTests TestToRun, std::string& OutErrMsg)
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
		case ESerializationTests::TEST_SERIALIZE_NETSERIALIZABLE:
		{
			return TestNetSerializable<MemoryStream>() && TestNetSerializable<MemoryBitStream>();
		}
			break;
		case ESerializationTests::TEST_SERIALIZE_NETSERIALIZABLE_VECTOR:
		{
			return TestNetSerializableVector<MemoryStream>() && TestNetSerializableVector<MemoryBitStream>();
		}
			break;
		case ESerializationTests::TEST_SERIALIZE_VECTOR3:
		{
			return TestVector3<MemoryStream>() && TestVector3<MemoryBitStream>();
		}
			break;
		case ESerializationTests::TEST_SERIALIZE_QUATERNION:
		{
			OutErrMsg.append(" There was with the compression of the quaternion. ");
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



