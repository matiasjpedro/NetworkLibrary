#include "MemoryStream.h"
#include "ICustomSerializable.h"
#include <algorithm>
#include "Compression.h"
#include "glm/vec3.hpp"
#include "glm/ext/quaternion_float.hpp"
#include "Reflection/Reflection.h"

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

void MemoryStream::SerializeCustom(ICustomSerializable& CustomSerializable)
{
	CustomSerializable.Serialize(this);
}

void MemoryStream::SerializeVector3(glm::vec3& InVector3, uint8_t AxisToSkip)
{
	// Suppose this is our world bounds
	constexpr int WorldHalfBounds = 4000 / 2;

	// Suppose in the client the precision of this position only need to be this one.
	constexpr float ClientRequiredPrecision = 0.1f;

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
			InVector3.x = ConvertFromFixed(FixedValue, -WorldHalfBounds, ClientRequiredPrecision);
		}

		SerializeBool(AxisSkipped);

		if (!AxisSkipped)
		{
			FixedValue = 0;
			SerializePrim(FixedValue, LenghtPerComp);
			InVector3.y = ConvertFromFixed(FixedValue, -WorldHalfBounds, ClientRequiredPrecision);
		}

		SerializeBool(AxisSkipped);

		if (!AxisSkipped)
		{
			FixedValue = 0;
			SerializePrim(FixedValue, LenghtPerComp);
			InVector3.z = ConvertFromFixed(FixedValue, -WorldHalfBounds, ClientRequiredPrecision);
		}
		else
		{
			InVector3.z = 0;
		}
	}
	else
	{
		bool AxisSkipped = HasFlag(AxisToSkip, EAxisToSkip::X);
		SerializeBool(AxisSkipped);

		if (!AxisSkipped)
		{
			FixedValue = ConvertToFixed(InVector3.x, -WorldHalfBounds, ClientRequiredPrecision);
			SerializePrim(FixedValue, LenghtPerComp);
		}

		AxisSkipped = HasFlag(AxisToSkip, EAxisToSkip::Y);
		SerializeBool(AxisSkipped);

		if (!AxisSkipped)
		{
			FixedValue = ConvertToFixed(InVector3.y, -WorldHalfBounds, ClientRequiredPrecision);
			SerializePrim(FixedValue, LenghtPerComp);
		}

		AxisSkipped = HasFlag(AxisToSkip, EAxisToSkip::Z);
		SerializeBool(AxisSkipped);

		if (!AxisSkipped)
		{
			FixedValue = ConvertToFixed(InVector3.z, -WorldHalfBounds, ClientRequiredPrecision);
			SerializePrim(FixedValue, LenghtPerComp);
		}
	}
}


void MemoryStream::SerializeQuaternion(glm::quat& InQuaternion)
{
	// for 1 and -1 32767 should be enough precision
	constexpr float Precision = (2.f / 32767.f);
	uint64_t FixedValue = 0;
	bool bIsNegative;

	if (bIsReading)
	{
		SerializePrim(FixedValue, 2);
		InQuaternion.x = ConvertFromFixed(FixedValue, -1.f, Precision);

		FixedValue = 0;
		SerializePrim(FixedValue, 2);
		InQuaternion.y = ConvertFromFixed(FixedValue, -1.f, Precision);

		FixedValue = 0;
		SerializePrim(FixedValue, 2);
		InQuaternion.z = ConvertFromFixed(FixedValue, -1.f, Precision);

		const float Squared = (InQuaternion.x * InQuaternion.x) +
			(InQuaternion.y * InQuaternion.y) +
			(InQuaternion.z * InQuaternion.z);

		InQuaternion.w = glm::sqrt(1.f - Squared);

		SerializePrim(bIsNegative, 1);

		if (bIsNegative)
		{
			InQuaternion.w *= -1.f;
		}
	}
	else
	{
		InQuaternion = glm::normalize(InQuaternion);

		FixedValue = ConvertToFixed(InQuaternion.x, -1.f, Precision);
		SerializePrim(FixedValue, 2);

		FixedValue = ConvertToFixed(InQuaternion.y, -1.f, Precision);
		SerializePrim(FixedValue, 2);

		FixedValue = ConvertToFixed(InQuaternion.z, -1.f, Precision);
		SerializePrim(FixedValue, 2);

		bIsNegative = InQuaternion.w < 0;
		SerializePrim(bIsNegative, 1);
	}
}

void MemoryStream::SerializeGeneric(const DataType* ClassType, uint8_t* InData)
{
	for (const auto& mv : ClassType->GetMemberVariables())
	{
		void* mvLocation = InData + mv.GetOffset();

		switch (mv.GetPrimitiveType())
		{
		case EPT_Int:
			SerializePrim(*reinterpret_cast<int*>(mvLocation));
			break;
		case EPT_String:
			SerializeString(*reinterpret_cast<std::string*>(mvLocation));
			break;
		case EPT_Float:
			SerializePrim(*reinterpret_cast<float*>(mvLocation));
			break;
		case EPT_Bool:
			SerializeBool(*reinterpret_cast<bool*>(mvLocation));
			break;
		}
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

