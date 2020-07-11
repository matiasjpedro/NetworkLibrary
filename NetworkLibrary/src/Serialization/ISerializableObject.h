#pragma once

class ISerializableObject
{
public:

	virtual void Serialize(class MemoryStream* Stream) = 0;
};
