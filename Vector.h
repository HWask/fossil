#pragma once
#include <math.h>
#include <d3dx9.h>

class Vector
{
public:
	Vector() {}
	Vector(float _X, float _Y, float _Z) : x(_X), y(_Y), z(_Z) {}
	Vector(float _X, float _Y) : x(_X), y(_Y), z(0) {}

	float x = 0;
	float y = 0;
	float z = 0;

	Vector& operator=(Vector r)
	{
		x = r.x;
		y = r.y;
		z = r.z;
		return *this;
	}

	Vector operator+(Vector& r)
	{
		return Vector(x + r.x, y + r.y, z + r.z);
	}

	Vector operator-(Vector& r)
	{
		return Vector(x - r.x, y - r.y, z - r.z);
	}

	Vector operator+(float r)
	{
		return Vector(x + r, y + r, z + r);
	}

	Vector operator-(float r)
	{
		return Vector(x - r, y - r, z - r);
	}

	Vector operator/(float r)
	{
		return Vector(x/r, y/r, z/r);
	}

	Vector operator*(float r)
	{
		return Vector(x * r, y * r, z * r);
	}

	Vector operator*(int r)
	{
		return Vector(x * r, y * r, z * r);
	}

	//Euklidische Norm du wixxa XDDD
	float length()
	{
		return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	}

	float lengthSqr()
	{
		return (pow(x, 2) + pow(y, 2) + pow(z, 2));
	}

	float static Angle(Vector& a, Vector& b)
	{
		float dotProduct = a.x*b.x + a.y*b.y + a.z*b.z;
		return D3DXToDegree(acos(dotProduct / (a.length()*b.length())));
	}

	Vector static ClampAngle(Vector qaAng)
	{
		Vector ret;
		ret.x = qaAng.x;
		ret.y = qaAng.y;

		if (qaAng.x > 89.0f && qaAng.x <= 180.0f)
			ret.x = 89.0f;

		if (qaAng.x > 180.0f)
			ret.x = qaAng.x - 360.0f;

		if (qaAng.x < -89.0f)
			ret.x = -89.0f;

		if (qaAng.y > 180.0f)
			ret.y = qaAng.y  - 360.0f;

		if (qaAng.y < -180.0f)
			ret.y = qaAng.y + 360.0f;

		ret.z = 0;

		return ret;
	}
};