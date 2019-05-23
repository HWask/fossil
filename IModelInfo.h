#pragma once
#include "stdio.h"

typedef void model_t;

class IModelInfo
{
public:
	studiohdr_t* GetStudioModel(void* mod)
	{
		typedef studiohdr_t*(__thiscall* tGetStudioModel)(PVOID, void*);
		return Utils::vfunc<tGetStudioModel>(this, 30)(this, mod);
	}

	const char* GetModelName(void* model)
	{
		typedef char*(__thiscall* tGetModelName)(PVOID, void*);
		return Utils::vfunc<tGetModelName>(this, 3)(this, model);
	}
};