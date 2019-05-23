#pragma once

#include <Windows.h>

#include "Factory.h"
#include "CTraceFilter.h"

class Helper
{
public:
	static bool GetRecoilFix(Factory* pFactory, Vector& viewangles, Vector& fixxed)
	{
		int local = pFactory->m_pEngine->GetLocalPlayer();
		auto ME = pFactory->m_pEntList->GetClientEntity(local);
		if (!ME)
			return false;

		fixxed = viewangles - (ME->GetPunchAngles() * 2.0f);

		return true;
	}

	static bool AddPunch(Factory* pFactory, Vector& viewangles, Vector& fixxed)
	{
		int local = pFactory->m_pEngine->GetLocalPlayer();
		auto ME = pFactory->m_pEntList->GetClientEntity(local);
		if (!ME)
			return false;

		fixxed = viewangles + (ME->GetPunchAngles() * 2.0f);

		return true;
	}

	static bool InterpolateEyePos(Factory* pFactory, Vector& eyePos)
	{
		int local = pFactory->m_pEngine->GetLocalPlayer();
		auto ME = pFactory->m_pEntList->GetClientEntity(local);
		if (!ME)
			return false;

		eyePos = ME->GetEyePosition() + (ME->GetVelocity() * pFactory->m_pGlobalVars->flIntervalsPerTick);

		return true;
	}

	static bool GetBoneVector(Factory* pFactory, IClientEntity* ent, int Bone, Vector& vecBone)
	{
		matrix3x4_t bones[128];

		if (!ent->SetupBones(bones, 128, 0x00000100, pFactory->m_pEngine->GetLastTimeStamp()))
			return false;

		vecBone.x = bones[Bone].m[0][3];
		vecBone.y = bones[Bone].m[1][3];
		vecBone.z = bones[Bone].m[2][3];

		return true;
	}

	static bool GetBoneMatrix(Factory* pFactory, IClientEntity* ent, int Bone, matrix3x4_t& boneMat)
	{
		matrix3x4_t bones[128];

		if (!ent->SetupBones(bones, 128, 0x00000100, pFactory->m_pEngine->GetLastTimeStamp()))
			return false;

		boneMat = bones[Bone];

		return true;
	}

	static bool IsVisible(Factory* pFactory, IClientEntity* pLocal, IClientEntity* pEnd, Vector start, Vector end)
	{
		trace_t trace;
		ray_t ray;
		CTraceFilter filter(pLocal, pEnd);

		ray.Init(start, end);
		pFactory->m_pEnginetrace->TraceRay(ray, 0x4600400B, &filter, &trace);

		return trace.fraction > 0.97;
	}
};