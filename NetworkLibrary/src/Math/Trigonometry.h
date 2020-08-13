#pragma once
#include <cmath>

struct Vector2
{
public:

	float		mX, mY;

	Vector2(float x, float y) :
		mX(x),
		mY(y)
	{}

	Vector2() :
		mX(0.0f),
		mY(0.0f)
	{}

	friend Vector2 operator+(const Vector2& inLeft, const Vector2& inRight)
	{
		return Vector2(inLeft.mX + inRight.mX, inLeft.mY + inRight.mY);
	}

	friend Vector2 operator-(const Vector2& inLeft, const Vector2& inRight)
	{
		return Vector2(inLeft.mX - inRight.mX, inLeft.mY - inRight.mY);
	}

	friend Vector2 operator*(const Vector2& inLeft, const Vector2& inRight)
	{
		return Vector2(inLeft.mX * inRight.mX, inLeft.mY * inRight.mY);
	}

	friend Vector2 operator*(float inScalar, const Vector2& inVec)
	{
		return Vector2(inVec.mX * inScalar, inVec.mY * inScalar);
	}

	friend Vector2 operator*(const Vector2& inVec, float inScalar)
	{
		return Vector2(inVec.mX * inScalar, inVec.mY * inScalar);
	}

	Vector2& operator*=(float inScalar)
	{
		mX *= inScalar;
		mY *= inScalar;
		return *this;
	}

	Vector2& operator+=(const Vector2& inRight)
	{
		mX += inRight.mX;
		mY += inRight.mY;
		return *this;
	}

	Vector2& operator-=(const Vector2& inRight)
	{
		mX -= inRight.mX;
		mY -= inRight.mY;
		return *this;
	}

	float Length()
	{
		return std::sqrtf(mX * mX + mY * mY);
	}

	float LengthSq()
	{
		return mX * mX + mY * mY;
	}

	void Normalize()
	{
		float length = Length();
		mX /= length;
		mY /= length;
	}

	friend float Dot(const Vector2& inLeft, const Vector2& inRight)
	{
		return (inLeft.mX * inRight.mX + inLeft.mY * inRight.mY);
	}

	friend Vector2 Lerp(const Vector2& inA, const Vector2& inB, float t)
	{
		return Vector2(inA + t * (inB - inA));
	}
};

struct Vector3
{
public:

	float		mX, mY, mZ;

	Vector3(float x, float y, float z) :
		mX(x),
		mY(y),
		mZ(z)
	{}

	Vector3() :
		mX(0.0f),
		mY(0.0f),
		mZ(0.0f)
	{}

	friend Vector3 operator+(const Vector3& inLeft, const Vector3& inRight)
	{
		return Vector3(inLeft.mX + inRight.mX, inLeft.mY + inRight.mY, inLeft.mZ + inRight.mZ);
	}

	friend Vector3 operator-(const Vector3& inLeft, const Vector3& inRight)
	{
		return Vector3(inLeft.mX - inRight.mX, inLeft.mY - inRight.mY, inLeft.mZ - inRight.mZ);
	}

	friend Vector3 operator*(const Vector3& inLeft, const Vector3& inRight)
	{
		return Vector3(inLeft.mX * inRight.mX, inLeft.mY * inRight.mY, inLeft.mZ * inRight.mZ);
	}

	friend Vector3 operator*(float inScalar, const Vector3& inVec)
	{
		return Vector3(inVec.mX * inScalar, inVec.mY * inScalar, inVec.mZ * inScalar);
	}

	friend Vector3 operator*(const Vector3& inVec, float inScalar)
	{
		return Vector3(inVec.mX * inScalar, inVec.mY * inScalar, inVec.mZ * inScalar);
	}

	Vector3& operator*=(float inScalar)
	{
		mX *= inScalar;
		mY *= inScalar;
		mZ *= inScalar;
		return *this;
	}

	Vector3& operator+=(const Vector3& inRight)
	{
		mX += inRight.mX;
		mY += inRight.mY;
		mZ += inRight.mZ;
		return *this;
	}

	Vector3& operator-=(const Vector3& inRight)
	{
		mX -= inRight.mX;
		mY -= inRight.mY;
		mZ -= inRight.mZ;
		return *this;
	}

	float Length()
	{
		return std::sqrtf(mX * mX + mY * mY + mZ * mZ);
	}

	float LengthSq()
	{
		return mX * mX + mY * mY + mZ * mZ;
	}

	void Normalize()
	{
		float length = Length();
		mX /= length;
		mY /= length;
		mZ /= length;
	}

	friend float Dot(const Vector3& inLeft, const Vector3& inRight)
	{
		return (inLeft.mX * inRight.mX + inLeft.mY * inRight.mY + inLeft.mZ * inRight.mZ);
	}

	friend Vector3 Cross(const Vector3& inLeft, const Vector3& inRight)
	{
		Vector3 temp;
		temp.mX = inLeft.mY * inRight.mZ - inLeft.mZ * inRight.mY;
		temp.mY = inLeft.mZ * inRight.mX - inLeft.mX * inRight.mZ;
		temp.mZ = inLeft.mX * inRight.mY - inLeft.mY * inRight.mX;
		return temp;
	}

	friend Vector3 Lerp(const Vector3& inA, const Vector3& inB, float t)
	{
		return Vector3(inA + t * (inB - inA));
	}
};


struct Quaternion
{
public:

	float		mX, mY, mZ, mW;

	Quaternion(float x, float y, float z, float w) :
		mX(x),
		mY(y),
		mZ(z),
		mW(w)
	{}

	Quaternion() :
		mX(0.0f),
		mY(0.0f),
		mZ(0.0f),
		mW(0.0f)
	{}
};

inline float ToDegrees(float inRadians)
{
	return inRadians * 180.0f / 3.1415926535f;
}
