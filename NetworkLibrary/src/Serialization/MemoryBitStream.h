#pragma once

#include "MemoryStream.h"

class MemoryBitStream : public MemoryStream
{
public:

	MemoryBitStream(uint32_t InBitsCount = 8) : MemoryStream(InBitsCount)
	{
	}

	MemoryBitStream(const char* InBuffer, uint32_t InBitsCount) : MemoryStream(InBuffer, InBitsCount)
	{
	}

	void SerializeBits(void* Data, size_t InBitCount);
	virtual void Serialize(void* Data, size_t InByteCount) override;

	virtual MemoryStream& operator<<(bool& Boolean) override
	{
		this->SerializeBits((void*)&Boolean, 1);

		return *this;
	}

protected:

	virtual void ReallocBuffer(uint32_t InNewLenght) override;

private:

	void SerializeBitsInternal(uint8_t& Data, size_t InBitCount);
};

template<int tValue, int tBits>
struct GetRequiredBitHelper
{
	enum
	{
		Value = GetRequiredBitHelper<(tValue >> 1), tBits + 1>::Value
	};
};

template<int tBits>
struct GetRequiredBitHelper<0, tBits>
{
	enum
	{
		
		Value = tBits
	};
};

template<int tValue>
struct GetRequiredBits
{
	enum
	{
		Value = GetRequiredBitHelper<tValue, 0>::Value
	};
};
