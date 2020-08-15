#include "MemoryBitStream.h"
#include <algorithm>
#include "Compression.h"

void MemoryBitStream::SerializeBits(void* Data, size_t InBitCount)
{	
	uint8_t* DestByte = reinterpret_cast<uint8_t*>(Data);

	//Read all the bits
	while (InBitCount > 8)
	{
		SerializeBitsInternal(*DestByte, 8);
		++DestByte;
		InBitCount -= 8;
	}

	//Read anything left
	if (InBitCount > 0)
	{
		SerializeBitsInternal(*DestByte, InBitCount);
	}	
}

void MemoryBitStream::Serialize(void* Data, size_t InByteCount)
{
	SerializeBits(Data, InByteCount << 3);
}

void MemoryBitStream::ReallocBuffer(uint32_t InNewLenght)
{
	//Transform Bits in bytes.
	MemoryStream::ReallocBuffer(InNewLenght >> 3);

	mCapacity = InNewLenght;
}

void MemoryBitStream::SerializeBitsInternal(uint8_t& Data, size_t InBitCount)
{
	if (bIsReading)
	{
		uint32_t ByteOffset = mHead >> 3;;
		uint32_t BitOffset = mHead & 0x7;

		Data = static_cast<uint8_t>(mBuffer[ByteOffset]) >> BitOffset;

		uint32_t BitsFreeThisByte = 8 - BitOffset;
		if (BitsFreeThisByte < InBitCount)
		{
			//we need another byte
			Data = Data | (static_cast<uint8_t>(mBuffer[ByteOffset + 1]) << BitsFreeThisByte);
		}

		//don't forget a mask so that we only read the bit we wanted...
		Data = Data & (~(0x00ff << InBitCount));

		mHead += InBitCount;
	}
	else
	{
		uint32_t ResultBitHead = mHead + static_cast<uint32_t>(InBitCount);
		if (ResultBitHead > mCapacity)
		{
			ReallocBuffer(std::max(mCapacity * 2, ResultBitHead));
		}

		//Calculate the byteOffset by dividing the head by 8, 
		uint32_t ByteOffset = mHead >> 3;

		//Calculate the bitOffset by taking the last 3 bits.
		//0x7 = 111
		uint32_t BitOffset = mHead & 0x7;

		//Calculate which bits of the current byte to preserve
		//0xff = 255
		//Ref: https://en.wikipedia.org/wiki/Mask_%28computing%29
		uint8_t CurrentMask = ~(0xff << BitOffset);
		mBuffer[ByteOffset] = (mBuffer[ByteOffset] & CurrentMask) | (Data << BitOffset);

		//Calculate how many bits were not yet used in our target byte in the buffer
		uint32_t BitsFreeThisByte = 8 - BitOffset;

		//If we need more than that, carry to the next byte
		if (BitsFreeThisByte < InBitCount)
		{
			mBuffer[ByteOffset + 1] = Data >> BitsFreeThisByte;
		}

		mHead = ResultBitHead;
	}
}

