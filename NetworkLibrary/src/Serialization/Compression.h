#pragma once
#include <stdint.h>
#include "MemoryStream.h"


enum EAxisToSkip : uint8_t
{
	X = 1,
	Y = 2,
	Z = 4,
	W = 8
};

inline uint8_t GetMaxLenghtGivenFixedPoint(float InMax, float InMin, float InPrecision, bool bWantBits = false)
{
	// There is not point for this method as the result will be static but for the sake of knowledge
	// I put here how to calculate the max amount of bits given the specific bounds and precision needed.

	uint32_t MaxNumber = static_cast<uint32_t>((InMax + InMin) / InPrecision + 1);

	if (MaxNumber <= 255)
		return bWantBits ? sizeof(uint8_t) * 8 : sizeof(uint8_t);

	if (MaxNumber <= 65534)
		return  bWantBits ? sizeof(uint16_t) * 8 : sizeof(uint16_t);

	if (MaxNumber <= 2147483647)
		return  bWantBits ? sizeof(uint32_t) * 8 : sizeof(uint32_t);

	return bWantBits ? sizeof(uint64_t) * 8 : sizeof(uint64_t);
}

inline uint64_t ConvertToFixed(float InNumber, float InMin, float InPrecision)
{
	return static_cast<uint64_t> ((InNumber - InMin) / InPrecision);
}

inline float ConvertFromFixed(uint64_t InNumber, float InMin, float InPrecision)
{
	return static_cast<float>(InNumber) * InPrecision + InMin;
}
