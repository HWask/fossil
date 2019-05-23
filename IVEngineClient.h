#pragma once
#include "Utils.h"
#include "PlayerInfo.h"
#include "Vector.h"

class IVEngineClient
{
public:
	void GetScreenSize(int& width, int& height)
	{
		typedef void(__thiscall* oGetScreenSize)(PVOID, int&, int&);
		return Utils::vfunc<oGetScreenSize>(this, 5)(this, width, height);
	}

	void ClientCmd(const char* szCmdString)
	{
		typedef void(__thiscall* oClientCmd)(PVOID, const char*);
		return Utils::vfunc<oClientCmd>(this, 7)(this, szCmdString);
	}

	bool GetPlayerInfo(int ent_num, player_info_t *pinfo)
	{
		typedef bool(__thiscall* oGetPlayerInfo)(PVOID, int, player_info_t*);
		return Utils::vfunc<oGetPlayerInfo>(this, 8)(this, ent_num, pinfo);
	}

	int GetLocalPlayer()
	{
		typedef int(__thiscall* oLocal)(PVOID);
		return Utils::vfunc<oLocal>(this, 12)(this);
	}

	float GetLastTimeStamp()
	{
		typedef float(__thiscall* oGetLastTimeStamp)(PVOID);
		return Utils::vfunc<oGetLastTimeStamp>(this, 14)(this);
	}

	void SetViewAngles(Vector& vAngles)
	{
		typedef void(__thiscall* oSetViewAngles)(PVOID, Vector&);
		return Utils::vfunc<oSetViewAngles>(this, 19)(this, vAngles);
	}

	void GetViewAngles(Vector& vAngles)
	{
		typedef void(__thiscall* oSetViewAngles)(PVOID, Vector&);
		return Utils::vfunc<oSetViewAngles>(this, 18)(this, vAngles);
	}

	int GetMaxClients()
	{
		typedef bool(__thiscall* oGetMaxClients)(PVOID);
		return Utils::vfunc<oGetMaxClients>(this, 20)(this);
	}

	bool IsConnected()
	{
		typedef bool(__thiscall* oGetScreenSize)(PVOID);
		return Utils::vfunc<oGetScreenSize>(this, 27)(this);
	}

	bool IsInGame()
	{
		typedef bool(__thiscall* oLocal)(PVOID);
		return Utils::vfunc<oLocal>(this, 26)(this);
	}

	D3DXMATRIX& WorldToScreenMatrix()
	{
		typedef D3DXMATRIX& (__thiscall* oWorldToScreenMatrix)(PVOID);
		return Utils::vfunc<oWorldToScreenMatrix>(this, 37)(this);
	}
};