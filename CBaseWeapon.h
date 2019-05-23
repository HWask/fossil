#pragma once
#include <Windows.h>
#include "Utils.h"

class CBaseWeapon
{
public:
	enum CSGO_Weapon_ID
	{
		weapon_none,
		weapon_deagle,
		weapon_elite,
		weapon_fiveseven,
		weapon_glock,
		weapon_p228,
		weapon_usp,
		weapon_ak47,
		weapon_aug,
		weapon_awp,
		weapon_famas,
		weapon_g3sg1,
		weapon_galil,
		weapon_galilar,
		weapon_m249,
		weapon_m3,
		weapon_m4a1,
		weapon_mac10,
		weapon_mp5navy,
		weapon_p90,
		weapon_scout,
		weapon_sg550,
		weapon_sg552,
		weapon_tmp,
		weapon_ump45,
		weapon_xm1014,
		weapon_bizon,
		weapon_mag7,
		weapon_negev,
		weapon_sawedoff,
		weapon_tec9,
		weapon_taser,
		weapon_hkp2000,
		weapon_mp7,
		weapon_mp9,
		weapon_nova,
		weapon_p250,
		weapon_scar17,
		weapon_scar20,
		weapon_sg556,
		weapon_ssg08,
		weapon_knifegg,
		weapon_knife,
		weapon_flashbang,
		weapon_hegrenade,
		weapon_smokegrenade,
		weapon_molotov,
		weapon_decoy,
		weapon_incgrenade,
		weapon_c4,
		MAX
	};

	const char* GetWeaponName(int WeaponID)
	{
		switch (WeaponID)
		{
		case 0:
			return "None";
		case 1:
			return "Deagle";
		case 2:
			return "Elite";
		case 3:
			return "Fiveseven";
		case 4:
			return "Glock";
		case 5:
			return "P228";
		case 6:
			return "USP";
		case 7:
			return "AK47";
		case 8:
			return "AUG";
		case 9:
			return "AWP";
		case 10:
			return "Famas";
		case 11:
			return "G3SG1";
		case 12:
			return "Galil";
		case 13:
			return "Galilar";
		case 14:
			return "M249";
		case 15:
			return "M3";
		case 16:
			return "M4A1";
		case 17:
			return "Mac10";
		case 18:
			return "MP5Navy";
		case 19:
			return "P90";
		case 20:
			return "Scout";
		case 21:
			return "SG550";
		case 22:
			return "SG552";
		case 23:
			return "TMP";
		case 24:
			return "UMP45";
		case 25:
			return "XM1014";
		case 26:
			return "Bizon";
		case 27:
			return "Mag7";
		case 28:
			return "Negev";
		case 29:
			return "Sawedoff";
		case 30:
			return "Tec9";
		case 31:
			return "Taser";
		case 32:
			return "HKP2000";
		case 33:
			return "MP7";
		case 34:
			return "MP9";
		case 35:
			return "Nova";
		case 36:
			return "P250";
		case 37:
			return "Scar17";
		case 38:
			return "Scar20";
		case 39:
			return "SG556";
		case 40:
			return "SSG08";
		case 41:
			return "Knifeegg";
		case 42:
			return "Knife";
		case 43:
			return "Flash";
		case 44:
			return "He";
		case 45:
			return "Smoke";
		case 46:
			return "Molotov";
		case 47:
			return "Decoy";
		case 48:
			return "Incgrenade";
		case 49:
			return "C4";
		default:
			return "Unknown";
		}
	}
	
	int GetWeaponID()
	{
		return *(int*)(((DWORD)this) + 0x00001694);
	}

	bool static IsWeaponNonAim(int iWeaponID)
	{
		return iWeaponID == weapon_knifegg || iWeaponID == weapon_knife || iWeaponID == weapon_flashbang || iWeaponID == weapon_hegrenade || iWeaponID == weapon_smokegrenade
			|| iWeaponID == weapon_molotov || iWeaponID == weapon_decoy || iWeaponID == weapon_incgrenade || iWeaponID == weapon_c4;
	}

	bool static IsWeaponPistol(int iWeaponID)
	{
		return iWeaponID == weapon_deagle || iWeaponID == weapon_elite || iWeaponID == weapon_fiveseven || iWeaponID == weapon_glock
			|| iWeaponID == weapon_p228 || iWeaponID == weapon_p250 || iWeaponID == weapon_usp || iWeaponID == weapon_tec9 || iWeaponID == weapon_taser || iWeaponID == weapon_hkp2000;
	}

	bool static IsWeaponShotgun(int iWeaponID)
	{
		return iWeaponID == weapon_mag7 || iWeaponID == weapon_xm1014 || iWeaponID == weapon_m3 || iWeaponID == weapon_sawedoff || iWeaponID == weapon_nova;
	}

	bool static IsWeaponSniper(int iWeaponID)
	{
		return iWeaponID == weapon_awp || iWeaponID == weapon_g3sg1 || iWeaponID == weapon_scout
			|| iWeaponID == weapon_ssg08 || iWeaponID == weapon_scar20;
	}

	int GetClip1()
	{
		return *(int*)((DWORD)this + (DWORD)0x000015C0);
	}

	float NextPrimaryAttack()
	{
		return *(float*)((DWORD)this + (DWORD)0x00001598);
	}
};