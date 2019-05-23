#include "Factory.h"
#include "CVMTHookManager.h"

#include "GUI\CMenu.h"
#include "GUI\CMenuConfigMgr.h"
#include "GUI\ConConsole.h"

#include "CWeaponConfig.h"
#include "MISC.h"
#include "Aimbot.h"
#include "Triggerbot.h"
#include "CRankBoard.h"
#include "ESP.h"
#include "EngineRender.h"

typedef HRESULT(__stdcall* EndScene_t)(LPDIRECT3DDEVICE9 pDevice);
typedef HRESULT(__stdcall* Reset_t)(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS *pPresentationParameters);
typedef HRESULT(__stdcall* DrawIndexedPrimitive_t)(IDirect3DDevice9*, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);

typedef void(__stdcall* CreateMove_t)(int sequence_number, float input_sample_frametime, bool active);
typedef void(__fastcall* PaintTraverse_t)(IPanel* pPanel, void* edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce);
typedef	int(__stdcall* KeyEvent_t)(int down, int keynum, const char* pszCurrentBinding);

HRESULT __stdcall hk_EndScene(LPDIRECT3DDEVICE9 pDevice);
HRESULT __stdcall hk_Reset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS *pPresentationParameters);
HRESULT __stdcall hk_DrawIndexedPrimitive(IDirect3DDevice9* Device, D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount);

void hk_CreateMoveWrapper(int sequence_number, float input_sample_frametime, bool active);
void __stdcall hk_CreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket);
void __fastcall hk_PaintTraverse(IPanel* pPanel, void* edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce);
int __stdcall hk_KeyEvent(int down, int keynum, const char* pszCurrentBinding);

DWORD WINAPI Thread(LPVOID lpParams);
void ApplyD3DHooks();
void ApplyEngineHooks();
void* traverse_stack(const void** ebp);
void InitMenuItems();
void UpdateStatus();

EndScene_t oEndScene;
Reset_t oReset;
DrawIndexedPrimitive_t oDrawIndexedPrimitive;

CreateMove_t oCreateMove;
PaintTraverse_t oPaintTraverse;
KeyEvent_t	oKeyEvent;

bool IsPixelShaderCreated = false;
IDirect3DPixelShader9* green;
IDirect3DPixelShader9* blue;
IDirect3DPixelShader9* yellow;
IDirect3DPixelShader9* red;

Factory* g_pFactory = new Factory();
CRender* g_pRender = new CRender();
CMenu* g_pMenu = new CMenu();

WeaponConfig* g_pWeaponConfig = new WeaponConfig();
MISC* g_pMISC = new MISC(g_pWeaponConfig);
Aimbot* g_pAimbot = new Aimbot(g_pWeaponConfig);
Triggerbot* g_pTriggerbot = new Triggerbot(g_pWeaponConfig);
CRankBoard* g_pRankBoard = new CRankBoard();
ESP* g_pESP = new ESP();
EngineRender* g_pEngineRender = new EngineRender();

float gSidemove = 0;
float gForwardmove = 0;

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID lpReserved)
{
	DisableThreadLibraryCalls(hInst);
	if (dwReason == DLL_PROCESS_ATTACH)
		CreateThread(0, 0, Thread, 0, 0, 0);

	return TRUE;
}

void ApplyD3DHooks()
{
	DWORD shaderapidx9 = 0;
	while (!shaderapidx9)
		shaderapidx9 = (DWORD)GetModuleHandle("shaderapidx9.dll");

	char sig[] = "\xC7\x44\x24\x0C\x00\x00\x00\x00\x8B\x77\x1C\xB9\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xA1";
	DWORD ShaderApi = Utils::FindPattern(shaderapidx9, 10e5, (BYTE*)sig, "xxxx????xxxx????x????x");
	DWORD ShaderBaseClass = **(DWORD**)(ShaderApi + sizeof(sig)-1);

#ifdef _DEBUG
	g_pFactory->Logger("ShaderApi: 0x%X", ShaderApi);
	g_pFactory->Logger("ShaderBaseClass: 0x%X", ShaderBaseClass);
#endif

	oEndScene = (EndScene_t)CVMTHookManager((PDWORD*)ShaderBaseClass).dwHookMethod((DWORD)hk_EndScene, 42);
	oReset = (Reset_t)CVMTHookManager((PDWORD*)ShaderBaseClass).dwHookMethod((DWORD)hk_Reset, 16);
	oDrawIndexedPrimitive = (DrawIndexedPrimitive_t)CVMTHookManager((PDWORD*)ShaderBaseClass).dwHookMethod((DWORD)hk_DrawIndexedPrimitive, 82);
}

void ApplyEngineHooks()
{
	CVMTHookManager mgr((PDWORD*)g_pFactory->m_pClient);
	oCreateMove = (CreateMove_t)mgr.dwHookMethod((DWORD)hk_CreateMoveWrapper, 21);
	oKeyEvent = (KeyEvent_t)mgr.dwHookMethod((DWORD)hk_KeyEvent, 20);

	CVMTHookManager mgr2((PDWORD*)g_pFactory->m_pPanel);
	oPaintTraverse = (PaintTraverse_t)mgr2.dwHookMethod((DWORD)hk_PaintTraverse, 41);
}

DWORD WINAPI Thread(LPVOID lpParams)
{
	g_pFactory->GetInterfaces();
	g_pFactory->GetCInput();
	g_pFactory->GetGlobalVars();
	g_pFactory->GetDrawPointsReturn();
	g_pRankBoard->getGameResourcesAddr();

	g_pAimbot->Create(g_pFactory);
	g_pTriggerbot->Create(g_pFactory);
	g_pEngineRender->Create(g_pFactory->m_pSurface);
	g_pMISC->Create(g_pFactory);

	g_pMenu->Initialize("f0ss!l reBorN");

	ApplyD3DHooks();
	ApplyEngineHooks();

	return 0;
}

int __stdcall hk_KeyEvent(int down, int keynum, const char* pszCurrentBinding)
{
	if (g_pMenu->MenuOpen())
		return 0;
	else
		return oKeyEvent(down, keynum, pszCurrentBinding);
}

void __fastcall hk_PaintTraverse(IPanel* pPanel, void* edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	if (!strcmp(pPanel->GetName(vguiPanel), "MatSystemTopPanel"))
	{
		if (!g_pEngineRender->IsFontCreated())
			g_pEngineRender->InitFont();

		if (!g_pESP->IsCreated())
			g_pESP->Create(g_pEngineRender, g_pFactory);

		g_pESP->EntESP();
		g_pESP->DrawESP();
	}

	oPaintTraverse(pPanel, edx, vguiPanel, forceRepaint, allowForce);
}

void __declspec(naked) hk_CreateMoveWrapper(int sequence_number, float input_sample_frametime, bool active)
{
	__asm
	{
		push ebp;
		mov ebp, esp;

		push ebx;

		lea ecx, [esp];
		push ecx;

		xor eax, eax;
		mov al, active;
		push eax;
		mov eax, input_sample_frametime;
		push eax;
		mov eax, sequence_number;
		push eax;

		call hk_CreateMove;

		pop ebx;
		pop ebp;

		retn 0xC;
	}
}

void __stdcall hk_CreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket)
{
	oCreateMove(sequence_number, input_sample_frametime, active);

	auto cmd = g_pFactory->m_pInput->GetUserCmd(0, sequence_number);
	CVerifiedUserCmd* verified_cmd = (CVerifiedUserCmd*)*(DWORD*)((DWORD)g_pFactory->m_pInput + 0xF0);
	if (!cmd || !verified_cmd)
		return;

	int local = g_pFactory->m_pEngine->GetLocalPlayer();
	auto ME = g_pFactory->m_pEntList->GetClientEntity(local);
	if (!ME)
		return;

	unsigned long hactiveWeapon = ME->GetActiveWeapon();
	CBaseWeapon* BaseWeapon = (CBaseWeapon*)g_pFactory->m_pEntList->GetClientEntityFromHandle(hactiveWeapon);
	if (!BaseWeapon)
		return;

	float flServerTime = (ME->GetTickBase() + 1) * g_pFactory->m_pGlobalVars->flIntervalsPerTick;

	Vector viewAnglesCopy(cmd->viewangles);
	CUserCmd cmdCopy = *cmd;
	float oSideMove = cmd->sidemove;
	float oForwardMove = cmd->forwardmove;

	//Triggerbot
	{
		static bool bulletShooting = false;
		static bool hasTarget = false;
		static bool shouldUpdate = true;
		static float lastAimTime = 999999;
		static int curClip;

		float flDelay = g_pTriggerbot->Delay();
		int burstShots = g_pTriggerbot->BurstShotAmount();

		if (g_pTriggerbot->Enabled())
		{
			if ((GetAsyncKeyState(VK_XBUTTON1) & 0x8000))
			{
				if (ME->IsAlive() &&
					!CBaseWeapon::IsWeaponNonAim(BaseWeapon->GetWeaponID()) &&
					BaseWeapon->GetClip1() > 0 &&
					g_pTriggerbot->_wpnCfg->trigger(BaseWeapon->GetWeaponID()))
				{
					if (hasTarget)
					{
						if (lastAimTime + flDelay / 100 < g_pFactory->m_pEngine->GetLastTimeStamp())
						{
							if (BaseWeapon->NextPrimaryAttack() <= flServerTime)
								cmd->buttons |= IN_ATTACK;
							else
								cmd->buttons &= ~IN_ATTACK;

							if (curClip - burstShots == BaseWeapon->GetClip1())
							{
								shouldUpdate = false;
								hasTarget = false;
							}
						}
					}
					else
					{
						Vector fixxedRecoil = cmd->viewangles;
						if (g_pTriggerbot->NoRecoil())
						{
							Helper::AddPunch(g_pFactory, cmd->viewangles, fixxedRecoil);
							fixxedRecoil = Vector::ClampAngle(fixxedRecoil);
						}

						if (g_pTriggerbot->TriggerShoot(fixxedRecoil))
						{
							if (shouldUpdate)
								lastAimTime = g_pFactory->m_pEngine->GetLastTimeStamp();

							hasTarget = true;
							curClip = BaseWeapon->GetClip1();
						}
					}
				}
				else
				{
					hasTarget = false;
					shouldUpdate = true;
				}
			}
			else
			{
				hasTarget = false;
				shouldUpdate = true;
			}
		}
	}

	bool bAimbot = false;
	//Legit
	{
		Vector aimAngles;
		static float lastAimTime;
		static bool shouldWrite;
		float aimLockTime = g_pAimbot->LockTime();
		bool bAutoShoot;
		float startTime;
		float curTime = g_pFactory->m_pEngine->GetLastTimeStamp();

		if (g_pAimbot->Enabled() && g_pAimbot->Legit())
		{
			if (cmd->buttons & IN_ATTACK)
			{
				if (!CBaseWeapon::IsWeaponNonAim(BaseWeapon->GetWeaponID()) &&
					g_pAimbot->_wpnCfg->aimbot(BaseWeapon->GetWeaponID()) &&
					BaseWeapon->GetClip1() > 0)
				{
					DWORD state = -1;
					if (shouldWrite)
						state = g_pAimbot->Aim(viewAnglesCopy, aimAngles, lastAimTime, shouldWrite, bAutoShoot, BaseWeapon->GetWeaponID(), startTime);
					else
					{
						if (lastAimTime + aimLockTime/100 >= flServerTime)
							state = g_pAimbot->Aim(viewAnglesCopy, aimAngles, lastAimTime, shouldWrite, bAutoShoot, BaseWeapon->GetWeaponID(), startTime);
					}

					if (state == 1)
					{
						bAimbot = true;
						shouldWrite = false;

						if (BaseWeapon->NextPrimaryAttack() <= flServerTime)
						{
							Vector fixxedRecoil = aimAngles;
							if (g_pAimbot->NoRecoil())
							{
								Helper::GetRecoilFix(g_pFactory, fixxedRecoil, fixxedRecoil);
								fixxedRecoil = Vector::ClampAngle(fixxedRecoil);
							}

							if (startTime != -1)
							{
								float deltaTime = curTime - startTime;
								float percentage = deltaTime / g_pAimbot->_wpnCfg->smooth(BaseWeapon->GetWeaponID());

								if (percentage < 0)
									percentage = 0;

								if (percentage > 1)
									percentage = 1;

								Vector delta(Vector::ClampAngle(fixxedRecoil - viewAnglesCopy));
								delta = delta * percentage;
								fixxedRecoil = Vector::ClampAngle(viewAnglesCopy + delta);
							}

							cmd->viewangles = fixxedRecoil;
							g_pFactory->m_pEngine->SetViewAngles(fixxedRecoil);

							if (g_pAimbot->Autoshoot())
								if (!bAutoShoot)
									cmd->buttons &= ~IN_ATTACK;
						}
					}
					if (state == 0)
					{
						cmd->buttons &= ~IN_ATTACK;
						shouldWrite = false;
					}
				}
			}
			else
				shouldWrite = true;
		}
	}

	//Rage
	{
		Vector aimAngles;

		if (g_pAimbot->Enabled() && !g_pAimbot->Legit())
		{
			if (cmd->buttons & IN_ATTACK)
			{
				if (!CBaseWeapon::IsWeaponNonAim(BaseWeapon->GetWeaponID()) &&
					g_pAimbot->_wpnCfg->aimbot(BaseWeapon->GetWeaponID()) &&
					BaseWeapon->GetClip1() > 0)
				{
					if (g_pAimbot->AimSilent(viewAnglesCopy, aimAngles, BaseWeapon->GetWeaponID()))
					{
						bAimbot = true;
						if (BaseWeapon->NextPrimaryAttack() <= flServerTime)
						{
							cmd->viewangles = aimAngles;

							if (g_pAimbot->SilentAim())
								bSendPacket = false;

							if (g_pAimbot->NoRecoil())
							{
								Vector fixxedRecoil;
								Helper::GetRecoilFix(g_pFactory, aimAngles, fixxedRecoil);
								fixxedRecoil = Vector::ClampAngle(fixxedRecoil);
								cmd->viewangles = fixxedRecoil;
							}
						}
					}
				}
			}
		}
	}

	//No Recoil
	{
		if (g_pMISC->recoilXHair())
		{
			Vector punch = ME->GetPunchAngles();
			int w, h;
			g_pFactory->m_pEngine->GetScreenSize(w, h);

			if (punch.x == 0.0 && punch.y == 0.0)
			{
				g_pMISC->setrecoilXHairPos(Vector(w / 2.f, h / 2.f, 0));
			}
			else
			{
				int x = w / 2;
				int y = h / 2;
				int dy = h / 90;
				int dx = w / 90;
				x -= dx*punch.y;
				y += dy*punch.x;
				g_pMISC->setrecoilXHairPos(Vector(x, y, 0));
			}
		}


		if (g_pMISC->recoilControl())
		{
			static Vector oldAngles = cmd->viewangles;
			static bool firstShot = true;

			if (cmd->buttons & IN_ATTACK)
			{

				if (!CBaseWeapon::IsWeaponNonAim(BaseWeapon->GetWeaponID()) &&
					g_pMISC->_wpnCfg->rcs(BaseWeapon->GetWeaponID()) &&
					BaseWeapon->GetClip1() > 0) {

					Vector fixxed;

					if (firstShot)
					{
						fixxed = cmd->viewangles;
						firstShot = false;
					}
					else
					{
						Vector delta = ME->GetPunchAngles() * 2.0f - oldAngles;
						delta.x /= g_pMISC->_wpnCfg->rcs_smooth(BaseWeapon->GetWeaponID());
						delta.y /= g_pMISC->_wpnCfg->rcs_smooth(BaseWeapon->GetWeaponID());

						fixxed = Vector::ClampAngle(cmd->viewangles - delta);
					}

					oldAngles = ME->GetPunchAngles() * 2.0f;

					if (!bAimbot)
						g_pFactory->m_pEngine->SetViewAngles(fixxed);
				}
			}
			else
			{
				oldAngles = cmd->viewangles;
				firstShot = true;
			}
		}
	}

	//dont matter when it overflows
	bool bLastTickAttack = false;
	static unsigned int state = 0;
	//Spinbot
	{
		if (g_pMISC->Spinbot())
		{
			if ((!(cmd->buttons & IN_ATTACK) || (cmd->buttons & IN_ATTACK && BaseWeapon->NextPrimaryAttack() > flServerTime &&
				!CBaseWeapon::IsWeaponNonAim(BaseWeapon->GetWeaponID())))
				&& !(cmd->buttons & IN_USE))
			{
				Vector spinAngles = viewAnglesCopy;

				spinAngles.y += 45 * (state % 8);
				cmd->viewangles = Vector::ClampAngle(spinAngles);

				g_pMISC->GetMovementFix(state % 8, oForwardMove, oSideMove, cmd);

				state++;
			}
		}
	}

	//Anti Aim
	{
		static bool bSwitch = false;
		Vector antiAimAngles = viewAnglesCopy;

		if (g_pMISC->AntiAim())
		{
			if (g_pMISC->Spinbot())
			{
				if (bLastTickAttack)
				{
					if (bSwitch)
					{
						if ((!(cmd->buttons & IN_ATTACK) || (cmd->buttons & IN_ATTACK && BaseWeapon->NextPrimaryAttack() > flServerTime &&
							!CBaseWeapon::IsWeaponNonAim(BaseWeapon->GetWeaponID())))
							&& !(cmd->buttons & IN_USE))
						{
							antiAimAngles.y += 90;
							cmd->viewangles = Vector::ClampAngle(antiAimAngles);
							cmd->forwardmove = -oSideMove;
							cmd->sidemove = oForwardMove;
							bSendPacket = false;
							if (g_pMISC->FakeDuck())
								if (cmd->buttons & IN_DUCK)
									cmd->buttons &= ~IN_DUCK;
								else
									cmd->buttons |= IN_DUCK;

							bSwitch = false;
						}
					}
					else
						bSwitch = true;
				}
				else
				{
					if (state >= 2)
					{
						if (bSwitch)
						{
							/*
							unsigned int serverState = (state - 2) % 8;
							antiAimAngles.y += 45 * serverState;
							antiAimAngles.y += 90;
							*/

							if ((!(cmd->buttons & IN_ATTACK) || (cmd->buttons & IN_ATTACK && BaseWeapon->NextPrimaryAttack() > flServerTime &&
								!CBaseWeapon::IsWeaponNonAim(BaseWeapon->GetWeaponID())))
								&& !(cmd->buttons & IN_USE))
							{
								antiAimAngles.y += 45 * (state % 8);
								g_pMISC->GetMovementFix(state % 8, oForwardMove, oSideMove, &cmdCopy);
								cmd->forwardmove = cmdCopy.forwardmove;
								cmd->sidemove = cmdCopy.sidemove;

								//make sure next spinbot packet is changed in 90°
								state += 2;

								cmd->viewangles = Vector::ClampAngle(antiAimAngles);
								bSendPacket = false;
								if (g_pMISC->FakeDuck())
									if (cmd->buttons & IN_DUCK)
										cmd->buttons &= ~IN_DUCK;
									else
										cmd->buttons |= IN_DUCK;

								bSwitch = false;
							}
						}
						else
							bSwitch = true;
					}
				}
			}
			else
			{
				if (bSwitch)
				{
					if ((!(cmd->buttons & IN_ATTACK) || (cmd->buttons & IN_ATTACK && BaseWeapon->NextPrimaryAttack() > flServerTime &&
						!CBaseWeapon::IsWeaponNonAim(BaseWeapon->GetWeaponID())))
						&& !(cmd->buttons & IN_USE))
					{
						antiAimAngles.y += 90;
						cmd->viewangles = Vector::ClampAngle(antiAimAngles);
						cmd->forwardmove = -oSideMove;
						cmd->sidemove = oForwardMove;

						bSendPacket = false;
						if (g_pMISC->FakeDuck())
							if (cmd->buttons & IN_DUCK)
								cmd->buttons &= ~IN_DUCK;
							else
								cmd->buttons |= IN_DUCK;

						bSwitch = false;
					}
				}
				else
					bSwitch = true;
			}

			if (cmd->buttons & IN_ATTACK)
				bLastTickAttack = true;
			else
				bLastTickAttack = false;
		}
	}

	//Lagg Switch
	{
		static int chokedPackets = 0;
		static int sentPackets = 0;

		if (g_pMISC->LaggSwitch())
		{
			if (chokedPackets < 10)
			{
				bSendPacket = false;
				chokedPackets++;
			}
			else
			{
				sentPackets++;
				if (sentPackets >= 10)
				{
					sentPackets = 0;
					chokedPackets = 0;
				}
			}
		}
	}

	//Radar
	{
		if (g_pMISC->radarEnabled())
			g_pMISC->Radar();
	}

	//BunnyHop
	{
		if (g_pMISC->BunnyHop())
		{
			if (ME->IsAlive() && cmd->buttons & IN_JUMP)
			{
				if (ME->GetFlags() & FL_ON_GROUND)
					cmd->buttons |= IN_JUMP;
				else
					cmd->buttons &= ~IN_JUMP;
			}
		}
	}

	//autopistol
	{
		if (g_pMISC->AutoPistol())
		{
			if (cmd->buttons & IN_ATTACK && ME->IsAlive()
				&& CBaseWeapon::IsWeaponPistol(BaseWeapon->GetWeaponID()) && BaseWeapon->GetClip1() > 0)
			{
				if (BaseWeapon->NextPrimaryAttack() <= flServerTime)
					cmd->buttons |= IN_ATTACK;
				else
					cmd->buttons &= ~IN_ATTACK;
			}
		}
	}

	verified_cmd[sequence_number % 150].m_cmd = *cmd;
	verified_cmd[sequence_number % 150].m_crc = cmd->GetChecksum();
}

HRESULT __stdcall hk_Reset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	if (g_pRender->IsCreated())
		g_pRender->Release();

	HRESULT ret = oReset(pDevice, pPresentationParameters);

	if (ret == D3D_OK && !g_pRender->IsCreated())
		g_pRender->Create(pDevice);

	return ret;
}

HRESULT __stdcall hk_EndScene(LPDIRECT3DDEVICE9 pDevice)
{
	if (!g_pRender->IsCreated())
		g_pRender->Create(pDevice);

	if (!g_pMenu->IsCreated())
	{
		g_pMenu->Create(g_pRender);
		InitMenuItems();
	}

	if (!g_pRankBoard->IsCreated())
		g_pRankBoard->Create(g_pFactory, g_pRender);

	g_pRender->Begin();

	if (g_pMISC->Crosshair() && g_pFactory->m_pEngine->IsInGame() && g_pFactory->m_pEngine->IsConnected())
	{
		int w, h;
		g_pFactory->m_pEngine->GetScreenSize(w, h);

		if (g_pMISC->getxhairtype() == MISC::CROSSHAIR::eCSGO)
			g_pRender->DrawCrosshairCSGO(w / 2., h / 2.);
		else if (g_pMISC->getxhairtype() == MISC::CROSSHAIR::eCustom)
			g_pRender->DrawCrosshairRedWhite(w / 2., h / 2.);
		else if (g_pMISC->getxhairtype() == MISC::CROSSHAIR::eOlofMeister)
			g_pRender->DrawCrosshairOlofMeister(w / 2., h / 2.);
	}

	if (g_pMISC->recoilXHair() && g_pFactory->m_pEngine->IsInGame() && g_pFactory->m_pEngine->IsConnected())
	{
		Vector v = g_pMISC->recoilXHairPos();

		if (g_pMISC->getxhairtype() == MISC::CROSSHAIR::eCSGO)
			g_pRender->DrawCrosshairCSGO(v.x, v.y);
		else if (g_pMISC->getxhairtype() == MISC::CROSSHAIR::eCustom)
			g_pRender->DrawCrosshairRedWhite(v.x, v.y);
		else if (g_pMISC->getxhairtype() == MISC::CROSSHAIR::eOlofMeister)
			g_pRender->DrawCrosshairOlofMeister(v.x, v.y);
	}

	if (GetAsyncKeyState(VK_HOME) & 0x8000)
	{
		if (g_pRankBoard->GrabData())
		{
			int w, h;
			g_pFactory->m_pEngine->GetScreenSize(w, h);
			int k, l;
			g_pRankBoard->GetDimensions(k, l);
			g_pRankBoard->Render(w / 2. - k / 2., h / 2. - l / 2.);
		}
	}

	g_pMenu->RenderGUI();

	g_pRender->End();

	UpdateStatus();

	static bool MouseEnabled = false;
	if (g_pMenu->MenuOpen() && !MouseEnabled)
	{
		g_pFactory->m_pEngine->ClientCmd("cl_mouseenable 0");
		MouseEnabled = true;
	}

	if (!g_pMenu->MenuOpen() && MouseEnabled)
	{
		g_pFactory->m_pEngine->ClientCmd("cl_mouseenable 1");
		MouseEnabled = false;
	}

	return oEndScene(pDevice);
}

void UpdateStatus()
{
	g_pAimbot->Update();
	g_pESP->Update();
	g_pMISC->Update();
	g_pTriggerbot->Update();
}

void InitMenuItems()
{

	CMenuTab* esp = new CMenuTab("ESP", 40, 100, g_pRender);
	esp->AddComboxBox("Enabled");
	esp->AddComboxBox("Draw Enemy Only");
	esp->AddComboxBox("Box ESP");
	esp->AddComboxBox("Health ESP");
	esp->AddComboxBox("Name ESP");
	esp->AddComboxBox("Weapon ESP");
	esp->AddComboxBox("Entity ESP");
	esp->AddComboxBox("Skeleton ESP");
	esp->AddComboxBox("Distance ESP");
	esp->AddComboxBox("Draw Bone IDs");
	esp->AddComboxBox("Hitbox ESP");

	std::vector<CMenuItem*> EspItems = esp->GetMenuItems();
	for (std::vector<CMenuItem*>::iterator l = EspItems.begin(); l != EspItems.end(); ++l)
	{
		g_pESP->addObserver(*l);
	}

	CMenuTab* aimbot = new CMenuTab("Aimbot", 220, 100, g_pRender);
	CMenuTabControl* tabCtrl = new CMenuTabControl(aimbot, 0, 0, g_pRender);
	CMenuTabControlItem* tab1 = new CMenuTabControlItem("Settings", 0, 0, tabCtrl, g_pRender);
	CMenuTabControlItem* tab2 = new CMenuTabControlItem("Bone", 0, 0, tabCtrl, g_pRender);

	tab1->AddComboxBox("Enabled");
	tab1->AddComboxBox("Aim At Friendly");
	tab1->AddComboxBox("No Recoil View");
	tab1->AddComboxBox("PSilent");

	CMenuDropDownList* ModeSelection = new CMenuDropDownList("Mode", tab1, 0, 0, g_pRender);
	ModeSelection->AddListItem("FOV");
	ModeSelection->AddListItem("Real Dist");
	tab1->AddMenuItem(ModeSelection);
	tab1->AddComboxBox("Legit");
	tab1->AddComboxBox("Non Sticky");
	tab1->AddSlider("Reaction Time", 0, 100, 250, Datatype_t::I_INT);
	tab1->AddSlider("Lock Time", 0, 500, 250, Datatype_t::I_INT);
	tab1->AddComboxBox("Shoot players in air");
	tab1->AddComboxBox("Autoshoot");

	CBoneSelector* boneSelect = new CBoneSelector("Bone", tab2, 0, 0, g_pRender);
	boneSelect->AllowMultipleSelections();

	boneSelect->AddHead(Triggerbot::HitboxList::HITBOX_HEAD);
	boneSelect->AddNeck(Triggerbot::HitboxList::HITBOX_NECK);

	boneSelect->AddPelvis(Triggerbot::HitboxList::HITBOX_SPINE1);
	boneSelect->AddTorso1(Triggerbot::HitboxList::HITBOX_SPINE2);
	boneSelect->AddTorso2(Triggerbot::HitboxList::HITBOX_SPINE3);
	boneSelect->AddTorso3(Triggerbot::HitboxList::HITBOX_SPINE4);

	boneSelect->AddLeftArm1(Triggerbot::HitboxList::HITBOX_L_UPPERARM);
	boneSelect->AddLeftArm2(Triggerbot::HitboxList::HITBOX_L_FOREARM);
	boneSelect->AddLeftArm3(Triggerbot::HitboxList::HITBOX_L_HAND);

	boneSelect->AddRightArm1(Triggerbot::HitboxList::HITBOX_R_UPPERARM);
	boneSelect->AddRightArm2(Triggerbot::HitboxList::HITBOX_R_FOREARM);
	boneSelect->AddRightArm3(Triggerbot::HitboxList::HITBOX_R_HAND);

	boneSelect->AddLeftLeg1(Triggerbot::HitboxList::HITBOX_L_THIGH);
	boneSelect->AddLeftLeg2(Triggerbot::HitboxList::HITBOX_L_CALF);
	boneSelect->AddLeftFoot1(Triggerbot::HitboxList::HITBOX_L_FOOT);

	boneSelect->AddRightLeg1(Triggerbot::HitboxList::HITBOX_R_THIGH);
	boneSelect->AddRightLeg2(Triggerbot::HitboxList::HITBOX_R_CALF);
	boneSelect->AddRightFoot1(Triggerbot::HitboxList::HITBOX_R_FOOT);

	tab2->AddMenuItem(boneSelect);

	tabCtrl->AddTab(tab1);
	tabCtrl->AddTab(tab2);
	aimbot->AddMenuItem(tabCtrl);

	std::vector<CMenuItem*> tab1Items = tab1->GetMenuItems();
	for (std::vector<CMenuItem*>::iterator l = tab1Items.begin(); l != tab1Items.end(); ++l)
	{
		g_pAimbot->addObserver(*l);
	}
	g_pAimbot->addObserver(boneSelect);

	CMenuTab* misc = new CMenuTab("MISC", 920, 100, g_pRender);
	misc->AddComboxBox("RCS");
	misc->AddComboxBox("Recoil XHair");

	CMenuDropDownList* WallhackSelection = new CMenuDropDownList("Wallhack", misc, 0, 0, g_pRender);
	WallhackSelection->AddListItem("None");
	WallhackSelection->AddListItem("Color Models");
	WallhackSelection->AddListItem("Chams");
	misc->AddMenuItem(WallhackSelection);

	misc->AddComboxBox("Autopistol");
	misc->AddComboxBox("XHair");

	CMenuDropDownList* XHairSelection = new CMenuDropDownList("XHair Type", misc, 0, 0, g_pRender);
	XHairSelection->AddListItem("Custom");
	XHairSelection->AddListItem("Big");
	XHairSelection->AddListItem("Small");
	misc->AddMenuItem(XHairSelection);

	misc->AddComboxBox("Radar");
	misc->AddComboxBox("Bunnyhop");
	misc->AddComboxBox("Spinbot");
	misc->AddComboxBox("Anti Aim");
	misc->AddComboxBox("Fake Duck");
	misc->AddComboxBox("Lagswitch");

	std::vector<CMenuItem*> MISCItems = misc->GetMenuItems();
	for (std::vector<CMenuItem*>::iterator l = MISCItems.begin(); l != MISCItems.end(); ++l)
	{
		g_pMISC->addObserver(*l);
	}

	CMenuTab* TriggerBot = new CMenuTab("Triggerbot", 600, 400, g_pRender);
	TriggerBot->AddComboxBox("Enabled");
	TriggerBot->AddComboxBox("No Recoil View");
	TriggerBot->AddSlider("Delay", 0, 1000, 100, Datatype_t::I_INT);
	TriggerBot->AddSlider("Burstshots", 0, 5, 100, Datatype_t::I_INT);

	CBoneSelector* hitboxSelection = new CBoneSelector("Hitboxes", TriggerBot, 0, 0, g_pRender);
	hitboxSelection->AllowMultipleSelections();

	hitboxSelection->AddHead(Triggerbot::HitboxList::HITBOX_HEAD);
	hitboxSelection->AddNeck(Triggerbot::HitboxList::HITBOX_NECK);

	hitboxSelection->AddPelvis(Triggerbot::HitboxList::HITBOX_SPINE1);
	hitboxSelection->AddTorso1(Triggerbot::HitboxList::HITBOX_SPINE2);
	hitboxSelection->AddTorso2(Triggerbot::HitboxList::HITBOX_SPINE3);
	hitboxSelection->AddTorso3(Triggerbot::HitboxList::HITBOX_SPINE4);

	hitboxSelection->AddLeftArm1(Triggerbot::HitboxList::HITBOX_L_UPPERARM);
	hitboxSelection->AddLeftArm2(Triggerbot::HitboxList::HITBOX_L_FOREARM);
	hitboxSelection->AddLeftArm3(Triggerbot::HitboxList::HITBOX_L_HAND);

	hitboxSelection->AddRightArm1(Triggerbot::HitboxList::HITBOX_R_UPPERARM);
	hitboxSelection->AddRightArm2(Triggerbot::HitboxList::HITBOX_R_FOREARM);
	hitboxSelection->AddRightArm3(Triggerbot::HitboxList::HITBOX_R_HAND);

	hitboxSelection->AddLeftLeg1(Triggerbot::HitboxList::HITBOX_L_THIGH);
	hitboxSelection->AddLeftLeg2(Triggerbot::HitboxList::HITBOX_L_CALF);
	hitboxSelection->AddLeftFoot1(Triggerbot::HitboxList::HITBOX_L_FOOT);

	hitboxSelection->AddRightLeg1(Triggerbot::HitboxList::HITBOX_R_THIGH);
	hitboxSelection->AddRightLeg2(Triggerbot::HitboxList::HITBOX_R_CALF);
	hitboxSelection->AddRightFoot1(Triggerbot::HitboxList::HITBOX_R_FOOT);

	TriggerBot->AddMenuItem(hitboxSelection);

	std::vector<CMenuItem*> TriggerbotItems = TriggerBot->GetMenuItems();
	for (std::vector<CMenuItem*>::iterator l = TriggerbotItems.begin(); l != TriggerbotItems.end(); ++l)
	{
		g_pTriggerbot->addObserver(*l);
	}

	Console* pConsole = new Console(800, 400, 350, 300, g_pRender);

	/*
	Weapon Config
	*/
	std::vector<std::string> weapons{
		"None", "Deagle", "Elite", "Fiveseven", "Glock", "P228", "USP", "AK47",
		"AUG", "AWP", "Famas", "G3SG1", "Galil", "Galilar", "M249", "M3", "M4A1",
		"Mac10", "MP5Navy", "P90", "Scout", "SG550", "SG552", "TMP", "UMP45",
		"XM1014", "Bizon", "Mag7", "Negev", "Sawedoff", "Tec9", "Taser", "HKP2000",
		"MP7", "MP9", "Nova", "P250", "Scar17", "Scar20", "SG556", "SSG08", "Knifeegg",
		"Knife", "Flash", "He", "Smoke", "Molotov", "Decoy", "Incgrenade", "C4"
	};

	for (int i = 0; i < weapons.size(); i++)
	{
		pConsole->RegisterBool(weapons[i] + ".rcs", "Enables rcs on this weapon", g_pWeaponConfig,
			WeaponConfig::toIndex(i, 0));
		pConsole->RegisterBool(weapons[i] + ".triggerbot", "Enables triggerbot on this weapon", g_pWeaponConfig,
			WeaponConfig::toIndex(i, 1));
		pConsole->RegisterBool(weapons[i] + ".aimbot", "Enables aimbot on this weapon", g_pWeaponConfig,
			WeaponConfig::toIndex(i, 2));
		pConsole->RegisterFloat(weapons[i] + ".fov", "Set the fov for the aimbot on this weapon", 0, 360, g_pWeaponConfig,
			WeaponConfig::toIndex(i, 3));
		pConsole->RegisterFloat(weapons[i] + ".smooth", "Sets the smooth of the aimbot for this weapon", 1, 5000, g_pWeaponConfig,
			WeaponConfig::toIndex(i, 4));
		pConsole->RegisterFloat(weapons[i] + ".rcssmooth", "Sets the smooth of the rcs for this weapon", 1, 50, g_pWeaponConfig,
			WeaponConfig::toIndex(i, 5));
		pConsole->RegisterInt(weapons[i] + ".minAimDist", "Sets the minimum distance at which the aimbot will activate for this weapon", 0, 5000, g_pWeaponConfig,
			WeaponConfig::toIndex(i, 6));
	}

	g_pMenu->AddTab(esp);
	g_pMenu->AddTab(aimbot);
	g_pMenu->AddTab(misc);
	g_pMenu->AddTab(TriggerBot);
	g_pMenu->AddTab(pConsole);

	std::string tempPath;
	tempPath.resize(300);
	GetTempPath(300, (char*)tempPath.c_str());

	CMenuConfigMgr* configMgr = new CMenuConfigMgr(g_pMenu, g_pRender, tempPath.c_str());
	configMgr->CreateConfigTab(1004, 570);

	CMenuTab::SetTabItems(g_pMenu->GetTabs());
}

void* traverse_stack(void** ebp)
{
	if (ebp == nullptr)
		return nullptr;

	auto **next = *(void***)(ebp);
	if (ebp[1] == (void*)g_pFactory->draw_points_ret_addr)
		return next[4];

	return traverse_stack(next);
}

//for chams
HRESULT __stdcall hk_DrawIndexedPrimitive(IDirect3DDevice9* device, D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount)
{
	if (!IsPixelShaderCreated)
	{
		Utils::GenerateShader(device, &green, 0.0f, 1.0f, 0.0f);
		Utils::GenerateShader(device, &blue, 0.0f, 0.0f, 1.0f);
		Utils::GenerateShader(device, &red, 1.0f, 0.0f, 0.0f);
		Utils::GenerateShader(device, &yellow, 1.0f, 1.0f, 0.0f);
		IsPixelShaderCreated = true;
	}

	if (g_pMISC->getWallhackType() == MISC::WALLHACK::eColoredModels)
	{
		HRESULT ret;
		DWORD dwebp = 0;
		_asm MOV dwebp, EBP;
		auto pEnt = (IClientEntity*)traverse_stack((void**)dwebp);
		if (pEnt)
		{
			auto team = pEnt->GetTeamNum();
			if (team != 2 && team != 3)
				return oDrawIndexedPrimitive(device, Type, BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);

			IDirect3DPixelShader9 *shader;
			device->GetPixelShader(&shader);

			if (team == 2) //T
			{
				device->SetRenderState(D3DRS_ZENABLE, true);
				device->SetPixelShader(red);
				ret = oDrawIndexedPrimitive(device, Type, BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
			}
			if (team == 3) //CT
			{
				device->SetRenderState(D3DRS_ZENABLE, true);
				device->SetPixelShader(blue);
				ret = oDrawIndexedPrimitive(device, Type, BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
			}

			device->SetPixelShader(shader);
			return ret;
		}
	}

	if (g_pMISC->getWallhackType() == MISC::WALLHACK::eChams)
	{
		HRESULT ret;
		DWORD dwebp = 0;
		_asm MOV dwebp, EBP;
		auto pEnt = (IClientEntity*)traverse_stack((void**)dwebp);
		if (pEnt)
		{
			auto team = pEnt->GetTeamNum();
			if (team != 2 && team != 3)
				return oDrawIndexedPrimitive(device, Type, BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);

			IDirect3DPixelShader9 *shader;
			device->GetPixelShader(&shader);

			if (team == 2) //T
			{
				device->SetRenderState(D3DRS_ZENABLE, false);
				device->SetPixelShader(yellow);
				oDrawIndexedPrimitive(device, Type, BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
				device->SetRenderState(D3DRS_ZENABLE, true);
				device->SetPixelShader(red);
				ret = oDrawIndexedPrimitive(device, Type, BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
			}
			if (team == 3) //CT
			{
				device->SetRenderState(D3DRS_ZENABLE, false);
				device->SetPixelShader(green);
				oDrawIndexedPrimitive(device, Type, BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
				device->SetRenderState(D3DRS_ZENABLE, true);
				device->SetPixelShader(blue);
				ret = oDrawIndexedPrimitive(device, Type, BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
			}

			device->SetPixelShader(shader);
			return ret;
		}
	}

	return oDrawIndexedPrimitive(device, Type, BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
}