#pragma once
#include <Windows.h>
#include "Vector.h"

struct cplane_t
{
	Vector      normal;
	float       dist;
	BYTE        type;
	BYTE        signbits;
	BYTE        pad[2];
};
struct csurface_t
{
	const char    *name;
	short        surfaceProps;
	unsigned short    flags;
};

struct trace_t
{
	Vector            startpos;
	Vector            endpos;
	cplane_t        plane;
	float            fraction;
	int                contents;
	unsigned short    dispFlags;
	bool            allsolid;
	bool            startsolid;
	float        fractionleftsolid;
	csurface_t    surface;
	int            hitgroup;
	short        physicsbone;
	unsigned short    worldSurfaceIndex;
	IClientEntity*	pEnt;
	int            hitbox;
};