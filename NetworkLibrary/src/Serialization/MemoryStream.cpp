#include "MemoryStream.h"
#include "INetSerializable.h"
#include <algorithm>
#include "Compression.h"

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

void MemoryStream::SerializeNet(INetSerializable& NetSerializable)
{
	NetSerializable.Serialize(this);
}

void MemoryStream::SerializeVector3(Vector3& InVector3, uint8_t AxisToSkip)
{
	// Suppose this is our world bounds
	const int WorldHalfBounds = 4000 / 2;

	// Suppose in the client the precision of this position only need to be this one.
	const float ClientRequiredPrecision = 0.1f;

	const uint8_t LenghtPerComp = GetMaxLenghtGivenFixedPoint(WorldHalfBounds, WorldHalfBounds, ClientRequiredPrecision);

	uint64_t FixedValue = 0;

	if (bIsReading)
	{
		//SerializePrim(AxisToSkip);
		bool AxisSkipped = false;
		SerializeBool(AxisSkipped);

		if (!AxisSkipped)
		{
			SerializePrim(FixedValue, LenghtPerComp);
			InVector3.X = ConvertFromFixed(FixedValue, -WorldHalfBounds, ClientRequiredPrecision);
		}

		SerializeBool(AxisSkipped);

		if (!AxisSkipped)
		{
			FixedValue = 0;
			SerializePrim(FixedValue, LenghtPerComp);
			InVector3.Y = ConvertFromFixed(FixedValue, -WorldHalfBounds, ClientRequiredPrecision);
		}

		SerializeBool(AxisSkipped);

		if (!AxisSkipped)
		{
			FixedValue = 0;
			SerializePrim(FixedValue, LenghtPerComp);
			InVector3.Z = ConvertFromFixed(FixedValue, -WorldHalfBounds, ClientRequiredPrecision);
		}
	}
	else
	{
		bool AxisSkipped = HasFlag(AxisToSkip, EAxisToSkip::X);
		SerializeBool(AxisSkipped);

		if (!AxisSkipped)
		{
			FixedValue = ConvertToFixed(InVector3.X, -WorldHalfBounds, ClientRequiredPrecision);
			SerializePrim(FixedValue, LenghtPerComp);
		}

		AxisSkipped = HasFlag(AxisToSkip, EAxisToSkip::Y);
		SerializeBool(AxisSkipped);

		if (!AxisSkipped)
		{
			FixedValue = ConvertToFixed(InVector3.Y, -WorldHalfBounds, ClientRequiredPrecision);
			SerializePrim(FixedValue, LenghtPerComp);
		}

		AxisSkipped = HasFlag(AxisToSkip, EAxisToSkip::Z);
		SerializeBool(AxisSkipped);

		if (!AxisSkipped)
		{
			FixedValue = ConvertToFixed(InVector3.Z, -WorldHalfBounds, ClientRequiredPrecision);
			SerializePrim(FixedValue, LenghtPerComp);
		}
	}
}


void MemoryStream::SerializeQuaternion(Quaternion& InQuaternion)
{
	// for 1 and -1 32767 should be enough precision
	const float Precision = (2.f / 2147483647.5f);
	uint64_t FixedValue = 0;
	bool bIsNegative;

	if (bIsReading)
	{
		SerializePrim(FixedValue, 4);
		InQuaternion.X = ConvertFromFixed(FixedValue, -1.f, Precision);

		FixedValue = 0;
		SerializePrim(FixedValue, 4);
		InQuaternion.Y = ConvertFromFixed(FixedValue, -1.f, Precision);

		FixedValue = 0;
		SerializePrim(FixedValue, 4);
		InQuaternion.Z = ConvertFromFixed(FixedValue, -1.f, Precision);

		InQuaternion.W = sqrtf(1.f -
			(InQuaternion.X * InQuaternion.X) +
			(InQuaternion.Y * InQuaternion.Y) +
			(InQuaternion.Z * InQuaternion.Z));

		SerializePrim(bIsNegative, 1);

		if (bIsNegative)
		{
			InQuaternion.W *= -1.f;
		}
	}
	else
	{
		FixedValue = ConvertToFixed(InQuaternion.X, -1.f, Precision);
		SerializePrim(FixedValue, 4);

		FixedValue = ConvertToFixed(InQuaternion.Y, -1.f, Precision);
		SerializePrim(FixedValue, 4);

		FixedValue = ConvertToFixed(InQuaternion.Z, -1.f, Precision);
		SerializePrim(FixedValue, 4);

		bIsNegative = InQuaternion.W < 0;
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

