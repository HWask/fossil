#pragma once
#include "CBaseWeapon.h"
#include "GUI\ConCheatClass.h"

class WeaponConfig : public ConsoleCheatClass
{
private:
	struct Config
	{
		bool rcs = true;
		bool trigger = true;
		bool aimbot = true;
		float fov = 20.0;
		float smooth = 2000.0;
		float rcssmooth = 1;
		int minAimDist = 0;
	};

	Config weaponCfg[CBaseWeapon::CSGO_Weapon_ID::MAX];
public:
	static int toIndex(int weaponID, int eleIndex)
	{
		return weaponID != 0 ? weaponID * 10 + eleIndex : 0;
	}

	int MinAimDist(int weaponID)
	{
		return weaponCfg[weaponID].minAimDist;
	}

	float rcs_smooth(int weaponID)
	{
		return weaponCfg[weaponID].rcssmooth;
	}

	bool aimbot(int weaponID)
	{
		return weaponCfg[weaponID].aimbot;
	}

	bool trigger(int weaponID)
	{
		return weaponCfg[weaponID].trigger;
	}

	bool rcs(int weaponID)
	{
		return weaponCfg[weaponID].rcs;
	}

	float fov(int weaponID)
	{
		return weaponCfg[weaponID].fov;
	}

	float smooth(int weaponID)
	{
		return weaponCfg[weaponID].smooth;
	}

	int GetIValue(int index) override
	{
		int weaponIndex = index / 10;
		int structIndex = index % 10;

		if (structIndex == 6)
			return weaponCfg[weaponIndex].minAimDist;

		return 0;
	}

	bool GetBoolValue(int index) override
	{
		int weaponIndex = index / 10;
		int structIndex = index % 10;

		if (structIndex == 0)
			return weaponCfg[weaponIndex].rcs;

		if (structIndex == 1)
			return weaponCfg[weaponIndex].trigger;

		if (structIndex == 2)
			return weaponCfg[weaponIndex].aimbot;
	}

	float GetFLValue(int index) override
	{
		int weaponIndex = index / 10;
		int structIndex = index % 10;

		if (structIndex == 3)
			return weaponCfg[weaponIndex].fov;

		if (structIndex == 4)
			return weaponCfg[weaponIndex].smooth;

		if (structIndex == 5)
			return weaponCfg[weaponIndex].rcssmooth;
	}

	std::vector<int> GetVECValue(int index) override
	{
		return std::vector<int>();
	}

	void SetIValue(int val, int index) override
	{
		int weaponIndex = index / 10;
		int structIndex = index % 10;

		if (structIndex == 6)
			weaponCfg[weaponIndex].minAimDist = val;
	}

	void SetBoolValue(bool val, int index) override
	{
		int weaponIndex = index / 10;
		int structIndex = index % 10;

		if (structIndex == 0)
			weaponCfg[weaponIndex].rcs = val;

		if (structIndex == 1)
			weaponCfg[weaponIndex].trigger = val;

		if (structIndex == 2)
			weaponCfg[weaponIndex].aimbot = val;
	}

	void SetFLValue(float val, int index) override
	{
		int weaponIndex = index / 10;
		int structIndex = index % 10;

		if (structIndex == 3)
			weaponCfg[weaponIndex].fov = val;

		if (structIndex == 4)
			weaponCfg[weaponIndex].smooth = val;

		if (structIndex == 5)
			weaponCfg[weaponIndex].rcssmooth = val;
	}

	void SetVECValue(std::vector<int> val, int index) override
	{
	}
};