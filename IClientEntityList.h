#pragma once
#include "Utils.h"
#include "IClientEntity.h"

class IClientEntityList
{
public:
	IClientEntity* GetClientEntity(int entnum)
	{
		typedef IClientEntity*(__thiscall* oGetClientEntity)(PVOID, int);
		return Utils::vfunc<oGetClientEntity>(this, 3)(this, entnum);
	}

	IClientEntity* GetClientEntityFromHandle(unsigned long hEnt)
	{
		typedef IClientEntity*(__thiscall* oGetClientEntityFromHandle)(PVOID, unsigned long);
		return Utils::vfunc<oGetClientEntityFromHandle>(this, 4)(this, hEnt);
	}

	int GetHighestEntityIndex()
	{
		typedef int(__thiscall* oGetHighestEntityIndex)(PVOID);
		return Utils::vfunc<oGetHighestEntityIndex>(this, 6)(this);
	}
};