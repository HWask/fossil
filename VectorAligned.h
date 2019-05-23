#pragma once
#include "Vector.h"

class __declspec(align(16)) VectorAligned : public Vector
{
public:
	float w;
};