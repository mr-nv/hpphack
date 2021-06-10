#include "main.h"

bool C_BaseEntity::IsPlayer()
{
	//index: 152
	//ref: "effects/nightvision"
	//sig: 8B 92 ? ? ? ? FF D2 84 C0 0F 45 F7 85 F6
	return CallVFunction<bool(__thiscall*)(C_BaseEntity*)>(this, 157)(this);
}

Vector C_BasePlayer::GetEyePos()
{
	return m_vecOrigin() + m_vecViewOffset();
}

bool C_BasePlayer::IsAlive()
{
	return m_lifeState() == LIFE_ALIVE;
}

Vector C_BasePlayer::GetHitboxPos(int hitbox_id)
{
	matrix3x4_t BoneMatrix[MAXSTUDIOBONES];
	if (!SetupBones(BoneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.f))
		return Vector();

	auto* pStudioModel = g_pIModelInfo->GetStudiomodel(GetModel());
	if (!pStudioModel)
		return Vector();

	auto* pHitbox = pStudioModel->GetHitboxSet(0)->GetHitbox(hitbox_id);
	if (!pHitbox)
		return Vector();

	Vector min, max;
	g_Utils.VectorTransform(pHitbox->bbmin, BoneMatrix[pHitbox->bone], min);
	g_Utils.VectorTransform(pHitbox->bbmax, BoneMatrix[pHitbox->bone], max);

	return (min + max) / 2.f;
}

Vector C_BasePlayer::GetBonePos(int bone)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];
	if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, 0.0f))
		return boneMatrix[bone].at(3);

	return Vector();
}

player_info_t C_BasePlayer::GetPlayerInfo()
{
	player_info_t info;
	g_pIEngine->GetPlayerInfo(EntIndex(), &info);
	return info;
}

bool C_BaseCombatWeapon::CanFire()
{
	static decltype(this) stored_weapon = nullptr;

	static auto stored_tick = 0;

	if (stored_weapon != this || stored_tick >= g_pLocalPlayer->m_nTickBase())
	{
		stored_weapon = this;
		stored_tick = g_pLocalPlayer->m_nTickBase();
		return false; //cannot shoot first tick after switch
	}

	if (IsReloading() || m_iClip1() <= 0 || !g_pLocalPlayer)
		return false;

	auto flServerTime = g_pLocalPlayer->m_nTickBase() * g_pGlobalVars->interval_per_tick;

	return m_flNextPrimaryAttack() <= flServerTime;
}

bool C_BaseCombatWeapon::IsGrenade()
{
	return GetCSWeaponData()->iWeaponType == WEAPONTYPE_GRENADE;
}

bool C_BaseCombatWeapon::IsGun()
{
	switch (GetCSWeaponData()->iWeaponType)
	{
	case WEAPONTYPE_C4:
		return false;
	case WEAPONTYPE_GRENADE:
		return false;
	case WEAPONTYPE_KNIFE:
		return false;
	case WEAPONTYPE_UNKNOWN:
		return false;
	default:
		return true;
	}
}

bool C_BaseCombatWeapon::IsKnife()
{
	if (this->m_Item().m_iItemDefinitionIndex() == WEAPON_ZEUS)
		return false;

	return GetCSWeaponData()->iWeaponType == WEAPONTYPE_KNIFE;
}

bool C_BaseCombatWeapon::IsRifle()
{
	switch (GetCSWeaponData()->iWeaponType)
	{
	case WEAPONTYPE_RIFLE:
		return true;
	case WEAPONTYPE_SUBMACHINEGUN:
		return true;
	case WEAPONTYPE_SHOTGUN:
		return true;
	case WEAPONTYPE_MACHINEGUN:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::IsPistol()
{
	switch (GetCSWeaponData()->iWeaponType)
	{
	case WEAPONTYPE_PISTOL:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::IsSniper()
{
	switch (GetCSWeaponData()->iWeaponType)
	{
	case WEAPONTYPE_SNIPER_RIFLE:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::IsReloading()
{
	static auto inReload = *(uint32_t*)(g_Offsets.FindDownPattern((PCHAR)"\xC6\x87\xFF\xFF\xFF\xFF\xFF\x8B\x06\x8B\xCE\xFF\x90", (PCHAR)"xx?????xxxxxx", g_Offsets.client_panorama.base, g_Offsets.client_panorama.end, 2));
	return *(bool*)((uintptr_t)this + inReload);
}

float C_BaseCombatWeapon::GetInaccuracy()
{
	return CallVFunction<float(__thiscall*)(void*)>(this, 478)(this);
}

float C_BaseCombatWeapon::GetSpread()
{
	return CallVFunction<float(__thiscall*)(void*)>(this, 448)(this);
}

void C_BaseCombatWeapon::UpdateAccuracyPenalty()
{
	CallVFunction<void(__thiscall*)(void*)>(this, 479)(this);
}

bool C_BaseCombatWeapon::HasBullets()
{
	return !IsReloading() && m_iClip1() > 0;
}

CCSWeaponInfo* C_BaseCombatWeapon::GetCSWeaponData()
{
	return CallVFunction<CCSWeaponInfo* (__thiscall*)(void*)>(this, 456)(this);
	/*
	static auto fnGetWpnData
	= reinterpret_cast<CCSWeaponInfo*(__thiscall*)(void*)>(
	Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "55 8B EC 81 EC ? ? ? ? 53 8B D9 56 57 8D 8B")
	);
	return fnGetWpnData(this);*/
}

void C_BasePlayer::InvalidateBoneCache()
{
	static auto InvalidateBoneCache = g_Offsets.FindDownPattern((PCHAR)"\x80\x3D\xFF\xFF\xFF\xFF\xFF\x74\x16\xA1\xFF\xFF\xFF\xFF\x48\xC7\x81", (PCHAR)"xx?????xxx????xxx", g_Offsets.client_panorama.base, g_Offsets.client_panorama.end);

	if (InvalidateBoneCache)
	{
		unsigned long g_iModelBoneCounter = **(unsigned long**)(InvalidateBoneCache + 10);
		*(unsigned int*)((DWORD)this + 0x2924) = 0xFF7FFFFF; // m_flLastBoneSetupTime = -FLT_MAX;
		*(unsigned int*)((DWORD)this + 0x2690) = (g_iModelBoneCounter - 1); // m_iMostRecentModelBoneCounter = g_iModelBoneCounter - 1;
	}
}

void C_BasePlayer::SetAbsOrigin(const Vector &origin)
{
	using SetAbsOriginFn = void(__thiscall*)(void*, const Vector &origin);
	static SetAbsOriginFn SetAbsOrigin = (SetAbsOriginFn)g_Offsets.FindDownPattern((PCHAR)"\x55\x8B\xEC\x83\xE4\xF8\x51\x53\x56\x57\x8B\xF1\xE8", g_Offsets.client_panorama.base, g_Offsets.client_panorama.end);

	if (SetAbsOrigin)
		SetAbsOrigin(this, origin);
}

void C_BasePlayer::SetAbsAngles(const QAngle &angles)
{
	using SetAbsAnglesFn = void(__thiscall*)(void*, const QAngle &angles);
	static SetAbsAnglesFn SetAbsAngles = (SetAbsAnglesFn)g_Offsets.FindDownPattern((PCHAR)"\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x64\x53\x56\x57\x8B\xF1\xE8", g_Offsets.client_panorama.base, g_Offsets.client_panorama.end);

	if (SetAbsAngles)
		SetAbsAngles(this, angles);
}

void C_BasePlayer::UpdateClientSideAnimation()
{
	return CallVFunction<void(__thiscall*)(void*)>(this, 223)(this);
}

IClientRenderable* C_BasePlayer::GetClientRenderable()
{
	return reinterpret_cast<IClientRenderable*>(this + 0x4);
}

matrix3x4_t* C_BasePlayer::GetBoneCache()
{
	auto pRenderable = GetClientRenderable();

	if (pRenderable)
		return *(matrix3x4_t**)((uintptr_t)pRenderable + 10508);

	return nullptr;
}

int* C_BasePlayer::GetBoneCacheCount()
{
	auto pRenderable = GetClientRenderable();

	if (pRenderable)
		return (int*)((uintptr_t)pRenderable + 10520);

	return nullptr;
}

CAnimState *C_BasePlayer::AnimState()
{
	auto ptr = (CAnimState*)(this + 0x3900);

	if (ptr)
		return ptr;

	return nullptr;
}

float C_BasePlayer::GetMaxDesynsAngle()
{
	float max_desync_angle = 0.f;

	auto anim_state = this->AnimState();

	if (!anim_state)
		return max_desync_angle;

	float duck_amount = anim_state->duck_amount;
	float speed_fraction = max(0.f, min(anim_state->feet_speed_forwards_or_sideways, 1.f));
	float speed_factor = max(0.f, min(1.f, anim_state->feet_speed_unknown_forwards_or_sideways));

	float yaw_modifier = (((anim_state->stop_to_full_running_fraction * -0.3f) - 0.2f) * speed_fraction) + 1.0f;

	if (duck_amount > 0.f) 
		yaw_modifier += ((duck_amount * speed_factor) * (0.5f - yaw_modifier));

	max_desync_angle = anim_state->velocity_subtract_y * yaw_modifier;

	g_pIEngine->Con_NPrintf(12, "%f %f %f", duck_amount, speed_factor, speed_fraction, yaw_modifier);

	return max_desync_angle;
	/*auto anim_state = this->AnimState();

	if (!anim_state)
		return 0.f;

	float duck_amount = anim_state->duck_amount;
	float speed_fraction = max(0, min(anim_state->feet_speed_forwards_or_sideways, 1));
	float speed_factor = max(0, min(1, anim_state->feet_speed_unknown_forwards_or_sideways));

	float yaw_modifier = (((anim_state->stop_to_full_running_fraction * -0.3f) - 0.2f) * speed_fraction) + 1.0f;

	if (duck_amount > 0.f)
		yaw_modifier += ((duck_amount * speed_factor) * (0.5f - yaw_modifier));

	return anim_state->velocity_subtract_y * yaw_modifier;*/
}