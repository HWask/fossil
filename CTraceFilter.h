#pragma once
#include "SDK.h"

class CTraceFilter : public ITraceFilter
{
public:
	CTraceFilter(void* pPassEnt1, void* pPassEnt2)
	{
		passentity1 = pPassEnt1;
		passentity2 = pPassEnt2;
	}

	virtual bool ShouldHitEntity(IClientEntity* pEntity, int contentsMask)
	{
		return !(pEntity == passentity1 || pEntity == passentity2);
	}

	virtual TraceType_t GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}

	void* passentity1;
	void* passentity2;
};