#include "MemoryStream.h"
#include "ISerializableObject.h"
#include <algorithm>
#include "Compression.h"
#include "Math/Trigonometry.h"

void MemoryStream::SerializeRAW(void* Data, size_t InByteCount)
{
	if (bIsReading)
	{
		uint32_t TargetHead = mHead + static_cast<uint32_t>(InByteCount);

		// Get the data from the TargetHead;
		std::memcpy(Data, mBuffer + mHead, InByteCount);

		mHead = TargetHead;
	}
	else
	{
		//Make sure we have space
		uint32_t ResultHead = mHead + static_cast<uint32_t>(InByteCount);
		if (ResultHead > mCapacity)
		{
			ReallocBuffer(std::max(mCapacity * 2, ResultHead));
		}

		// Copy new data to the previous head position
		std::memcpy(mBuffer + mHead, Data, InByteCount);

		mHead = ResultHead;
	}
}

void MemoryStream::SerializeString(std::string& String)
{
	size_t StringSize = 0;
	if (!bIsReading)
	{
		//+1 for null termination
		StringSize = String.size() + 1;
	}

	SerializePrim(StringSize, 2);

	if (bIsReading)
	{
		String.resize(StringSize);
	}

	SerializeRAW((void*)String.data(), StringSize);
}

void MemoryStream::SerializeStringArr(std::vector<std::string>& Vector)
{
	size_t ElementCount = 0;

	if (!bIsReading)
	{
		ElementCount = Vector.size();
	}

	// With two byte should be enough
	SerializePrim(ElementCount,2);

	if (bIsReading)
	{
		Vector.resize(ElementCount);
	}

	for (std::string& Element : Vector)
	{

		SerializeString(Element);
	}
}

void MemoryStream::SerializeObject(ISerializableObject& SerializableObject)
{
	SerializableObject.Serialize(this);
}

void MemoryStream::SerializeVector3(Vector3& InVector3)
{
	// Suppose this is our world bounds
	constexpr int WorldHalfBounds = 4000 / 2;

	// Suppose in the client the precision of this position only need to be this one.
	constexpr float ClientRequiredPrecision = 0.1f;

	const uint8_t LenghtPerComp = GetMaxLenghtGivenFixedPoint(WorldHalfBounds, WorldHalfBounds, ClientRequiredPrecision);

	uint64_t FixedValue = 0;

	if (bIsReading)
	{
		SerializePrim(FixedValue, LenghtPerComp);
		InVector3.mX = ConvertFromFixed(FixedValue, -WorldHalfBounds, ClientRequiredPrecision);		

		FixedValue = 0;
		SerializePrim(FixedValue, LenghtPerComp);
		InVector3.mY = ConvertFromFixed(FixedValue, -WorldHalfBounds, ClientRequiredPrecision);
		
		FixedValue = 0;
		SerializePrim(FixedValue, LenghtPerComp);
		InVector3.mZ = ConvertFromFixed(FixedValue, -WorldHalfBounds, ClientRequiredPrecision);
	}
	else
	{
		
		FixedValue = ConvertToFixed(InVector3.mX, -WorldHalfBounds, ClientRequiredPrecision);
		SerializePrim(FixedValue, LenghtPerComp);
		
		FixedValue = ConvertToFixed(InVector3.mY, -WorldHalfBounds, ClientRequiredPrecision);
		SerializePrim(FixedValue, LenghtPerComp);

		FixedValue = ConvertToFixed(InVector3.mZ, -WorldHalfBounds, ClientRequiredPrecision);
		SerializePrim(FixedValue, LenghtPerComp);
		
	}
}


void MemoryStream::SerializeQuaternion(Quaternion& InQuaternion)
{
	// for 1 and -1 32767 should be enough precision
	constexpr float Precision = (2.f / 32767.f);
	uint64_t FixedValue = 0;
	bool bIsNegative;

	if (bIsReading)
	{
		SerializePrim(FixedValue, 2);
		InQuaternion.mX = ConvertFromFixed(FixedValue, -1.f, Precision);

		FixedValue = 0;
		SerializePrim(FixedValue, 2);
		InQuaternion.mY = ConvertFromFixed(FixedValue, -1.f, Precision);

		FixedValue = 0;
		SerializePrim(FixedValue, 2);
		InQuaternion.mZ = ConvertFromFixed(FixedValue, -1.f, Precision);

		const float Squared = (InQuaternion.mX * InQuaternion.mX) +
			(InQuaternion.mY * InQuaternion.mY) +
			(InQuaternion.mZ * InQuaternion.mZ);

		InQuaternion.mW = std::sqrt(1.f - Squared);

		SerializePrim(bIsNegative, 1);

		if (bIsNegative)
		{
			InQuaternion.mW *= -1.f;
		}
	}
	else
	{
		//InQuaternion = std::normalize(InQuaternion);

		FixedValue = ConvertToFixed(InQuaternion.mX, -1.f, Precision);
		SerializePrim(FixedValue, 2);

		FixedValue = ConvertToFixed(InQuaternion.mY, -1.f, Precision);
		SerializePrim(FixedValue, 2);

		FixedValue = ConvertToFixed(InQuaternion.mZ, -1.f, Precision);
		SerializePrim(FixedValue, 2);

		bIsNegative = InQuaternion.mW < 0;
		SerializePrim(bIsNegative, 1);
	}
}

void MemoryStream::SerializeBool(bool& Value)
{
	SerializePrim(Value);
}

void MemoryStream::ReallocBuffer(uint32_t InNewLenght)
{
	mBuffer = static_cast<char*>(std::realloc(mBuffer, InNewLenght));

	mCapacity = InNewLenght;
}

