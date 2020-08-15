#pragma once
#include <iostream>

class ISerializableObject
{
public:

	virtual void Serialize(class MemoryStream& Stream) = 0;
};
