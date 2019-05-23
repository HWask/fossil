#pragma once
#include <Windows.h>

class CClientClass
{
public:
	char* GetName()
	{
		return (char*)*(DWORD*)((DWORD)this + 0x8);
	}

	CClientClass* GetNextClass()
	{
		return (CClientClass*)*(DWORD*)((DWORD)this + 0x10);
	}
};