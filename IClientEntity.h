#pragma once
#include "Utils.h"
#include "matrix3x4_t.h"
#include "CClientClass.h"

class IClientUnknown;
class IClientRenderable;
class IClientNetworkable;
class IClientThinkable;
class IClientEntity;

class IClientRenderable
{
public:
	virtual void*					GetIClientUnknown() = 0;
	virtual Vector const&			GetRenderOrigin(void) = 0;
	virtual Vector const&			GetRenderAngles(void) = 0;
	virtual bool					ShouldDraw(void) = 0;
	virtual bool					IsTransparent(void) = 0;
	virtual bool					UsesPowerOfTwoFrameBufferTexture() = 0;
	virtual bool					UsesFullFrameBufferTexture() = 0;
	virtual void					GetShadowHandle() const = 0;
	virtual void*					RenderHandle() = 0;
	virtual int						DrawModel(int flags) = 0;
	virtual int						GetBody() = 0;

	void GetRenderBounds(Vector& bbmin, Vector& bbmax)
	{
		typedef void(__thiscall* tGetRenderBounds)(PVOID, Vector& bbmin, Vector& bbmax);
		Utils::vfunc<tGetRenderBounds>(this, 17)(this, bbmin, bbmax);
	}

	void* GetModel()
	{
		typedef void*(__thiscall* tGetModel)(PVOID);
		return Utils::vfunc<tGetModel>(this, 8)(this);
	}

	bool SetupBones(matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
	{
		typedef bool(__thiscall* oSetupBones)(PVOID, matrix3x4_t*, int, int, float);
		return Utils::vfunc<oSetupBones>(this, 13)(this, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	}
};

class IClientNetworkable
{
public:
	virtual IClientUnknown*	GetIClientUnknown() = 0;
	virtual void			Release() = 0;
	virtual CClientClass*	GetClientClass() = 0;// FOR NETVARS FIND YOURSELF ClientClass* stuffs
	virtual void			NotifyShouldTransmit( /* ShouldTransmitState_t state*/) = 0;
	virtual void			OnPreDataChanged( /*DataUpdateType_t updateType*/) = 0;
	virtual void			OnDataChanged( /*DataUpdateType_t updateType*/) = 0;
	virtual void			PreDataUpdate( /*DataUpdateType_t updateType*/) = 0;
	virtual void			PostDataUpdate( /*DataUpdateType_t updateType*/) = 0;
	virtual void			unknown();
	virtual bool			IsDormant(void) = 0;
	virtual int				Index(void) const = 0;
	virtual void			ReceiveMessage(int classID /*, bf_read &msg*/) = 0;
	virtual void*			GetDataTableBasePtr() = 0;
	virtual void			SetDestroyedOnRecreateEntities(void) = 0;
};

class IClientUnknown
{
public:
	virtual void*				GetCollideable() = 0;
	virtual IClientNetworkable*	GetClientNetworkable() = 0;
	virtual IClientRenderable*	GetClientRenderable() = 0;
	virtual IClientEntity*		GetIClientEntity() = 0;
	virtual IClientEntity*		GetBaseEntity() = 0;
	virtual IClientThinkable*	GetClientThinkable() = 0;
};

class IClientThinkable
{
public:
	virtual IClientUnknown*		GetIClientUnknown() = 0;
	virtual void				ClientThink() = 0;
	virtual void*				GetThinkHandle() = 0;
	virtual void				SetThinkHandle(void* hThink) = 0;
	virtual void				Release() = 0;
};

class __declspec(novtable) IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
	virtual void			Release(void) = 0;
	virtual void			blahblahpad(void) = 0;
	virtual Vector&	GetAbsOrigin(void) const = 0;//in broken place use GetOrigin Below
	virtual const Vector&	GetAbsAngles(void) const = 0;

	Vector GetOrigin()
	{
		return *(Vector*)((DWORD)this + (DWORD)0x134);
	}

	Vector GetRotation()
	{
		return *(Vector*)((DWORD)this + (DWORD)0x128);
	}

	matrix3x4_t GetRGFLCoordinateFrame()
	{
		return *(matrix3x4_t*)((DWORD)this + (DWORD)(0x046C-0x30));
	}

	Vector Mins()
	{
		return *(Vector*)((DWORD)this + (DWORD)(0x0314+0x8));
	}

	Vector Maxs()
	{
		return *(Vector*)((DWORD)this + (DWORD)(0x0314+0x14));
	}

	int GetTeamNum()
	{
		return *(int*)((DWORD)this + (DWORD)0xF0);
	}

	int GetFlags()
	{
		return *(int*)((DWORD)this + (DWORD)0x100);
	}

	int GetHealth()
	{
		return *(int*)((DWORD)this + (DWORD)0xFC);
	}

	bool IsDead()
	{
		return *(bool*)((DWORD)this + (DWORD)0x25B);
	}

	bool IsAlive()
	{
		return !IsDead();
	}

	Vector GetEyePosition()
	{
		Vector headOffset = *(Vector*)((DWORD)this + (DWORD)0x104);
		return GetOrigin() + headOffset;
	}

	bool IsDormant()
	{
		return *(bool*)((DWORD)this + (DWORD)0xE9);
	}

	bool IsNotASpectator()
	{
		//2 = T, 3 = CT
		return GetTeamNum() == 2 || GetTeamNum() == 3;
	}

	bool IsValidPlayer()
	{
		return !IsDormant() && IsNotASpectator() && IsAlive() && GetHealth() > 0;
	}

	unsigned long GetActiveWeapon()
	{
		return *(unsigned long*)((DWORD)this + (DWORD)0x000012C0);
	}

	char* GetModelName()
	{
		return (char*)*(DWORD*)((DWORD)this + 0x6C);
	}

	Vector GetPunchAngles()
	{
		return *(Vector*)((DWORD)this + 0x000013E8);
	}

	/*
	Vector GetViewPunchAngles()
	{
		return *(Vector*)((DWORD)this + 0x1378 + 0x64);
	}

	void SetViewPunchAngles(Vector& angles)
	{
		*(Vector*)((DWORD)this + 0x1378 + 0x64) = angles;
	}
	*/

	Vector GetVelocity()
	{
		return *(Vector*)((DWORD)this + 0x110);
	}

	int GetTickBase()
	{
		return *(int*)((DWORD)this + 0x000017DC);
	}
};