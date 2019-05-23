#pragma once
#include "IClientEntity.h"

enum TraceType_t
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,
	TRACE_ENTITIES_ONLY,
	TRACE_EVERYTHING_FILTER_PROPS,
};

class ITraceFilter
{
public:
	virtual bool ShouldHitEntity(IClientEntity* pEntity, int contentsMask) = 0;
	virtual TraceType_t GetTraceType() const = 0;
};