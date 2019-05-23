#pragma once
#define _USE_MATH_DEFINES
#include "Factory.h"
#include "Helper.h"

#include <map>
#include <math.h>
#include <string>
#include "GUI\Observer.h"

#include "CWeaponConfig.h"

#define SMALL_NUM   0.00000001
#define dot(u,v)   ((u).x * (v).x + (u).y * (v).y + (u).z * (v).z)
#define norm(v)    sqrt(dot(v,v))
#define d(u,v)     norm(u-v)        
#define abs(x)     ((x) >= 0 ? (x) : -(x))  

class Aimbot : public Observer
{
private:
	bool bIsCreated = false;

	enum Cheats
	{
		eenabled,
		eshootfriendly,
		enorecoilview,
		esilentaim,
		emode,
		elegit,
		enonsticky,
		edelay,
		elocktime,
		eshootinair,
		eautoshoot,
		ebone,
		MAX
	};

	enum MODE
	{
		FOV,
		CLOSEST_REAL_DIST
	};

	Factory* m_pFactory;

	MODE mode = MODE::FOV;

	bool bEnabled = false;
	bool noRecoil = false;
	bool bSilentAim = false;
	bool bLegit = false;

	bool shootfriendly = false;

	bool bNonSticky = true;
	bool bDelay = true;
	bool ShootInAir = false;
	float aimDelay = 100;
	float aimLockTime = 100;

	std::vector<int> hitboxIDs;

	bool bAutoShoot;

	struct hitbox_t 
	{
		Vector bbMin;
		Vector bbMax;
		int bone;
		float radius;
	};
public:
	WeaponConfig* _wpnCfg;
public:
	Aimbot(WeaponConfig* wpnCfg) : _wpnCfg(wpnCfg) {}

	float static GetRealDistance(Vector &myViewAngles, Vector& targetDir)
	{
		float dist = targetDir.length();
		Vector targetAngles;
		Utils::VectorAngles(targetDir, targetAngles);

		float pitchDiff = abs(myViewAngles.x - targetAngles.x);
		float yawDiff = abs(myViewAngles.y - targetAngles.y);

		float realDistX = sin(D3DXToRadian(pitchDiff)) * dist;
		float realDistY = sin(D3DXToRadian(yawDiff)) * dist;

		return sqrt(realDistX*realDistX + realDistY*realDistY);
	}

	float static GetFOV(Vector& myViewAngles, Vector& targetDir)
	{
		Vector targetAngles;
		Utils::VectorAngles(targetDir, targetAngles);
		Vector targetClamped = Vector::ClampAngle(targetAngles);

		Vector delta;
		delta = Vector::ClampAngle(myViewAngles - targetClamped);

		float fov = delta.length();

		return fov;
	}
public:
	bool NoRecoil()
	{
		return noRecoil;
	}

	bool Enabled()
	{
		return bEnabled;
	}

	bool Legit()
	{
		return bLegit;
	}

	bool SilentAim()
	{
		return bSilentAim;
	}

	void Create(Factory* pFactory)
	{
		m_pFactory = pFactory;
		bIsCreated = true;
	}

	bool Autoshoot()
	{
		return bAutoShoot;
	}

	bool IsCreated()
	{
		return bIsCreated;
	}

	float LockTime()
	{
		return aimLockTime;
	}

	DWORD Aim(Vector& Myviewangles, Vector& angles, float& lastAimTime, bool shouldWrite, bool& shouldShoot, int weaponID, float& startTime)
	{
		static IClientEntity* oldTarget;
		static float curTime;
		static bool bSticky;
		static bool bShouldShoot = false;
		shouldShoot = bShouldShoot;

		IClientEntity* target = 0;
		Vector gtargetDir, gTargetBone;
		float minDist = 999999;
		hitbox_t hitboxInfo;
		matrix3x4_t gboneMat;

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
			if (i == local)
				continue;

			auto ent = m_pFactory->m_pEntList->GetClientEntity(i);
			if (ent)
			{
				if (ent->IsValidPlayer() && ((!shootfriendly && ent->GetTeamNum() != myTeam) || shootfriendly))
				{
					if (!ShootInAir && !(ent->GetFlags() & FL_ON_GROUND))
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

					for (auto k : hitboxIDs)
					{
						auto hitbox = hitboxset->pHitbox(k);

						if (Helper::GetBoneMatrix(m_pFactory, ent, hitbox->m_Bone, gboneMat))
						{
							gTargetBone.x = gboneMat.m[0][3];
							gTargetBone.y = gboneMat.m[1][3];
							gTargetBone.z = gboneMat.m[2][3];

							bool bIsVis = Helper::IsVisible(m_pFactory, ME, ent, myPos, gTargetBone);
							if (bIsVis)
							{
								Vector targetDir = gTargetBone - myPos;
								float targetDistance = targetDir.length();

								if (targetDistance > _wpnCfg->MinAimDist(weaponID) / 0.01905f)
								{
									if (mode == MODE::FOV)
									{
										float fov = GetFOV(Myviewangles, targetDir);

										if (fov < _wpnCfg->fov(weaponID))
										{
											if (fov < minDist)
											{
												minDist = fov;
												target = ent;
												gtargetDir = targetDir;
												hitboxInfo.bbMax = hitbox->m_vBbmax;
												hitboxInfo.bbMin = hitbox->m_vBbmin;
												hitboxInfo.bone = hitbox->m_Bone;
												hitboxInfo.radius = hitbox->m_flRadius;
											}
										}
									}
									if (mode == MODE::CLOSEST_REAL_DIST)
									{
										float realDist = GetRealDistance(Myviewangles, targetDir);

										if (GetFOV(Myviewangles, targetDir) <= 45 && realDist < _wpnCfg->fov(weaponID))
										{
											if (realDist < minDist)
											{
												minDist = realDist;
												target = ent;
												gtargetDir = targetDir;
												hitboxInfo.bbMax = hitbox->m_vBbmax;
												hitboxInfo.bbMin = hitbox->m_vBbmin;
												hitboxInfo.bone = hitbox->m_Bone;
												hitboxInfo.radius = hitbox->m_flRadius;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}

		if (target)
		{
			if (shouldWrite)
			{
				oldTarget = target;
				lastAimTime = m_pFactory->m_pEngine->GetLastTimeStamp();
				curTime = m_pFactory->m_pEngine->GetLastTimeStamp();
				bSticky = false;
				bDelay = true;
				bShouldShoot = false;
				shouldShoot = bShouldShoot;
			}

			Vector myDir;
			Utils::AngleVectors(Myviewangles, myDir);
			matrix3x4_t boneMat;

			if (Helper::GetBoneMatrix(m_pFactory, target, hitboxInfo.bone, boneMat))
			{
				if (hitboxInfo.radius == -1.f)
				{
					if (InsersectRayWithOBB(myPos, myDir, boneMat, hitboxInfo.bbMin, hitboxInfo.bbMax))
					{
						bSticky = true;
						bShouldShoot = true;
						shouldShoot = bShouldShoot;
					}
				}
				else
				{
					Vector CapsuleA = Utils::MatMul(hitboxInfo.bbMin, boneMat);
					Vector CapsuleB = Utils::MatMul(hitboxInfo.bbMax, boneMat);

					if (DoesIntersectCapsule(myPos, myDir, CapsuleA, CapsuleB, hitboxInfo.radius))
					{
						bSticky = true;
						bShouldShoot = true;
						shouldShoot = bShouldShoot;
					}
				}
			}

			if (bNonSticky && bSticky)
			{
				bSticky = false;
				angles = Myviewangles;
				startTime = -1;
				return 1;
			}

			//we changed the target
			if (target != oldTarget)
			{
				curTime = m_pFactory->m_pEngine->GetLastTimeStamp();
				oldTarget = target;
				bDelay = true;
				bShouldShoot = false;
				shouldShoot = bShouldShoot;
			}

			if (bDelay)
			{
				if (bDelay && (curTime + aimDelay / 100 <= m_pFactory->m_pEngine->GetLastTimeStamp()))
				{
					Vector targetAngles;
					Utils::VectorAngles(gtargetDir, targetAngles);
					angles = Vector::ClampAngle(targetAngles);

					curTime = 0;
					bDelay = false;
					startTime = m_pFactory->m_pEngine->GetLastTimeStamp();
				}
				else
				{
					return 0;
				}
			}
			else
			{
				Vector targetAngles;
				Utils::VectorAngles(gtargetDir, targetAngles);
				angles = Vector::ClampAngle(targetAngles);
			}

			return 1;

		}

		return -1;
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

	bool AimSilent(Vector& Myviewangles, Vector& angles, int weaponID)
	{
		IClientEntity* target = 0;
		Vector gtargetDir, gTargetBone;
		float minDist = 999999;
		matrix3x4_t gboneMat;

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
			if (i == local)
				continue;

			auto ent = m_pFactory->m_pEntList->GetClientEntity(i);
			if (ent)
			{
				if (ent->IsValidPlayer() && ((!shootfriendly && ent->GetTeamNum() != myTeam) || shootfriendly))
				{
					if (Helper::GetBoneMatrix(m_pFactory, ent, 6, gboneMat))
					{
						gTargetBone.x = gboneMat.m[0][3];
						gTargetBone.y = gboneMat.m[1][3];
						gTargetBone.z = gboneMat.m[2][3];

						bool bIsVis = Helper::IsVisible(m_pFactory, ME, ent, myPos, gTargetBone);
						if (bIsVis)
						{
							Vector targetDir = gTargetBone - myPos;
							float targetDistance = targetDir.length();

							if (targetDistance > _wpnCfg->MinAimDist(weaponID) / 0.01905f)
							{
								if (mode == MODE::FOV)
								{
									float fov = GetFOV(Myviewangles, targetDir);

									if (fov < _wpnCfg->fov(weaponID))
									{
										if (fov < minDist)
										{
											minDist = fov;
											target = ent;
											gtargetDir = targetDir;
										}
									}
								}
								if (mode == MODE::CLOSEST_REAL_DIST)
								{
									float realDist = GetRealDistance(Myviewangles, targetDir);

									if (GetFOV(Myviewangles, targetDir) <= 45 && realDist < _wpnCfg->fov(weaponID))
									{
										if (realDist < minDist)
										{
											minDist = realDist;
											target = ent;
											gtargetDir = targetDir;
										}
									}
								}
							}
						}
					}
				}
			}
		}

		if (target)
		{
			Vector targetAngles;
			Utils::VectorAngles(gtargetDir, targetAngles);
			angles = Vector::ClampAngle(targetAngles);

			return true;
		}

		return false;
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
			case eshootfriendly:
				shootfriendly = (*it)->GetbValue();
				break;
			case enorecoilview:
				noRecoil = (*it)->GetbValue();
				break;
			case esilentaim:
				bSilentAim = (*it)->GetbValue();
				break;
			case emode:
				mode = (MODE)(*it)->GetiValue();
				break;
			case elegit:
				bLegit = (*it)->GetbValue();
				break;
			case enonsticky:
				bNonSticky = (*it)->GetbValue();
				break;
			case edelay:
				aimDelay = (*it)->GetiValue();
				break;
			case elocktime:
				aimLockTime = (*it)->GetiValue();
				break;
			case eshootinair:
				ShootInAir = (*it)->GetbValue();
				break;
			case eautoshoot:
				bAutoShoot = (*it)->GetbValue();
				break;
			case ebone:
				hitboxIDs = ((CBoneSelector*)(*it))->GetSelection();
				break;
			default:
				break;
			}
		}
	}
};