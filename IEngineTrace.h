#pragma once
#include "ray_t.h"
#include "trace_t.h"
#include "ITraceFilter.h"

class IEngineTrace
{
public:
	void TraceRay(ray_t& ray, unsigned int mask, ITraceFilter* ptraceFilter, trace_t* ptrace)
	{
		typedef void(__thiscall* oTraceRay)(PVOID, ray_t&, unsigned int, ITraceFilter*, trace_t*);
		return Utils::vfunc<oTraceRay>(this, 5)(this, ray, mask, ptraceFilter, ptrace);
	}
};