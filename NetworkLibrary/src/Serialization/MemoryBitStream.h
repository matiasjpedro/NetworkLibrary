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

	void SerializeBitsRAW(void* Data, size_t InBitCount);
	virtual void SerializeRAW(void* Data, size_t InByteCount) override;

	virtual void SerializeBool(bool& Value) override;

protected:

	virtual void ReallocBuffer(uint32_t InNewLenght) override;

private:

	void SerializeBits_Internal(uint8_t& Data, size_t InBitCount);
};
