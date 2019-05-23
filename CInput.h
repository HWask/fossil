#pragma once
#include "CUserCmd.h"

class CInput
{
public:
	CUserCmd* GetUserCmd(int slot, int sequence_number)
	{
		typedef CUserCmd*(__thiscall* oGetUserCmd)(PVOID, int slot, int sequence_number);
		return Utils::vfunc<oGetUserCmd>(this, 8)(this, slot, sequence_number);
	}
};