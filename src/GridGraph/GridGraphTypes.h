#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <map>
#include <algorithm>

struct Vec2
{
	float X, Y = 0;

	Vec2(float InX, float InY) :
		X(InX), Y(InY)
	{

	}
};

class UObject
{
	uint32_t UniqueId;
	Vec2 Location;

public:

	UObject(uint32_t InUniqueId) :
		UniqueId(InUniqueId), Location(Vec2(0,0))
	{

	}

	UObject(uint32_t InUniqueId, const Vec2& InLocation) :
		UniqueId(InUniqueId), Location(InLocation)
	{

	}

	void SetObjectLocation(const Vec2& InLocation) { Location = InLocation; }
	Vec2 GetObjectLocation() const { return Location; }
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



