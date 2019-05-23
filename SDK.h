#pragma once
#include "IBaseClientDLL.h"
#include "CInput.h"
#include "IClientEntityList.h"
#include "IEngineTrace.h"
#include "IVEngineClient.h"
#include "CBaseWeapon.h"
#include "CVerifiedUserCmd.h"
#include "trace_t.h"
#include "ray_t.h"
#include "CGlobalVars.h"
#include "ISurface.h"
#include "IPanel.h"
#include "ITraceFilter.h"
#include "IModelInfo.h"
#include "IDebugOverlay.h"

#define WIN32_LEAN_AND_MEAN

#define IN_ATTACK					(1 << 0)
#define FL_ON_GROUND				(1 << 0)

#define IN_JUMP						(1 << 1)
#define FL_DUCKING					(1 << 1)

#define IN_DUCK						(1 << 2)
#define IN_FORWARD					(1 << 3)
#define IN_BACK						(1 << 4)
#define IN_USE						(1 << 5)
#define IN_CANCEL					(1 << 6)
#define IN_LEFT						(1 << 7)
#define IN_RIGHT					(1 << 8)
#define IN_MOVELEFT					(1 << 9)
#define IN_MOVERIGHT				(1 << 10)
#define IN_ATTACK2					(1 << 11)
#define IN_RUN						(1 << 12)
#define IN_RELOAD					(1 << 13)
#define IN_ALT1						(1 << 14)
#define IN_ALT2						(1 << 15)
#define IN_SCORE					(1 << 16)
#define IN_SPEED					(1 << 17)
#define IN_WALK						(1 << 18)
#define IN_ZOOM						(1 << 19)
#define IN_WEAPON1					(1 << 20)
#define IN_WEAPON2					(1 << 21)
#define IN_BULLRUSH					(1 << 22)