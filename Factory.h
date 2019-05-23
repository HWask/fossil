#pragma once
#include "SDK.h"
#include <time.h>
#include <cstdio>

class Factory
{
public:
	typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);

	DWORD draw_points_ret_addr;
	IVEngineClient*	m_pEngine;//engine
	IEngineTrace*	m_pEnginetrace;//engine
	IBaseClientDLL*	m_pClient; //client
	IClientEntityList*	m_pEntList;//client
	CInput*	m_pInput;
	ISurface* m_pSurface;
	IPanel* m_pPanel;
	CGlobalVars* m_pGlobalVars;
	IModelInfo* m_pModelInfo;
	IDebugOverlay* m_pDebugOverlay;

	DWORD clientModule;
	DWORD engineModule;
	DWORD vguimatsurface;
	DWORD vgui2;
public:
	Factory()
	{
		m_pClient			= NULL;
		m_pEntList			= NULL;
		m_pEngine			= NULL;
		m_pEnginetrace		= NULL;
		m_pInput			= NULL;
		clientModule		= NULL;
		engineModule		= NULL;
	}

	void GetGlobalVars()
	{
		DWORD dwInit = ((DWORD*)*(DWORD*)m_pClient)[0];
#ifdef _DEBUG
		Logger("dwInit: 0x%X", dwInit);
#endif
		for (DWORD dwIter = 2; dwIter <= 0xFF; dwIter++)
		{
			if (*(PBYTE)(dwInit + dwIter - 2) == 0x4D && *(PBYTE)(dwInit + dwIter - 1) == 0x08 && *(PBYTE)(dwInit + dwIter) == 0xA3)
			{
				m_pGlobalVars = (CGlobalVars*)*(PDWORD)*(PDWORD)(dwInit + dwIter + 1);
				break;
			}
		}

#ifdef _DEBUG
		Logger("m_pGlobalVars: 0x%X", m_pGlobalVars);
#endif
	}

	void GetDrawPointsReturn()
	{
		DWORD studiorender = (DWORD)GetModuleHandle("studiorender.dll");
		draw_points_ret_addr = Utils::FindPattern(studiorender, 0xFFFFFFFF, (BYTE*)"\x8B\x7C\x24\x10\x03\xF8", "xxxxxx");

#ifdef _DEBUG
		Logger("draw_points_ret_addr: 0x%X", draw_points_ret_addr);
#endif
	}

	void GetCInput()
	{
		DWORD dwCreateMove = ((DWORD*)*(DWORD*)m_pClient)[21];
		m_pInput = (CInput*)*(DWORD*)(dwCreateMove + 0x62);

#ifdef _DEBUG
		Logger("dwCreateMove: 0x%X", dwCreateMove);
		Logger("m_pInput: 0x%X", m_pInput);
#endif
	}

	void GetInterfaces()
	{
		// wait for game to init
		while (!GetModuleHandle("engine.dll")) {}

		HMODULE hEngine;
		HMODULE hClient;
		HMODULE hMaterialSystem;
		HMODULE hvgui;
		HMODULE hvgui2;

		hEngine = GetModuleHandle("engine.dll");
		engineModule = (DWORD)hEngine;

		CreateInterfaceFn EngineFactory = (CreateInterfaceFn)GetProcAddress(hEngine, "CreateInterface");

		m_pEngine = (IVEngineClient*)EngineFactory("VEngineClient013", NULL);
		m_pEnginetrace = (IEngineTrace*)EngineFactory("EngineTraceClient004", NULL);
		m_pModelInfo = (IModelInfo*)EngineFactory("VModelInfoClient004", NULL);
		m_pDebugOverlay = (IDebugOverlay*)EngineFactory("VDebugOverlay004", NULL);

#ifdef _DEBUG
		Logger("m_pEngine: 0x%X", m_pEngine);
		Logger("m_pEnginetrace: 0x%X", m_pEnginetrace);
		Logger("m_pModelInfo: 0x%X", m_pModelInfo);
		Logger("m_pDebugOverlay: 0x%X", m_pDebugOverlay);
#endif
		hClient = GetModuleHandle("client.dll");
		clientModule = (DWORD)hClient;

		CreateInterfaceFn ClientFactory = (CreateInterfaceFn)GetProcAddress(hClient, "CreateInterface");

		m_pClient = (IBaseClientDLL*)ClientFactory("VClient017", NULL);
		m_pEntList = (IClientEntityList*)ClientFactory("VClientEntityList003", NULL);

#ifdef _DEBUG
		Logger("m_pClient: 0x%X", m_pClient);
		Logger("m_pEntList: 0x%X", m_pEntList);
#endif

		hvgui = GetModuleHandle("vguimatsurface.dll");
		vguimatsurface = (DWORD)hvgui;

		CreateInterfaceFn MatSurfaceFactory = (CreateInterfaceFn)GetProcAddress(hvgui, "CreateInterface");

		m_pSurface = (ISurface*)MatSurfaceFactory("VGUI_Surface031", NULL);

#ifdef _DEBUG
		Logger("m_pSurface: 0x%X", m_pSurface);
#endif

		hvgui2 = GetModuleHandle("vgui2.dll");
		vgui2 = (DWORD)hvgui2;

		CreateInterfaceFn VGUIFactory = (CreateInterfaceFn)GetProcAddress(hvgui2, "CreateInterface");

		m_pPanel = (IPanel*)VGUIFactory("VGUI_Panel009", NULL);

#ifdef _DEBUG
		Logger("m_pPanel: 0x%X", m_pPanel);
#endif
	}
#ifdef _DEBUG
	void static Logger(char* format, ...) {}
#else
	void static Logger(char* format, ...) {}
#endif
};