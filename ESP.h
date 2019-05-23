#pragma once
#include <string>

#include "Factory.h"
#include "EngineRender.h"
#include "trace_t.h"
#include "CTraceFilter.h"
#include "Helper.h"
#include "GUI\Observer.h"

#define T 2
#define CT 3

#define TCOLOR pRender->GetColor(EngineRender::eColors::RED)
#define CTCOLOR pRender->GetColor(EngineRender::eColors::SKYBLUE)
#define TNVCOLOR pRender->GetColor(EngineRender::eColors::YELLOW)
#define CTNVCOLOR pRender->GetColor(EngineRender::eColors::GREEN)

class ESP : public Observer
{
private:
	enum Cheats
	{
		eenabled,
		eenemyonly,
		ebox,
		ehealth,
		ename,
		eweapon,
		eentity,
		eskeleton,
		edist,
		eboneids,
		ehitboxesp,
		MAX
	};

	EngineRender* pRender;
	Factory* pFactory;
	bool bIsCreated = false;

	void DrawHitbox(matrix3x4_t bone, D3DXMATRIX world, int w, int h, Vector min, Vector max)
	{
		Vector points[] = {
			Vector(min.x, min.y, min.z),
			Vector(min.x, max.y, min.z),
			Vector(max.x, max.y, min.z),
			Vector(max.x, min.y, min.z),
			Vector(max.x, max.y, max.z),
			Vector(min.x, max.y, max.z),
			Vector(min.x, min.y, max.z),
			Vector(max.x, min.y, max.z),
		};

		Vector transformedMat[8];
		for (int k = 0; k < 8; k++)
			transformedMat[k] = Utils::MatMul(points[k], bone);

		Vector transformedScreen[8];
		for (int k = 0; k < 8; k++)
			Utils::ScreenTransform(transformedMat[k], transformedScreen[k], world, w, h);

		float left = transformedScreen[0].x;
		float right = transformedScreen[0].x;
		float top = transformedScreen[0].y;
		float bottom = transformedScreen[0].y;

		for (int i = 1; i < 8; i++)
		{
			if (left > transformedScreen[i].x)
				left = transformedScreen[i].x;
			if (top < transformedScreen[i].y)
				top = transformedScreen[i].y;
			if (right < transformedScreen[i].x)
				right = transformedScreen[i].x;
			if (bottom > transformedScreen[i].y)
				bottom = transformedScreen[i].y;
		}

		pRender->DrawBox(left, bottom, right - left, top - bottom, 1, pRender->GetColor(EngineRender::eColors::WHITE));
	}

	void HitboxESP(IClientEntity* ent, matrix3x4_t* bones, D3DXMATRIX world, int w, int h, DWORD color)
	{
		auto model = ent->GetModel();
		if (!model)
			return;

		auto stdiohdr = pFactory->m_pModelInfo->GetStudioModel(model);
		if (!stdiohdr)
			return;

		auto hitboxSet = stdiohdr->pHitboxSet(0);
		if (!hitboxSet)
			return;

		for (int i = 0; i < hitboxSet->numhitboxes; i++)
		{
			auto hitbox = hitboxSet->pHitbox(i);
			if (!hitbox)
				continue;

			Vector pos(bones[hitbox->m_Bone].m[0][3], bones[hitbox->m_Bone].m[1][3], bones[hitbox->m_Bone].m[2][3]);
			Vector orientation = Utils::MatrixAngles(bones[hitbox->m_Bone]);
			int r, g, b;
			Utils::ColorToRGB(color, &r, &g, &b);

			if (hitbox->m_flRadius == -1.f)
			{
				pFactory->m_pDebugOverlay->AddBoxOverlay(pos, hitbox->m_vBbmin, hitbox->m_vBbmax, orientation, r,g,b, 255, 0.1);
			}
			else
			{
				pFactory->m_pDebugOverlay->AddBoxOverlay(pos, hitbox->m_vBbmin - hitbox->m_flRadius, hitbox->m_vBbmax + hitbox->m_flRadius, orientation, 
					r, g, b, 255, 0.1);
			}
		}
	}

	void DrawESPEnemyOnly()
	{
		if (pFactory->m_pEngine->IsInGame() && pFactory->m_pEngine->IsConnected())
		{
			D3DXMATRIX& mat = pFactory->m_pEngine->WorldToScreenMatrix();
			int w, h;
			pFactory->m_pEngine->GetScreenSize(w, h);
			int local = pFactory->m_pEngine->GetLocalPlayer();
			auto ME = pFactory->m_pEntList->GetClientEntity(local);
			int myteam;
			Vector myEye;
			if (ME)
			{
				myteam = ME->GetTeamNum();
				myEye = ME->GetEyePosition();
			}
			else
				return;

			for (int i = 0; i < pFactory->m_pEngine->GetMaxClients(); i++)
			{
				if (i == local)
					continue;

				auto ent = pFactory->m_pEntList->GetClientEntity(i);
				if (ent && ent->IsValidPlayer())
				{
					Vector bot;
					matrix3x4_t bones[128];

					if (!ent->SetupBones(bones, 128, 0x00000100, pFactory->m_pEngine->GetLastTimeStamp()) ||
						!GetBoneVector(bones, 1, mat, w, h, bot))
					{
						continue;
					}

					Vector head3d = GetBoneVector3D(bones, 6);
					float dist = (myEye - head3d).length() * 0.01905f;
					DWORD color;
					int team = ent->GetTeamNum();

					bool bIsVis = Helper::IsVisible(pFactory, ME, ent, myEye, head3d);

					if (myteam == T)
					{
						if (team == CT)
						{
							if (bIsVis)
								color = CTCOLOR;
							else
								color = CTNVCOLOR;
						}
					}
					if (myteam == CT)
					{
						if (team == T)
						{
							if (bIsVis)
								color = TCOLOR;
							else
								color = TNVCOLOR;
						}
					}

					if (myteam != team)
					{
						auto transformMat = ent->GetRGFLCoordinateFrame();
						auto min = ent->Mins();
						auto max = ent->Maxs();

						Vector points[] = {
							Vector(min.x, min.y, min.z),
							Vector(min.x, max.y, min.z),
							Vector(max.x, max.y, min.z),
							Vector(max.x, min.y, min.z),
							Vector(max.x, max.y, max.z),
							Vector(min.x, max.y, max.z),
							Vector(min.x, min.y, max.z),
							Vector(max.x, min.y, max.z),
							Vector(max.x, min.y, max.z+10),
							Vector(max.x, min.y, max.z-5),
						};

						Vector transformedMat[10];
						for (int k = 0; k < 10; k++)
							transformedMat[k] = Utils::MatMul(points[k], transformMat);

						Vector transformedScreen[10];
						for (int k = 0; k < 10; k++)
							if (!Utils::ScreenTransform(transformedMat[k], transformedScreen[k], mat, w, h))
								continue;

						float left = transformedScreen[0].x;
						float right = transformedScreen[0].x;
						float top = transformedScreen[0].y;
						float bottom = transformedScreen[0].y;

						for (int i = 1; i < 8; i++)
						{
							if (left > transformedScreen[i].x)
								left = transformedScreen[i].x;
							if (top < transformedScreen[i].y)
								top = transformedScreen[i].y;
							if (right < transformedScreen[i].x)
								right = transformedScreen[i].x;
							if (bottom > transformedScreen[i].y)
								bottom = transformedScreen[i].y;
						}

						int offset = 3;

						if(SkeletonESP)
							DrawSkeleton(ent, color, mat, bones, w, h);

						if (BoxESP)
							pRender->DrawBox(left, bottom, 
								abs(left-right),  
								abs(top-bottom), 
								1, color);

						if (HealthESP)
						{
							float h = abs(transformedScreen[8].y - transformedScreen[9].y);
							if (abs(transformedScreen[8].y - transformedScreen[9].y) > 7)
								h = 7;
							
							if (abs(transformedScreen[8].y - transformedScreen[9].y) < 3)
								h = 3;

							pRender->DrawHealthBar(left, transformedScreen[8].y,
								abs(left-right),
								h,
								ent->GetHealth());
						}

						player_info_t info;
						pFactory->m_pEngine->GetPlayerInfo(i, &info);

						if (NameESP)
						{
							pRender->DrawOutlinedFontText(bot.x, bot.y + offset, pRender->GetColor(EngineRender::eColors::PINK), Utils::asciiToUnicode(info.name).c_str());
							offset += 10;
						}

						if (WeaponESP)
						{
							pRender->DrawOutlinedFontText(bot.x, bot.y + offset, pRender->GetColor(EngineRender::eColors::ORANGE), Utils::asciiToUnicode(GetCurrentWeapon(ent)).c_str());
							offset += 10;
						}

						if (DistESP)
							pRender->DrawOutlinedFontText(bot.x, bot.y + offset, pRender->GetColor(EngineRender::eColors::WHITE), L"%d m", (int)dist);

						if (bHitboxESP)
							HitboxESP(ent, bones, mat, w, h, color);

						if (DrawBones)
						{
							Vector boneScreen;

							auto model = ent->GetModel();
							if (!model)
								continue;

							auto stdiohdr = pFactory->m_pModelInfo->GetStudioModel(model);
							if (!stdiohdr)
								continue;

							auto hitboxSet = stdiohdr->pHitboxSet(0);
							if (!hitboxSet)
								continue;

							for (int i = 0; i < hitboxSet->numhitboxes; i++)
							{
								auto hitbox = hitboxSet->pHitbox(i);
								if (!hitbox)
									continue;

								if (GetBoneVector(bones, hitbox->m_Bone, mat, w, h, boneScreen))
									pRender->DrawOutlinedFontText(boneScreen.x, boneScreen.y, pRender->GetColor(EngineRender::eColors::WHITE), L"%d", i);
							}
						}
					}
				}
			}
		}
	}

	void DrawESPAll()
	{
		if (pFactory->m_pEngine->IsInGame() && pFactory->m_pEngine->IsConnected())
		{
			D3DXMATRIX& mat = pFactory->m_pEngine->WorldToScreenMatrix();
			int w, h;
			pFactory->m_pEngine->GetScreenSize(w, h);
			int local = pFactory->m_pEngine->GetLocalPlayer();
			auto ME = pFactory->m_pEntList->GetClientEntity(local);
			Vector myEye;
			if (ME)
			{
				myEye = ME->GetEyePosition();
			}
			else
				return;

			for (int i = 0; i < pFactory->m_pEngine->GetMaxClients(); i++)
			{
				if (i == local)
					continue;

				auto ent = pFactory->m_pEntList->GetClientEntity(i);
				if (ent && ent->IsValidPlayer())
				{
					Vector bot;
					matrix3x4_t bones[128];

					if (!ent->SetupBones(bones, 128, 0x00000100, pFactory->m_pEngine->GetLastTimeStamp()) ||
						!GetBoneVector(bones, 1, mat, w, h, bot))
					{
						continue;
					}

					Vector head3d = GetBoneVector3D(bones, 6);
					float dist = (myEye - head3d).length() * 0.01905f;
					DWORD color;
					int team = ent->GetTeamNum();

					bool bIsVis = Helper::IsVisible(pFactory, ME, ent, myEye, head3d);

					if (team == T)
					{
						if (bIsVis)
							color = TCOLOR;
						else
							color = TNVCOLOR;
					}

					if (team == CT)
					{
						if (bIsVis)
							color = CTCOLOR;
						else
							color = CTNVCOLOR;
					}

					auto transformMat = ent->GetRGFLCoordinateFrame();
					auto min = ent->Mins();
					auto max = ent->Maxs();

					Vector points[] = {
						Vector(min.x, min.y, min.z),
						Vector(min.x, max.y, min.z),
						Vector(max.x, max.y, min.z),
						Vector(max.x, min.y, min.z),
						Vector(max.x, max.y, max.z),
						Vector(min.x, max.y, max.z),
						Vector(min.x, min.y, max.z),
						Vector(max.x, min.y, max.z),
						Vector(max.x, min.y, max.z + 10),
						Vector(max.x, min.y, max.z - 5),
					};

					Vector transformedMat[10];
					for (int k = 0; k < 10; k++)
						transformedMat[k] = Utils::MatMul(points[k], transformMat);

					Vector transformedScreen[10];
					for (int k = 0; k < 10; k++)
						if (!Utils::ScreenTransform(transformedMat[k], transformedScreen[k], mat, w, h))
							continue;

					float left = transformedScreen[0].x;
					float right = transformedScreen[0].x;
					float top = transformedScreen[0].y;
					float bottom = transformedScreen[0].y;

					for (int i = 1; i < 8; i++)
					{
						if (left > transformedScreen[i].x)
							left = transformedScreen[i].x;
						if (top < transformedScreen[i].y)
							top = transformedScreen[i].y;
						if (right < transformedScreen[i].x)
							right = transformedScreen[i].x;
						if (bottom > transformedScreen[i].y)
							bottom = transformedScreen[i].y;
					}

					int offset = 3;

					if (SkeletonESP)
						DrawSkeleton(ent, color, mat, bones, w, h);

					if (BoxESP)
						pRender->DrawBox(left, bottom,
						abs(left - right),
						abs(top - bottom),
						1, color);

					if (HealthESP)
					{
						float h = abs(transformedScreen[8].y - transformedScreen[9].y);
						if (abs(transformedScreen[8].y - transformedScreen[9].y) > 7)
							h = 7;

						if (abs(transformedScreen[8].y - transformedScreen[9].y) < 3)
							h = 3;

						pRender->DrawHealthBar(left, transformedScreen[8].y,
							abs(left - right),
							h,
							ent->GetHealth());
					}

					player_info_t info;
					pFactory->m_pEngine->GetPlayerInfo(i, &info);

					if (NameESP)
					{
						pRender->DrawOutlinedFontText(bot.x, bot.y + offset, pRender->GetColor(EngineRender::eColors::PINK), Utils::asciiToUnicode(info.name).c_str());
						offset += 10;
					}

					if (WeaponESP)
					{
						pRender->DrawOutlinedFontText(bot.x, bot.y + offset, pRender->GetColor(EngineRender::eColors::ORANGE), Utils::asciiToUnicode(GetCurrentWeapon(ent)).c_str());
						offset += 10;
					}

					if (DistESP)
						pRender->DrawOutlinedFontText(bot.x, bot.y + offset, pRender->GetColor(EngineRender::eColors::WHITE), L"%d m", (int)dist);

					if (bHitboxESP)
						HitboxESP(ent, bones, mat, w, h, color);

					if (DrawBones)
					{
						Vector boneScreen;

						auto model = ent->GetModel();
						if (!model)
							continue;

						auto stdiohdr = pFactory->m_pModelInfo->GetStudioModel(model);
						if (!stdiohdr)
							continue;

						auto hitboxSet = stdiohdr->pHitboxSet(0);
						if (!hitboxSet)
							continue;

						for (int i = 0; i < hitboxSet->numhitboxes; i++)
						{
							auto hitbox = hitboxSet->pHitbox(i);
							if (!hitbox)
								continue;

							if (GetBoneVector(bones, hitbox->m_Bone, mat, w, h, boneScreen))
								pRender->DrawOutlinedFontText(boneScreen.x, boneScreen.y, pRender->GetColor(EngineRender::eColors::WHITE), L"%d", i);
						}
					}
				}
			}
		}
	}

	Vector GetBoneVector3D(matrix3x4_t boneMat[128], int Bone)
	{
		return Vector(boneMat[Bone].m[0][3], boneMat[Bone].m[1][3], boneMat[Bone].m[2][3]);
	}

	bool GetBoneVector(matrix3x4_t boneMat[128], int Bone, D3DXMATRIX mat, int w, int h, Vector& screen)
	{
		Vector pos(boneMat[Bone].m[0][3], boneMat[Bone].m[1][3], boneMat[Bone].m[2][3]);
		if (Utils::ScreenTransform(pos, screen, mat, w, h))
		{
			return true;
		}

		return false;
	}

	char* GetCurrentWeapon(IClientEntity* ent)
	{
		unsigned long i = ent->GetActiveWeapon();
		CBaseWeapon* BaseWeapon = (CBaseWeapon*)pFactory->m_pEntList->GetClientEntityFromHandle(i);
		if (BaseWeapon)
		{
			int id = BaseWeapon->GetWeaponID();
			return (char*)BaseWeapon->GetWeaponName(id);
		}

		return "Error";
	}

	void DrawSkeleton(IClientEntity *ent, DWORD color, D3DXMATRIX& vMatrix, matrix3x4_t* bones, int w, int h)
	{
		Vector boneScreen, parentScreen;

		auto model = ent->GetModel();
		if (!model)
			return;

		auto stdiohdr = pFactory->m_pModelInfo->GetStudioModel(model);
		if (!stdiohdr)
			return;

		for (int i = 0; i < stdiohdr->numbones; i++)
		{
			auto bone = stdiohdr->pBone(i);
			if (!bone)
				continue;

			if (bone->flags & 0x100 && bone->parent >= 0 && bone->parent < 128)
				if (GetBoneVector(bones, i, vMatrix, w, h, boneScreen) && GetBoneVector(bones, bone->parent, vMatrix, w, h, parentScreen))
					pRender->DrawLine(parentScreen.x, parentScreen.y, boneScreen.x, boneScreen.y, color);
		}

	}

	DWORD mapHP(int x)
	{
		int B = 0;
		int G = (255 / 100.)*x;
		if (G > 255)
			G = 255;
		int R = 255 - G;

		return D3DCOLOR_RGBA(R, G, B, 255);
	}

	bool BoxESP = false;
	bool SkeletonESP = false;
	bool HealthESP = false;
	bool NameESP = false;
	bool EnemyOnly = false;
	bool Enabled = false;
	bool DistESP = false;
	bool WeaponESP = false;
	bool EntityESP = false;
	bool DrawBones = false;
	bool bHitboxESP = false;
public:
	void DrawESP()
	{
		if (!Enabled)
			return;

		if (EnemyOnly)
			DrawESPEnemyOnly();
		else
			DrawESPAll();
	}

	void EntESP()
	{
		if (!Enabled || !EntityESP)
			return;

		if (pFactory->m_pEngine->IsInGame() && pFactory->m_pEngine->IsConnected())
		{
			D3DXMATRIX& mat = pFactory->m_pEngine->WorldToScreenMatrix();
			int w, h;
			pFactory->m_pEngine->GetScreenSize(w, h);
			int max = pFactory->m_pEntList->GetHighestEntityIndex();

			for (int i = 0; i < max; i++)
			{
				auto ent = pFactory->m_pEntList->GetClientEntity(i);
				if (ent)
				{
					char name[200] = { 0 };
					bool didWork = false;
					auto className = ent->GetClientClass()->GetName();
					auto modelName = ent->GetModelName();
					
					if (className) 
					{
						if (!strcmp(className, "CAK47"))
						{
							memcpy(name, "AK47", strlen("AK47"));
							didWork = true;
						}

						if (strstr(className, "CWeapon"))
						{
							memcpy(name, className + 1 + strlen("Weapon"), strlen(className) - strlen("Weapon") + 1);
							didWork = true;
						}

						if (!strcmp(className, "CC4") || !strcmp(className, "CPlantedC4"))
						{
							memcpy(name, "C4", strlen("C4"));
							didWork = true;
						}

						//Molotov exploded
						if (strstr(className, "CInferno"))
						{
							memcpy(name, "Molotov", strlen("Molotov"));
							didWork = true;
						}
					}

					if (modelName)
					{
						//Defuse Kit
						if (strstr(modelName, "efuser"))
						{
							memcpy(name, "DefuseKit", strlen("DefuseKit"));
							didWork = true;
						}
						//Molotov
						if (strstr(modelName, "molotov"))
						{
							memcpy(name, "Molotov", strlen("Molotov"));
							didWork = true;
						}

						//Ködergranate xd
						if (strstr(modelName, "decoy"))
						{
							memcpy(name, "Decoy", strlen("Decoy"));
							didWork = true;
						}

						if (strstr(modelName, "smokegrenade"))
						{
							memcpy(name, "Smoke", strlen("Smoke"));
							didWork = true;
						}
						if (strstr(modelName, "flashbang"))
						{
							memcpy(name, "Flash", strlen("Flash"));
							didWork = true;
						}
						if (strstr(modelName, "fraggrenade"))
						{
							memcpy(name, "He", strlen("He"));
							didWork = true;
						}
					}

					Vector screen;
					if (didWork && Utils::ScreenTransform(ent->GetOrigin(), screen, mat, w, h))
						pRender->DrawOutlinedFontText(screen.x, screen.y, pRender->GetColor(EngineRender::eColors::WHITE), Utils::asciiToUnicode(name).c_str());
				}
			}
		}
	}

	bool IsCreated()
	{
		return bIsCreated;
	}

	void Create(EngineRender* Render, Factory* factory)
	{
		pRender = Render;
		pFactory = factory;
		bIsCreated = true;
	}

	void Update()
	{
		int i = 0;
		for (auto it = items.begin(); it != items.end(); ++it, i++)
		{
			switch (i)
			{
			case eenabled:
				Enabled = (*it)->GetbValue();
				break;
			case eenemyonly:
				EnemyOnly = (*it)->GetbValue();
				break;
			case ebox:
				 BoxESP = (*it)->GetbValue();
				break;
			case ehealth:
				HealthESP = (*it)->GetbValue();
				break;
			case ename:
				NameESP = (*it)->GetbValue();
				break;
			case eweapon:
				WeaponESP = (*it)->GetbValue();
				break;
			case eentity:
				EntityESP = (*it)->GetbValue();
				break;
			case eskeleton:
				SkeletonESP = (*it)->GetbValue();
				break;
			case edist:
				DistESP = (*it)->GetbValue();
				break;
			case eboneids:
				DrawBones = (*it)->GetbValue();
				break;
			case ehitboxesp:
				bHitboxESP = (*it)->GetbValue();
				break;
			default:
				break;
			}
		}
	}
};