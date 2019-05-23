#pragma once
#include "Vector.h"

class IDebugOverlay
{
public:
	virtual void AddEntityTextOverlay(int ent_index, int line_offset, float flDuration, int r, int g, int b, int a, const char *text) = 0;
	virtual void AddBoxOverlay(const Vector& origin, const Vector& mins, const Vector& max, const Vector & angles, int r, int g, int b, int a, float flDuration) = 0;
	virtual void AddSphereOverlay(const Vector& vOrigin, float flRadius, int nTheta, int nPhi, int r, int g, int b, int a, float flDuration) = 0;
	virtual void AddSweptBoxOverlay(const Vector& start, const Vector& end, const Vector& mins, const Vector& max, const Vector & angles, int r, int g, int b, int a, float flDuration) = 0;
	virtual void AddGridOverlay(const Vector& vPos) = 0;
	virtual void AddLineOverlay(const Vector& origin, const Vector& dest, int r, int g, int b, int a, bool noDepthTest, float flDuration) = 0;
	virtual void AddTriangleOverlay(const Vector& p1, const Vector& p2, const Vector &p3, int r, int g, int b, int a, bool noDepthTest, float flDuration) = 0;
	virtual void AddTextOverlay(const Vector& origin, float flDuration, const char *text) = 0;
	virtual void AddTextOverlay(const Vector& origin, int line_offset, float flDuration, const char *text) = 0;
	virtual void AddTextOverlay(const Vector& origin, int line_offset, float flDuration, float alpha, const char *text) = 0;
	virtual void AddTextOverlay(const Vector& origin, int line_offset, float flDuration, float r, float g, float b, float alpha, const char *text) = 0;
	virtual void AddScreenTextOverlay(float flXPos, float flYPos, float flDuration, int r, int g, int b, int a, const char *text) = 0;
	virtual void AddTextOverlay(const Vector& textPos, float duration, float alpha, const char *text) = 0;
	virtual void Draw3DOverlays(void) = 0;
};