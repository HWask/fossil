#pragma once

#include "VectorAligned.h"

class ray_t
{
public:
	VectorAligned  m_Start;
	VectorAligned  m_Delta;
	VectorAligned  m_StartOffset;
	VectorAligned  m_Extents;

	const matrix3x4_t *m_pWorldAxisTransform;
	bool    m_IsRay;
	bool    m_IsSwept;

	void Init(Vector& start, Vector& end)
	{
		m_Delta.x = end.x - start.x;
		m_Delta.y = end.y - start.y;
		m_Delta.z = end.z - start.z;
		m_Delta.w = 0;

		m_IsSwept = (m_Delta.lengthSqr() != 0);
		m_pWorldAxisTransform = NULL;
		m_IsRay = true;

		m_Extents.x = 0;
		m_Extents.y = 0;
		m_Extents.z = 0;
		m_Extents.w = 0;

		m_StartOffset = m_Extents;

		m_Start.x = start.x;
		m_Start.y = start.y;
		m_Start.z = start.z;
		m_Start.w = 0;
	}
};