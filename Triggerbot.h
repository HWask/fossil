#pragma once
#include "Factory.h"
#include "Helper.h"
#include "CWeaponConfig.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include "GUI\Observer.h"

#include <vector>

#define SMALL_NUM   0.00000001
#define dot(u,v)   ((u).x * (v).x + (u).y * (v).y + (u).z * (v).z)
#define norm(v)    sqrt(dot(v,v))
#define d(u,v)     norm(u-v)        
#define abs(x)     ((x) >= 0 ? (x) : -(x))   

class Triggerbot : public Observer
{
private:
	enum Cheats
	{
		eenabled,
		enorecoilview,
		edelay,
		eburstshots,
		ebones,
		MAX
	};

	bool bIsCreated = false;
	Factory* m_pFactory;

	std::vector<int> hitboxes;

	bool bEnabled = false;
	bool noRecoil = false;
	float flDelay = 1;
	int burstShots = 0;

	bool InsersectRayWithOBB(Vector& myPos, Vector& dir, matrix3x4_t& boneMat, Vector& bbmin, Vector& bbmax)
	{
		//Transform ray into model space of hitbox so we only have to deal with an AABB instead of OBB
		Vector ray_trans, dir_trans;
		Utils::ITransform(myPos, boneMat, ray_trans);
		Utils::IRotate(dir, boneMat, dir_trans);

		auto ret = IntersectRayWithAABB(ray_trans, dir_trans, bbmin, bbmax);

		return ret;
	}

	bool IntersectRayWithAABB(Vector& origin, Vector& dir, Vector& min, Vector& max)
	{
		float tmin, tmax, tymin, tymax, tzmin, tzmax;

		if (dir.x >= 0)
		{
			tmin = (min.x - origin.x) / dir.x;
			tmax = (max.x - origin.x) / dir.x;
		}
		else
		{
			tmin = (max.x - origin.x) / dir.x;
			tmax = (min.x - origin.x) / dir.x;
		}

		if (dir.y >= 0)
		{
			tymin = (min.y - origin.y) / dir.y;
			tymax = (max.y - origin.y) / dir.y;
		}
		else
		{
			tymin = (max.y - origin.y) / dir.y;
			tymax = (min.y - origin.y) / dir.y;
		}

		if (tmin > tymax || tymin > tmax)
			return false;

		if (tymin > tmin)
			tmin = tymin;

		if (tymax < tmax)
			tmax = tymax;

		if (dir.z >= 0)
		{
			tzmin = (min.z - origin.z) / dir.z;
			tzmax = (max.z - origin.z) / dir.z;
		}
		else
		{
			tzmin = (max.z - origin.z) / dir.z;
			tzmax = (min.z - origin.z) / dir.z;
		}

		if (tmin > tzmax || tzmin > tmax)
			return false;

		//behind us
		if (tmin < 0 || tmax < 0)
			return false;

		return true;
	}

	float dist_Segment_to_Segment(Vector s1, Vector s2, Vector k1, Vector k2)
	{
		Vector   u = s2 - s1;
		Vector   v = k2 - k1;
		Vector   w = s1 - k1;
		float    a = dot(u, u);
		float    b = dot(u, v);
		float    c = dot(v, v);
		float    d = dot(u, w);
		float    e = dot(v, w);
		float    D = a*c - b*b;  
		float    sc, sN, sD = D;       
		float    tc, tN, tD = D;  

		if (D < SMALL_NUM) { 
			sN = 0.0;        
			sD = 1.0;        
			tN = e;
			tD = c;
		}
		else {                
			sN = (b*e - c*d);
			tN = (a*e - b*d);
			if (sN < 0.0) {        
				sN = 0.0;
				tN = e;
				tD = c;
			}
			else if (sN > sD) {  
				sN = sD;
				tN = e + b;
				tD = c;
			}
		}

		if (tN < 0.0) {        
			tN = 0.0;

			if (-d < 0.0)
				sN = 0.0;
			else if (-d > a)
				sN = sD;
			else {
				sN = -d;
				sD = a;
			}
		}
		else if (tN > tD) {    
			tN = tD;

			if ((-d + b) < 0.0)
				sN = 0;
			else if ((-d + b) > a)
				sN = sD;
			else {
				sN = (-d + b);
				sD = a;
			}
		}

		sc = (abs(sN) < SMALL_NUM ? 0.0 : sN / sD);
		tc = (abs(tN) < SMALL_NUM ? 0.0 : tN / tD);

		Vector  dP = w + (u * sc) - (v * tc);

		return norm(dP);
	}

	bool DoesIntersectCapsule(Vector eyePos, Vector myDir, Vector capsuleA, Vector capsuleB, float radius)
	{
		Vector end = eyePos + (myDir * 8192);
		auto dist = dist_Segment_to_Segment(eyePos, end, capsuleA, capsuleB);
		
		return dist < radius;
	}
public:
	WeaponConfig* _wpnCfg;

	enum HitboxList
	{
		HITBOX_L_THIGH = 9,
		HITBOX_L_CALF = 11,
		HITBOX_L_FOOT = 13,

		HITBOX_R_THIGH = 8,
		HITBOX_R_CALF = 10,
		HITBOX_R_FOOT = 12,

		HITBOX_SPINE1 = 4,
		HITBOX_SPINE2 = 5,
		HITBOX_SPINE3 = 6,
		HITBOX_SPINE4 = 7,

		HITBOX_NECK = 2,
		HITBOX_HEAD = 0,

		HITBOX_L_UPPERARM = 18,
		HITBOX_L_FOREARM = 19,
		HITBOX_L_HAND = 15,

		HITBOX_R_UPPERARM = 16,
		HITBOX_R_FOREARM = 17,
		HITBOX_R_HAND = 14,
	};
public:
	Triggerbot(WeaponConfig* wpnCfg) : _wpnCfg(wpnCfg) {}

	bool Enabled()
	{
		return bEnabled;
	}

	void Create(Factory* pFactory)
	{
		m_pFactory = pFactory;
		bIsCreated = true;
	}

	bool IsCreated()
	{
		return bIsCreated;
	}

	float Delay()
	{
		return flDelay;
	}

	int BurstShotAmount()
	{
		return burstShots;
	}

	bool TriggerShoot(Vector& myViewAngles)
	{
		int local = m_pFactory->m_pEngine->GetLocalPlayer();
		auto ME = m_pFactory->m_pEntList->GetClientEntity(local);
		if (!ME)
			return false;

		int myTeam = ME->GetTeamNum();

		Vector myPos;
		if (!Helper::InterpolateEyePos(m_pFactory, myPos))
			return false;

		if (!ME->IsAlive())
			return false;

		for (int i = 0; i < m_pFactory->m_pEngine->GetMaxClients(); i++)
		{
			auto ent = m_pFactory->m_pEntList->GetClientEntity(i);
			if (!ent)
				continue;

			if (ent->IsValidPlayer() && ent->GetTeamNum() != myTeam)
			{
				matrix3x4_t bones[128];
				if (!ent->SetupBones(bones, 128, 0x00000100, m_pFactory->m_pEngine->GetLastTimeStamp()))
					continue;

				auto model = ent->GetModel();
				if (!model)
					continue;

				auto stdio = m_pFactory->m_pModelInfo->GetStudioModel(model);
				if (!stdio)
					continue;

				auto hitboxset = stdio->pHitboxSet(0);
				if (!hitboxset)
					continue;

				Vector dir;
				Utils::AngleVectors(myViewAngles, dir);

				for (auto k : hitboxes)
				{
					auto hitbox = hitboxset->pHitbox(k);

					if (Helper::IsVisible(m_pFactory, ME, ent, myPos,
						Vector(bones[hitbox->m_Bone].m[0][3], bones[hitbox->m_Bone].m[1][3], bones[hitbox->m_Bone].m[2][3])))
					{
						if (hitbox->m_flRadius == -1.f)
						{
							if (InsersectRayWithOBB(myPos, dir, bones[hitbox->m_Bone], hitbox->m_vBbmin, hitbox->m_vBbmax))
								return true;
						}
						else
						{
							Vector CapsuleA = Utils::MatMul(hitbox->m_vBbmin, bones[hitbox->m_Bone]);
							Vector CapsuleB = Utils::MatMul(hitbox->m_vBbmax, bones[hitbox->m_Bone]);

							if (DoesIntersectCapsule(myPos, dir, CapsuleA, CapsuleB, hitbox->m_flRadius))
								return true;
						}
					}
					
				}
			}
		}

		return false;
	}

	bool NoRecoil()
	{
		return noRecoil;
	}

	void Update()
	{
		int i = 0;
		for (auto it = items.begin(); it != items.end(); ++it, i++)
		{
			switch (i)
			{
			case eenabled:
				bEnabled = (*it)->GetbValue();
				break;
			case enorecoilview:
				noRecoil = (*it)->GetbValue();
				break;
			case edelay:
				flDelay = (*it)->GetiValue();
				break;
			case eburstshots:
				burstShots = (*it)->GetiValue();
				break;
			case ebones:
				hitboxes = ((CBoneSelector*)(*it))->GetSelection();
				break;
			default:
				break;
			}
		}
	}
};