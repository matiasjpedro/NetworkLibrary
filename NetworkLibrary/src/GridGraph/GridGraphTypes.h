#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <map>
#include <algorithm>
#include "../Math/Trigonometry.h"

class UGridObject
{
	uint32_t UniqueId;
	Vector2 Location;

public:

	UGridObject(uint32_t InUniqueId) :
		UniqueId(InUniqueId), Location(Vector2(0,0))
	{

	}

	UGridObject(uint32_t InUniqueId, const Vector2& InLocation) :
		UniqueId(InUniqueId), Location(InLocation)
	{

	}

	void SetObjectLocation(const Vector2& InLocation) { Location = InLocation; }
	Vector2 GetObjectLocation() const { return Location; }
};

struct FObjCellInfo
{
	bool IsValid() const { return StartX != -1; }
	void Reset() { StartX = -1; }
	int32_t StartX = -1;
	int32_t StartY = 0;
	int32_t EndX = 0;
	int32_t EndY = 0;
};



