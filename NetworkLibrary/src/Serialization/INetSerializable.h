#pragma once

class INetSerializable
{
public:

	virtual void Serialize(class MemoryStream* Stream) = 0;
};
