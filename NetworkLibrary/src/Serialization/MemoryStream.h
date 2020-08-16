#pragma once
#include <stdint.h>
#include <vector>
#include "ISerializableObject.h"
#include "../UObject.h"
#include "ByteSwapper.h"
#include <string>
#include <iostream>

struct Vector3;
struct Quaternion;

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

	virtual void Serialize(void* Data, size_t InByteCount);

	//Primitives
	template<typename T>
	void ByteOrderSerialize(T& Data, size_t InByteCount = sizeof(T));
	
	virtual MemoryStream& operator<<(bool& Boolean)
	{
		this->Serialize((void*)&Boolean, 1);

		return *this;
	}

	//Primitives
	friend MemoryStream& operator<<(class MemoryStream& Ar, float& Primitive)
	{
		Ar.ByteOrderSerialize(Primitive);

		return Ar;
	}

	friend MemoryStream& operator<<(class MemoryStream& Ar, std::uint8_t& Primitive)
	{
		Ar.Serialize((void*)&Primitive, 1);

		return Ar;
	}

	friend MemoryStream& operator<<(class MemoryStream& Ar, std::uint16_t& Primitive)
	{
		Ar.ByteOrderSerialize(Primitive);

		return Ar;
	}

	friend MemoryStream& operator<<(class MemoryStream& Ar, std::uint32_t& Primitive)
	{
		Ar.ByteOrderSerialize(Primitive);

		return Ar;
	}

	// [TO THINK] hacky const version to allow temp variables, think a better way.
	friend MemoryStream& operator<<(class MemoryStream& Ar, const std::uint32_t& Primitive)
	{
		Ar.ByteOrderSerialize(const_cast<std::uint32_t&>(Primitive));

		return Ar;
	}

	friend MemoryStream& operator<<(class MemoryStream& Ar, std::uint64_t& Primitive)
	{
		Ar.ByteOrderSerialize(Primitive);

		return Ar;
	}

	friend MemoryStream& operator<<(class MemoryStream& Ar, std::int8_t& Primitive)
	{
		Ar.Serialize((void*)&Primitive, 1);

		return Ar;
	}

	friend MemoryStream& operator<<(class MemoryStream& Ar, std::int16_t& Primitive)
	{
		Ar.ByteOrderSerialize(Primitive);

		return Ar;
	}

	friend MemoryStream& operator<<(class MemoryStream& Ar, std::int32_t& Primitive)
	{
		Ar.ByteOrderSerialize(Primitive);

		return Ar;
	}

	friend MemoryStream& operator<<(class MemoryStream& Ar, std::int64_t& Primitive)
	{
		Ar.ByteOrderSerialize(Primitive);

		return Ar;
	}

	//Others
	friend MemoryStream& operator<<(class MemoryStream& Ar, ISerializableObject& Obj)
	{
		// Let the SerialiableObject handle their own way to serialize.
		Obj.Serialize(Ar);

		return Ar;
	}

	friend MemoryStream& operator<<(class MemoryStream& Ar, struct Vector3& Obj)
	{
		Ar.SerializeVector3(Obj);

		return Ar;
	}

	friend MemoryStream& operator<<(class MemoryStream& Ar, struct Quaternion& Obj)
	{
		Ar.SerializeQuaternion(Obj);

		return Ar;
	}

	friend MemoryStream& operator<<(MemoryStream& Ar, std::string& String)
	{
		uint16_t StringSize = 0;
		if (!Ar.bIsReading)
		{
			//+1 for null termination
			StringSize = String.size() + 1;
		}

		Ar << StringSize;

		if (Ar.bIsReading)
		{
			String.resize(StringSize);
		}

		Ar.Serialize((void*)String.data(), StringSize);

		return Ar;
	}

	template<typename T>
	friend MemoryStream& operator<<(MemoryStream& Ar, std::vector<T>& Vector)
	{
		uint16_t ElementCount = 0;
		if (!Ar.bIsReading)
		{
			ElementCount = Vector.size();
		}

		Ar << ElementCount;

		if (Ar.bIsReading)
		{
			Vector.resize(ElementCount);
		}

		for (T& Element : Vector)
		{
			Ar << Element;
		}

		return Ar;
	}
	
	//Helpers
	const char* GetBufferPtr() const { return mBuffer; }
	uint32_t GetLength() const { return mHead; }
	uint32_t GetRemainingDataSize() { return mCapacity - mHead; }

protected:

	char* mBuffer;
	uint32_t mHead;
	uint32_t mCapacity;
	uint8_t bIsReading : 1;

	virtual void ReallocBuffer(uint32_t InNewLenght);

private:

	void SerializeVector3(struct Vector3& InVector3);
	void SerializeQuaternion(struct Quaternion& InQuaternion);

	template <typename T>
	T ByteSwap(T inData);	
};

template<typename T>
void MemoryStream::ByteOrderSerialize(T& Data, size_t InByteCount /*= sizeof(T)*/)
{
	static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Generic Write only support primitive data types, make an specialization for this type");

	if (STREAM_ENDIANNES != PLATFORM_ENDIANNES)
	{
		T SwappedData = ByteSwap(Data);
		Serialize(&SwappedData, InByteCount);
	}
	else
	{
		Serialize(&Data, InByteCount);
	}
}

template <typename T>
T MemoryStream::ByteSwap(T InData)
{
	return ByteSwapper<T, sizeof(T)>().Swap(InData);
}

