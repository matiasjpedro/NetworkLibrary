#pragma once
#include <stdint.h>
#include <cstdlib>
#include <vector>
#include "ICustomSerializable.h"
#include "ByteSwapper.h"
#include <string>
#include "glm/fwd.hpp"

//Temporary, 0 for Little-Endian, 1 for Big-Endian
#define STREAM_ENDIANNES 0
#define PLATFORM_ENDIANNES 0

class MemoryStream
{

public:

	MemoryStream(uint32_t InByteCount = 1) :
		mBuffer(nullptr),
		mHead(0),
		mCapacity(InByteCount),
		bIsReading(false)
	{
		ReallocBuffer(InByteCount);
	}

	MemoryStream(const char* InBuffer, uint32_t InByteCount) :
		mBuffer(const_cast<char*>(InBuffer)),
		mHead(0),
		mCapacity(InByteCount),
		bIsReading(true)
	{

	}

	~MemoryStream() 
	{
		//If I'm only reading this buffer I dont own that data, meaning that I only allowed to read it.
		if (!bIsReading)
		{
			std::free(mBuffer);
		}
	}

	const bool IsReading() { return bIsReading; }

	//RAW
	virtual void SerializeRAW(void* Data, size_t InByteCount);
	virtual void SerializeBool(bool& Value);

	//Primitives
	template<typename T>
	void SerializePrim(T& Data, size_t InByteCount = sizeof(T));
	template<typename T>
	void SerializePrimArr(std::vector<T>& Vector, size_t InBytePerElement = sizeof(T));
	
	//Specializations.
	void SerializeCustom(class ICustomSerializable& CustomSerializable);
	template<typename T>
	void SerializeCustomArr(std::vector<T>& Vector);

	void SerializeString(std::string& String);
	void SerializeStringArr(std::vector<std::string>& Vector);

	void SerializeVector3(glm::vec3& InVector3, uint8_t AxisToSkip = 0);
	void SerializeQuaternion(glm::quat& InQuaternion);
	
	void SerializeGeneric(const class DataType* ClassType, uint8_t* InData);

	//Helpers
	const char* GetBufferPtr() const { return mBuffer; }
	uint32_t GetLength() const { return mHead; }
	uint32_t GetRemainingDataSize() { return mCapacity - mHead; }

protected:

	bool bIsReading;
	char* mBuffer;
	uint32_t mHead;
	uint32_t mCapacity;

	virtual void ReallocBuffer(uint32_t InNewLenght);

private:

	template <typename T>
	T ByteSwap(T inData);	
};

template<typename T>
void MemoryStream::SerializePrim(T& Data, size_t InByteCount /*= sizeof(T)*/)
{
	static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Generic Write only support primitive data types, make an specialization for this type");
	
	if (STREAM_ENDIANNES != PLATFORM_ENDIANNES)
	{
		T SwappedData = ByteSwap(Data);
		SerializeRAW(&SwappedData, InByteCount);
	}
	else
	{
		SerializeRAW(&Data, InByteCount);
	}
}

template<typename T>
void MemoryStream::SerializePrimArr(std::vector<T>& Vector, size_t InBytePerElement /*= sizeof(T)*/)
{
	size_t ElementCount = 0; 
	
	if (!bIsReading)
	{
		ElementCount = Vector.size();
	}

	SerializePrim(ElementCount, 2);

	if (bIsReading)
	{
		Vector.resize(ElementCount);
	}

	static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Generic Write only support primitive data types, make an specialization for this type");

	if (InBytePerElement == sizeof(T))
	{
		SerializeRAW(Vector.data(), ElementCount * InBytePerElement);
	}
	else
	{
		for (T& Element : Vector)
		{

			SerializeRAW((void*)&Element, InBytePerElement);
		}
	}

}

template<typename T>
void MemoryStream::SerializeCustomArr(std::vector<T>& Vector)
{
	size_t ElementCount = 0;

	if (!bIsReading)
	{
		ElementCount = Vector.size();
	}

	SerializePrim(ElementCount, 2);

	if (bIsReading)
	{
		Vector.resize(ElementCount);
	}

	for (const T& Element : Vector)
	{
		SerializeCustom((ICustomSerializable&)Element);	
	}
}

template <typename T>
T MemoryStream::ByteSwap(T InData)
{
	return ByteSwapper<T, sizeof(T)>().Swap(InData);
}

