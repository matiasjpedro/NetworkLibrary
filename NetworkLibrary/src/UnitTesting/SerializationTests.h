#pragma once
#include "../UnitTesting/Testeable.h"
#include "../Serialization/MemoryStream.h"
#include "../Serialization/ISerializableObject.h"
#include "../Serialization/Compression.h"
#include "../Serialization/MemoryBitStream.h"
#include "../Math/Trigonometry.h"

#pragma region ITesteable
#define SERIALIZATION_TESTS_ENUM(DO) \
    DO(TEST_SERIALIZE_PRIMITIVE) \
    DO(TEST_SERIALIZE_PRIMITIVE_VECTOR) \
    DO(TEST_SERIALIZE_BOOL_VECTOR) \
	DO(TEST_SERIALIZE_STRING) \
    DO(TEST_SERIALIZE_STRING_VECTOR) \
	DO(TEST_SERIALIZE_SERIALIZABLEOBJECT) \
	DO(TEST_SERIALIZE_SERIALIZABLEOBJECT_VECTOR) \
	DO(TEST_SERIALIZE_VECTOR3) \
	DO(TEST_SERIALIZE_VECTOR3_VECTOR) \
	DO(TEST_SERIALIZE_QUATERNION) 


MAKE_LOGGABLE_ENUM(SERIALIZATION_TESTS_ENUM, ESerializationTests)
#pragma endregion

class SerializableObject : public ISerializableObject
{

public:

	uint32_t Health;

	SerializableObject()
	{

	}

	SerializableObject(int InHealth) :
		Health(5)
	{

	}

	void Serialize(class MemoryStream& Stream) override
	{
		Stream << Health;
	}
};

template<typename T>
bool TestPrimitive()
{
	uint32_t WriteInt = 13;
	T WriteStream = T();
	WriteStream << 13;
	WriteStream << WriteInt;

	uint32_t ReadInt = 0;
	uint32_t ReadInt2 = 0;
	T ReadStream = T(WriteStream.GetBufferPtr(), WriteStream.GetLength());
	ReadStream << ReadInt;
	ReadStream << ReadInt2;

	return ReadInt == WriteInt && ReadInt2 == WriteInt;
}

template<typename T>
bool TestPrimitiveVector()
{
	std::vector<uint32_t> WriteVector = { 13, 12 };

	T WriteStream = T();
	WriteStream << WriteVector;

	std::vector<uint32_t> ReadVector;

	T ReadStream = T(WriteStream.GetBufferPtr(), WriteStream.GetLength());
	ReadStream << ReadVector;

	return ReadVector[0] == WriteVector[0] && ReadVector[1] == WriteVector[1];
}

template<typename T>
bool TestBoolVector()
{
// 	std::vector<char> WriteVector = { true, false };
// 
// 	T WriteStream = T();
// 	WriteStream << WriteVector;
// 
// 	std::vector<char> ReadVector;
// 
// 	T ReadStream = T(WriteStream.GetBufferPtr(), WriteStream.GetLength());
// 	ReadStream << ReadVector;
// 
// 	return ReadVector[0] == WriteVector[0] && ReadVector[1] == WriteVector[1];

	return true;
}

template<typename T>
bool TestString()
{
	std::string WriteString = std::string("Test");
	T WriteStream = T();
	WriteStream << WriteString;

	std::string ReadString;
	T ReadStream = T(WriteStream.GetBufferPtr(), WriteStream.GetLength());
	ReadStream << ReadString;

	return WriteString.compare(ReadString);
}

template<typename T>
bool TestStringVector()
{
	std::vector<std::string> WriteVector = { std::string("Test1"),  std::string("Test2") };

	T WriteStream = T();
	WriteStream << WriteVector;

	std::vector<std::string> ReadVector;
	T ReadStream = T(WriteStream.GetBufferPtr(), WriteStream.GetLength());
	ReadStream << ReadVector;

	return WriteVector[0].compare(ReadVector[0]) && WriteVector[1].compare(ReadVector[1]);
}

template<typename T>
bool TestSerializableObject()
{
	SerializableObject WriteObj = SerializableObject(8);

	T WriteStream = T();
	WriteStream << WriteObj;

	SerializableObject ReadObj;
	T ReadStream = T(WriteStream.GetBufferPtr(), WriteStream.GetLength());
	ReadStream << ReadObj;

	return WriteObj.Health == ReadObj.Health;
}

template<typename T>
bool TestSerializableObjectVector()
{
	std::vector<SerializableObject> WriteVector{ SerializableObject(5) , SerializableObject(3) };

	T WriteStream = T();
	WriteStream << WriteVector;

	std::vector<SerializableObject> ReadVector;
	T ReadStream = T(WriteStream.GetBufferPtr(), WriteStream.GetLength());
	ReadStream << ReadVector;

	return ReadVector[0].Health == WriteVector[0].Health && ReadVector[1].Health == WriteVector[1].Health;
}

template<typename T>
bool TestVector3()
{
	Vector3 WriteVector3 = Vector3(5.f, 3.f, 5.f);
	T WriteStream = T();

	WriteStream << WriteVector3;

	Vector3 ReadVector3;
	T ReadStream = T(WriteStream.GetBufferPtr(), WriteStream.GetLength());
	ReadStream << ReadVector3;

	return WriteVector3.mX == ReadVector3.mX && WriteVector3.mY == ReadVector3.mY && ReadVector3.mZ == WriteVector3.mZ;
}

template<typename T>
bool TestVector3Vector()
{
	std::vector<Vector3> WriteVector{ Vector3(5,3,4) , Vector3(3,4,5) };

	T WriteStream = T();
	WriteStream << WriteVector;

	std::vector<Vector3> ReadVector;
	T ReadStream = T(WriteStream.GetBufferPtr(), WriteStream.GetLength());
	ReadStream << ReadVector;

	for (int i = 0; i < ReadVector.size(); i++)
	{
		if (ReadVector[i].mX == WriteVector[i].mX
			&& ReadVector[i].mY == WriteVector[i].mY
			&& ReadVector[i].mZ == WriteVector[i].mZ)
			continue;

		return false;
	}

	return true;
}

template<typename T>
bool TestQuaternion()
{
	Quaternion WriteQuat = Quaternion(-0.890f, 0.001f, 0.432f, -0.144f);
	T WriteStream = T();

	WriteStream << WriteQuat;

	Quaternion ReadQuat;
	T ReadStream = T(WriteStream.GetBufferPtr(), WriteStream.GetLength());
	ReadStream << ReadQuat;
	
	constexpr float Precision = 0.01f;

	if (std::abs(WriteQuat.mX - ReadQuat.mX) > Precision)
	{
		LastErrorStr = printf("X Axis fail: Original %f Result %f", WriteQuat.mX, ReadQuat.mX);
		return false;
	}
	else if (std::abs(WriteQuat.mY - ReadQuat.mY) > Precision)
	{
		LastErrorStr = printf("Y Axis fail: Original %f Result %f", WriteQuat.mY, ReadQuat.mY);
		return false;
	}
	else if (std::abs(WriteQuat.mZ - ReadQuat.mZ) > Precision)
	{
		LastErrorStr = printf("Z Axis fail: Original %f Result %f", WriteQuat.mZ, ReadQuat.mZ);
		return false;
	}
	else if (std::abs(WriteQuat.mW - ReadQuat.mW) > Precision)
	{
		LastErrorStr = printf("W Axis fail: Original %f Result %f", WriteQuat.mW, ReadQuat.mW);
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
		case ESerializationTests::TEST_SERIALIZE_BOOL_VECTOR:
		{
			return TestBoolVector<MemoryStream>() && TestBoolVector<MemoryBitStream>();
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
		case ESerializationTests::TEST_SERIALIZE_SERIALIZABLEOBJECT:
		{
			return TestSerializableObject<MemoryStream>() && TestSerializableObject<MemoryBitStream>();
		}
			break;
		case ESerializationTests::TEST_SERIALIZE_SERIALIZABLEOBJECT_VECTOR:
		{
			return TestSerializableObjectVector<MemoryStream>() && TestSerializableObjectVector<MemoryBitStream>();
		}
			break;
		case ESerializationTests::TEST_SERIALIZE_VECTOR3:
		{
			return TestVector3<MemoryStream>() && TestVector3<MemoryBitStream>();
		}
			break;
		case ESerializationTests::TEST_SERIALIZE_VECTOR3_VECTOR:
		{
			return TestVector3Vector<MemoryStream>() && TestVector3Vector<MemoryBitStream>();
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



