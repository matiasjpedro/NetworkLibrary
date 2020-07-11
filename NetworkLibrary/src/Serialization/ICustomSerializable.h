#pragma once

class ICustomSerializable
{
public:

	virtual void Serialize(class MemoryStream* Stream) = 0;
};
