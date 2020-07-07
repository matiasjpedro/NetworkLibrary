#pragma once

// Temporary until a use a specific math library.

struct Vector3
{
	Vector3() :
		X(0),
		Y(0),
		Z(0)
	{

	}

	Vector3(float InX, float InY, float InZ) :
		X(InX),
		Y(InY),
		Z(InZ)
	{

	}

	float X;
	float Y;
	float Z;
};

struct Quaternion
{
	Quaternion() :
		X(0),
		Y(0),
		Z(0),
		W(0)
	{

	}

	Quaternion(float InX, float InY, float InZ, float InW) :
		X(InX),
		Y(InY),
		Z(InZ),
		W(InW)
	{

	}

	float X;
	float Y;
	float Z;
	float W;
};