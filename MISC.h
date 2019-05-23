#pragma once
#include <iomanip>
#include <string>
#include <sstream>
#include "GUI\Observer.h"

#include "CWeaponConfig.h"

class MISC : public Observer
{
public:
	enum CROSSHAIR
	{
		eCSGO,
		eOlofMeister,
		eCustom
	};

	enum WALLHACK
	{
		eNone,
		eColoredModels,
		eChams
	};
private:
	enum Cheats
	{
		ercs,
		erecoilxhair,
		ewallhack,
		eautopistol,
		ecrosshair,
		ecrosshairtype,
		eradar,
		ebunnyhop,
		espinbot,
		eantiaim,
		efakeduck,
		elaggswitch,
		MAX
	};

	bool bIsCreated = false;
	Factory* m_pFactory;

	CROSSHAIR xhairtype = CROSSHAIR::eCSGO;
	bool radar = false;
	bool xhair = false;

	bool rcs = false;
	WALLHACK wallhack = WALLHACK::eNone;
	bool recoilXhair = false;
	bool bBunnyHop = false;
	bool bAutoPistol = false;
	bool bSpinbot = false;
	bool bAntiAim = false;
	bool bFakeDuck = false;
	bool bLaggSwitch = false;
	Vector vecRecoilXHair;
public:
	WeaponConfig* _wpnCfg;
public:

	MISC(WeaponConfig* wpnCfg) : _wpnCfg(wpnCfg) {}

	bool IsCreated()
	{
		return bIsCreated;
	}

	CROSSHAIR getxhairtype()
	{
		return xhairtype;
	}

	WALLHACK getWallhackType()
	{
		return wallhack;
	}

	bool AntiAim()
	{
		return bAntiAim;
	}

	bool LaggSwitch()
	{
		return bLaggSwitch;
	}

	bool FakeDuck()
	{
		return bFakeDuck;
	}

	bool AutoPistol()
	{
		return bAutoPistol;
	}

	bool Spinbot()
	{
		return bSpinbot;
	}

	bool BunnyHop()
	{
		return bBunnyHop;
	}

	void Create(Factory* pFactory)
	{
		m_pFactory = pFactory;
	}

	bool radarEnabled()
	{
		return radar;
	}

	void GetMovementFix(unsigned int state, float oForwardMove, float oSideMove, CUserCmd* cmd)
	{
		//fix movement Q_Q
		switch (state % 8)
		{
		case 0:
			break;
		case 1:
			if (cmd->forwardmove == 0)
			{
				if (cmd->sidemove == 0)
				{
					cmd->forwardmove = 0;
					cmd->sidemove = 0;
				}
				else if (cmd->sidemove < 0)
				{
					cmd->forwardmove = 450;
					cmd->sidemove = -450;
				}
				else if (cmd->sidemove > 0)
				{
					cmd->forwardmove = -450;
					cmd->sidemove = 450;
				}
			}
			else if (cmd->forwardmove < 0)
			{
				if (cmd->sidemove == 0)
				{
					cmd->forwardmove = -450;
					cmd->sidemove = -450;
				}
				else if (cmd->sidemove < 0)
				{
					cmd->forwardmove = 0;
					cmd->sidemove = -450;
				}
				else if (cmd->sidemove > 0)
				{
					cmd->forwardmove = -450;
					cmd->sidemove = 0;
				}
			}
			else if (cmd->forwardmove > 0)
			{
				if (cmd->sidemove == 0)
				{
					cmd->forwardmove = 450;
					cmd->sidemove = 450;
				}
				else if (cmd->sidemove < 0)
				{
					cmd->forwardmove = 450;
					cmd->sidemove = 0;
				}
				else if (cmd->sidemove > 0)
				{
					cmd->forwardmove = 0;
					cmd->sidemove = 450;
				}
			}
			break;
		case 2:
			cmd->forwardmove = -oSideMove;
			cmd->sidemove = oForwardMove;
			break;
		case 3:
			if (cmd->forwardmove == 0)
			{
				if (cmd->sidemove == 0)
				{
					cmd->sidemove = 0;
					cmd->forwardmove = 0;
				}
				else if (cmd->sidemove < 0)
				{
					cmd->sidemove = 450;
					cmd->forwardmove = 450;
				}
				else if (cmd->sidemove > 0)
				{
					cmd->sidemove = -450;
					cmd->forwardmove = -450;
				}
			}
			else if (cmd->forwardmove < 0)
			{
				if (cmd->sidemove == 0)
				{
					cmd->sidemove = -450;
					cmd->forwardmove = 450;
				}
				else if (cmd->sidemove < 0)
				{
					cmd->sidemove = 0;
					cmd->forwardmove = 450;
				}
				else if (cmd->sidemove > 0)
				{
					cmd->sidemove = -450;
					cmd->forwardmove = 0;
				}
			}
			else if (cmd->forwardmove > 0)
			{
				if (cmd->sidemove == 0)
				{
					cmd->sidemove = 450;
					cmd->forwardmove = -450;
				}
				else if (cmd->sidemove < 0)
				{
					cmd->sidemove = 450;
					cmd->forwardmove = 0;
				}
				else if (cmd->sidemove > 0)
				{
					cmd->sidemove = 0;
					cmd->forwardmove = -450;
				}
			}
			break;
		case 4:
			cmd->forwardmove = -oForwardMove;
			cmd->sidemove = -oSideMove;
			break;
		case 5:
			if (cmd->forwardmove == 0)
			{
				if (cmd->sidemove == 0)
				{
					cmd->forwardmove = 0;
					cmd->sidemove = 0;
				}
				else if (cmd->sidemove < 0)
				{
					cmd->forwardmove = -450;
					cmd->sidemove = 450;
				}
				else if (cmd->sidemove > 0)
				{
					cmd->forwardmove = 450;
					cmd->sidemove = -450;
				}
			}
			else if (cmd->forwardmove < 0)
			{
				if (cmd->sidemove == 0)
				{
					cmd->forwardmove = 450;
					cmd->sidemove = 450;
				}
				else if (cmd->sidemove < 0)
				{
					cmd->forwardmove = 0;
					cmd->sidemove = 450;
				}
				else if (cmd->sidemove > 0)
				{
					cmd->forwardmove = 450;
					cmd->sidemove = 0;
				}
			}
			else if (cmd->forwardmove > 0)
			{
				if (cmd->sidemove == 0)
				{
					cmd->forwardmove = -450;
					cmd->sidemove = -450;
				}
				else if (cmd->sidemove < 0)
				{
					cmd->forwardmove = -450;
					cmd->sidemove = 0;
				}
				else if (cmd->sidemove > 0)
				{
					cmd->forwardmove = 0;
					cmd->sidemove = -450;
				}
			}
			break;
		case 6:
			cmd->forwardmove = oSideMove;
			cmd->sidemove = -oForwardMove;
			break;
		case 7:
			if (cmd->forwardmove == 0)
			{
				if (cmd->sidemove == 0)
				{
					cmd->sidemove = 0;
					cmd->forwardmove = 0;
				}
				else if (cmd->sidemove < 0)
				{
					cmd->sidemove = -450;
					cmd->forwardmove = -450;
				}
				else if (cmd->sidemove > 0)
				{
					cmd->sidemove = 450;
					cmd->forwardmove = 450;
				}
			}
			else if (cmd->forwardmove < 0)
			{
				if (cmd->sidemove == 0)
				{
					cmd->sidemove = 450;
					cmd->forwardmove = -450;
				}
				else if (cmd->sidemove < 0)
				{
					cmd->sidemove = 0;
					cmd->forwardmove = -450;
				}
				else if (cmd->sidemove > 0)
				{
					cmd->sidemove = 450;
					cmd->forwardmove = 0;
				}
			}
			else if (cmd->forwardmove > 0)
			{
				if (cmd->sidemove == 0)
				{
					cmd->sidemove = -450;
					cmd->forwardmove = 450;
				}
				else if (cmd->sidemove < 0)
				{
					cmd->sidemove = -450;
					cmd->forwardmove = 0;
				}
				else if (cmd->sidemove > 0)
				{
					cmd->sidemove = 0;
					cmd->forwardmove = 450;
				}
			}
			break;
		}
	}

	void Radar()
	{
		auto local = m_pFactory->m_pEngine->GetLocalPlayer();
		auto ME = m_pFactory->m_pEntList->GetClientEntity(local);
		if (!ME)
			return;

		auto myTeam = ME->GetTeamNum();

		for (int i = 0; i < m_pFactory->m_pEngine->GetMaxClients(); i++)
		{
			if (i == local)
				continue;

			auto ent = m_pFactory->m_pEntList->GetClientEntity(i);
			if (ent && ent->IsAlive() && ent->GetTeamNum() != myTeam)
			{
				*(bool*)((DWORD)ent + 0x935) = true;
			}
		}
	}

	bool recoilXHair() {
		return recoilXhair;
	}

	Vector recoilXHairPos() {
		return vecRecoilXHair;
	}

	void setrecoilXHairPos(Vector v) {
		vecRecoilXHair = v;
	}

	bool recoilControl() {
		return rcs;
	}

	bool Crosshair()
	{
		return xhair;
	}

	void Update()
	{
		int i = 0;
		for (auto it = items.begin(); it != items.end(); ++it, i++)
		{
			switch (i)
			{
			case ercs:
				rcs = (*it)->GetbValue();
				break;
			case erecoilxhair:
				recoilXhair = (*it)->GetbValue();
				break;
			case ewallhack:
				wallhack = (WALLHACK)(*it)->GetiValue();
				break;
			case eautopistol:
				bAutoPistol = (*it)->GetbValue();
				break;
			case ecrosshair:
				xhair = (*it)->GetbValue();
				break;
			case ecrosshairtype:
				xhairtype = (CROSSHAIR)(*it)->GetiValue();
				break;
			case eradar:
				radar = (*it)->GetbValue();
				break;
			case ebunnyhop:
				bBunnyHop = (*it)->GetbValue();
				break;
			case espinbot:
				bSpinbot = (*it)->GetbValue();
				break;
			case eantiaim:
				bAntiAim = (*it)->GetbValue();
				break;
			case efakeduck:
				bFakeDuck = (*it)->GetbValue();
				break;
			case elaggswitch:
				bLaggSwitch = (*it)->GetbValue();
				break;
			default:
				break;
			}
		}
	}
};