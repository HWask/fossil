#pragma once
#include "Utils.h"

class IPanel
{
public:
	const char* GetName(unsigned int vguiPanel)
	{
		typedef const char* (__thiscall* GetPanelNameFn)(void*, unsigned int);
		return Utils::vfunc< GetPanelNameFn >(this, 36)(this, vguiPanel);
	}
};