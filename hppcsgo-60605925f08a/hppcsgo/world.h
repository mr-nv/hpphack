#define DEFAULT_FOV			90
#define DEFAULT_TICKRATE	64.f

class CBaseLocal
{
public:
	CBaseLocal() {}
	CBaseLocal(const CBaseLocal&) = delete;

	bool	m_IsConnected = false;
	bool	m_IsDead = false;
	bool	m_IsScoped = false;

	float	m_flTickRate = DEFAULT_TICKRATE;

	int		m_iEntIndex = 0;
	int		m_iTeamNum = 0;
	int		m_iFOV = DEFAULT_FOV;
	int		m_iObserverMode = 0;

	Vector	m_vecForward;
	Vector	m_vecRight;
	Vector	m_vecUp;
	Vector	m_vecOrigin;
	Vector	m_vecEyePos;
	Vector	m_vecVelocity;
	Vector	m_vecBBMaxs;
	Vector	m_vecBBMins;

	QAngle	m_QViewAngles;
	QAngle	m_QAimPunchAngle;
	QAngle	m_QViewPunchAngle;

	CHandle<C_BasePlayer> m_hObserverTarget;
};

class CLocalInfo
{
public:
	CLocalInfo() {}
	CLocalInfo(const CLocalInfo&) = delete;

	void Update(const CUserCmd* cmd)
	{
		g_Globals.weapon_recoil_scale = 2.0f; //getcvar

		m_BaseLocal.m_IsConnected = g_pIEngine->IsConnected();

		if (!m_BaseLocal.m_IsConnected)
			return;

		m_BaseLocal.m_iEntIndex = g_pLocalPlayer->EntIndex();
		m_BaseLocal.m_iTeamNum = g_pLocalPlayer->m_iTeamNum();
		m_BaseLocal.m_iObserverMode = g_pLocalPlayer->m_iObserverMode();
		m_BaseLocal.m_hObserverTarget = g_pLocalPlayer->m_hObserverTarget();
		m_BaseLocal.m_IsDead = g_pLocalPlayer->m_lifeState() != LIFE_ALIVE;

		const int& iFOV = g_pLocalPlayer->m_iFOV();
		m_BaseLocal.m_iFOV = iFOV > 0 ? iFOV : DEFAULT_FOV;

		if (m_BaseLocal.m_IsDead)
			return;

		m_BaseLocal.m_IsScoped = g_pLocalPlayer->m_bIsScoped();
		m_BaseLocal.m_QViewAngles = cmd->viewangles;
		m_BaseLocal.m_QViewAngles.Normalize();
		m_BaseLocal.m_QViewAngles.AngleVectors(&m_BaseLocal.m_vecForward, &m_BaseLocal.m_vecRight, &m_BaseLocal.m_vecUp);
		m_BaseLocal.m_vecOrigin = g_pLocalPlayer->m_vecOrigin();
		m_BaseLocal.m_vecEyePos = m_BaseLocal.m_vecOrigin + g_pLocalPlayer->m_vecViewOffset();
		m_BaseLocal.m_vecVelocity = g_pLocalPlayer->m_vecVelocity();
		m_BaseLocal.m_QAimPunchAngle = g_pLocalPlayer->m_aimPunchAngle();
		m_BaseLocal.m_QViewPunchAngle = g_pLocalPlayer->m_viewPunchAngle();

		const float interval_per_tick = g_pGlobalVars->interval_per_tick;
		m_BaseLocal.m_flTickRate = interval_per_tick > 0.f ? (1.f / interval_per_tick) : DEFAULT_TICKRATE;

		ICollideable* collideable = g_pLocalPlayer->GetCollideable();
		m_BaseLocal.m_vecBBMaxs = collideable->OBBMaxs();
		m_BaseLocal.m_vecBBMins = collideable->OBBMins();
	}

	void Clear()
	{
		m_BaseLocal.m_IsConnected = false;
		m_BaseLocal.m_IsDead = false;
		m_BaseLocal.m_IsScoped = false;
		m_BaseLocal.m_flTickRate = DEFAULT_TICKRATE;
		m_BaseLocal.m_iEntIndex = 0;
		m_BaseLocal.m_iTeamNum = 0;
		m_BaseLocal.m_iFOV = DEFAULT_FOV;
		m_BaseLocal.m_iObserverMode = OBS_MODE_NONE;
		m_BaseLocal.m_vecForward = Vector(0, 0, 0);
		m_BaseLocal.m_vecRight = Vector(0, 0, 0);
		m_BaseLocal.m_vecUp = Vector(0, 0, 0);
		m_BaseLocal.m_vecOrigin = Vector(0, 0, 0);
		m_BaseLocal.m_vecEyePos = Vector(0, 0, 0);
		m_BaseLocal.m_vecVelocity = Vector(0, 0, 0);
		m_BaseLocal.m_vecBBMaxs = Vector(0, 0, 0);
		m_BaseLocal.m_vecBBMins = Vector(0, 0, 0);
		m_BaseLocal.m_QViewAngles = QAngle(0, 0, 0);
		m_BaseLocal.m_QAimPunchAngle = QAngle(0, 0, 0);
		m_BaseLocal.m_QViewPunchAngle = QAngle(0, 0, 0);
	}

	const CBaseLocal* operator->()
	{
		return &m_BaseLocal;
	}

private:
	CBaseLocal m_BaseLocal;
};

extern CLocalInfo g_Local;

class CHistory
{
public:
	CHistory() {}
	CHistory(const CHistory& history)
	{
		m_iIndex = history.m_iIndex;
		m_iBoneCacheCount = history.m_iBoneCacheCount;
		m_flSimulationTime = history.m_flSimulationTime;
		m_QEyeAngles = history.m_QEyeAngles;
		m_vecOrigin = history.m_vecOrigin;
		m_vecAngles = history.m_vecAngles;

		RtlCopyMemory(m_BoneMatrix, history.m_BoneMatrix, sizeof(history.m_BoneMatrix));
		RtlCopyMemory(m_vecHitbox, history.m_vecHitbox, sizeof(history.m_vecHitbox));
		RtlCopyMemory(m_vecHitboxMin, history.m_vecHitboxMin, sizeof(history.m_vecHitboxMin));
		RtlCopyMemory(m_vecHitboxMax, history.m_vecHitboxMax, sizeof(history.m_vecHitboxMax));
		RtlCopyMemory(m_flHitboxRadius, history.m_flHitboxRadius, sizeof(history.m_flHitboxRadius));
	}

	int		m_iIndex = 0;
	int		m_iBoneCacheCount = 0;

	float	m_flSimulationTime = 0.f;
	float	m_flHitboxRadius[HITBOX_MAX]{ 0.f };

	QAngle	m_QEyeAngles;
	Vector	m_vecOrigin;
	Vector	m_vecAngles;
	Vector	m_vecHitbox[HITBOX_MAX];
	Vector	m_vecHitboxMin[HITBOX_MAX];
	Vector	m_vecHitboxMax[HITBOX_MAX];
	matrix3x4_t m_BoneMatrix[MAXSTUDIOBONES];
};

class CBasePlayer
{
public:
	CBasePlayer() {}
	CBasePlayer(const CBasePlayer&) = delete;

	bool	m_IsDead = false;
	bool	m_IsDormant = false;
	bool	m_IsSpotted = false;
	bool	m_IsGunGameImmunity = false;
	bool	m_IsScoped = false;
	bool	m_IsSoundUpdated = false;

	float	m_flHistory = 0.f;
	float	m_flHitboxRadius[HITBOX_MAX]{ 0.f };

	int		m_iTeamNum = 0;
	int		m_iArmorValue = 0;
	int		m_iHealth = 0;
	int		m_iWeaponId = WEAPON_INVALID;
	int		m_iWeaponClip1 = 0;
	int		m_iBoneCacheCount = 0;

	Vector	m_vecOrigin;
	Vector	m_vecBBMaxs;
	Vector	m_vecBBMins;
	Vector	m_vecHitbox[HITBOX_MAX];
	Vector	m_vecHitboxMin[HITBOX_MAX];
	Vector	m_vecHitboxMax[HITBOX_MAX];

	matrix3x4_t m_BoneMatrix[MAXSTUDIOBONES];

	CCSWeaponInfo* m_CSWeaponData = nullptr;
	C_BaseCombatWeapon* m_hActiveWeapon = nullptr;
};

class CPlayerInfo
{
public:
	CPlayerInfo() {}
	CPlayerInfo(const CPlayerInfo&) = delete;

	void Update()
	{
		for (int i = 1; i <= g_pIEngine->GetMaxClients(); i++)
		{
			if (g_Local->m_iEntIndex != i)
				UpdatePlayerByIndex(i);
		}

		static auto prev_time = g_pGlobalVars->curtime;

		//if (cvar.visuals.active && cvar.visuals.shared_esp && fabs(prev_time - g_pGlobalVars->curtime) > 0.1f)
		//{
		//	const std::string server_ip = g_pIEngine->GetNetChannelInfo()->GetAddress();

		//	if (server_ip.find("loopback") == std::string::npos) // true?
		//	{
		//		std::stringstream ssmsg;
		//		ssmsg << "shared>";
		//		ssmsg << server_ip << ">";

		//		auto players_for_send = 0;

		//		for (int i = 1; i <= MAX_CLIENTS; i++)
		//		{
		//			if (g_Local->m_iEntIndex == i)
		//				continue;

		//			auto pBasePlayer = C_BasePlayer::GetPlayerByIndex(i);

		//			if (!g_Utils.IsValidPlayer(pBasePlayer, true))
		//				continue;


		//		//	if (m_BasePlayer[i].m_IsDead)
		//			//	continue;

		//			//if (m_BasePlayer[i].m_IsDormant)
		//			//	continue;

		//			ssmsg << i << ";";
		//			ssmsg << pBasePlayer->m_vecOrigin().x << ";";
		//			ssmsg << pBasePlayer->m_vecOrigin().y << ";";
		//			ssmsg << pBasePlayer->m_vecOrigin().z << "^";

		//			players_for_send++;
		//		}

		//		if (players_for_send)
		//		{
		//			g_pICvar->ConsolePrintf("sent %s\n", ssmsg.str());
		//			websocket->send(ssmsg.str());
		//			prev_time = g_pGlobalVars->curtime;
		//		}
		//	}
		//}
	}

	void Clear()
	{
		for (size_t i = 0; i < MAX_CLIENTS - 1; i++)
		{
			m_BasePlayer[i].m_IsDead = false;
			m_BasePlayer[i].m_IsDormant = false;
			m_BasePlayer[i].m_IsSpotted = false;
			m_BasePlayer[i].m_IsGunGameImmunity = false;
			m_BasePlayer[i].m_IsScoped = false;
			m_BasePlayer[i].m_IsSoundUpdated = false;
			m_BasePlayer[i].m_flHistory = 0.f;
			m_BasePlayer[i].m_iTeamNum = 0;
			m_BasePlayer[i].m_iArmorValue = 0;
			m_BasePlayer[i].m_iHealth = 0;
			m_BasePlayer[i].m_iWeaponId = WEAPON_INVALID;
			m_BasePlayer[i].m_iWeaponClip1 = 0;
			m_BasePlayer[i].m_iBoneCacheCount = 0;
			m_BasePlayer[i].m_vecOrigin = Vector(0, 0, 0);
			m_BasePlayer[i].m_vecBBMaxs = Vector(0, 0, 0);
			m_BasePlayer[i].m_vecBBMins = Vector(0, 0, 0);

			RtlSecureZeroMemory(m_BasePlayer[i].m_vecHitbox, sizeof(m_BasePlayer[i].m_vecHitbox));
			RtlSecureZeroMemory(m_BasePlayer[i].m_vecHitboxMin, sizeof(m_BasePlayer[i].m_vecHitboxMin));
			RtlSecureZeroMemory(m_BasePlayer[i].m_vecHitboxMax, sizeof(m_BasePlayer[i].m_vecHitboxMax));
			RtlSecureZeroMemory(m_BasePlayer[i].m_flHitboxRadius, sizeof(m_BasePlayer[i].m_flHitboxRadius));
			RtlSecureZeroMemory(m_BasePlayer[i].m_BoneMatrix, sizeof(m_BasePlayer[i].m_BoneMatrix));

			m_BasePlayer[i].m_CSWeaponData = nullptr;
			m_BasePlayer[i].m_hActiveWeapon = nullptr;
		}
	}

	std::deque<CHistory>& GetHistory(int index)
	{
		return m_aHistory[index];
	}

	void UpdateOrigin(const Vector& origin, int index, bool is_sound)
	{
		index = GetOffsetIndex(index);

		if (m_BasePlayer[index].m_IsDormant)
		{
			m_BasePlayer[index].m_vecOrigin = origin;
			m_BasePlayer[index].m_flHistory = g_pGlobalVars->curtime;
			m_BasePlayer[index].m_IsSoundUpdated = is_sound;
		}
	}
	
	const CBasePlayer* operator[](int index)
	{
		return &m_BasePlayer[GetOffsetIndex(index)];
	}

private:
	CBasePlayer m_BasePlayer[MAX_CLIENTS - 1];
	std::deque<CHistory> m_aHistory[MAX_CLIENTS - 1];

	int GetOffsetIndex(int index)
	{
		return index < g_Local->m_iEntIndex ? index - 1 : index - 2;
	}

	void GetHitboxInfo(C_BasePlayer* pPlayer, int index)
	{
		m_BasePlayer[index].m_iBoneCacheCount = 0;

		auto* pStudioModel = g_pIModelInfo->GetStudiomodel(pPlayer->GetModel());

		if (pStudioModel == nullptr)
			return;

		auto pHitboxSet = pStudioModel->GetHitboxSet(0);

		if (!pHitboxSet)
			return;

		static matrix3x4_t BoneMatrix[MAXSTUDIOBONES];

		if (!pPlayer->SetupBones(BoneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.f))
			return;

		//RtlCopyMemory(m_BasePlayer[index].m_BoneMatrix, BoneMatrix, sizeof(BoneMatrix));

		for (auto y = 0; y < MAXSTUDIOBONES; y++)
		{
			g_Utils.MatrixCopy(BoneMatrix[y], m_BasePlayer[index].m_BoneMatrix[y]);
		}

		auto pBoneCacheCount = pPlayer->GetBoneCacheCount();

		if (pBoneCacheCount)
			m_BasePlayer[index].m_iBoneCacheCount = *pBoneCacheCount;

		Vector vecBoundBoxMin, vecBoundBoxMax;

		for (int i = 0; i < pHitboxSet->numhitboxes; i++)
		{
			auto* pHitbox = pHitboxSet->GetHitbox(i);

			if (pHitbox == nullptr)
				return;

			g_Utils.VectorTransform(pHitbox->bbmin, BoneMatrix[pHitbox->bone], vecBoundBoxMin);
			g_Utils.VectorTransform(pHitbox->bbmax, BoneMatrix[pHitbox->bone], vecBoundBoxMax);

			m_BasePlayer[index].m_flHitboxRadius[i] = pHitbox->m_flRadius;
			m_BasePlayer[index].m_vecHitbox[i] = (vecBoundBoxMin + vecBoundBoxMax) * 0.5f;
			m_BasePlayer[index].m_vecHitboxMin[i] = vecBoundBoxMin;
			m_BasePlayer[index].m_vecHitboxMax[i] = vecBoundBoxMax;
		}
	}

	void GetWeaponInfo(C_BasePlayer* pPlayer, int index)
	{
		m_BasePlayer[index].m_hActiveWeapon = pPlayer->m_hActiveWeapon().Get();

		if (m_BasePlayer[index].m_hActiveWeapon == nullptr)
			return;

		m_BasePlayer[index].m_CSWeaponData = m_BasePlayer[index].m_hActiveWeapon->GetCSWeaponData();
		m_BasePlayer[index].m_iWeaponId = m_BasePlayer[index].m_hActiveWeapon->m_Item().m_iItemDefinitionIndex();
		m_BasePlayer[index].m_iWeaponClip1 = m_BasePlayer[index].m_hActiveWeapon->m_iClip1();
	}

	void UpdatePlayerByIndex(int index)
	{
		C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex(index);

		if (pPlayer == nullptr)
			return;

		index = GetOffsetIndex(index);

		m_BasePlayer[index].m_iTeamNum = pPlayer->m_iTeamNum();
		m_BasePlayer[index].m_IsDead = pPlayer->m_lifeState() != LIFE_ALIVE;

		if (m_BasePlayer[index].m_IsDead)
			return;

		m_BasePlayer[index].m_IsSpotted = pPlayer->m_bSpotted();
		m_BasePlayer[index].m_IsDormant = pPlayer->IsDormant();

		if (m_BasePlayer[index].m_IsDormant)
			return;

		m_BasePlayer[index].m_IsSoundUpdated = false;
		m_BasePlayer[index].m_IsGunGameImmunity = pPlayer->m_bGunGameImmunity();
		m_BasePlayer[index].m_IsScoped = pPlayer->m_bIsScoped();
		m_BasePlayer[index].m_iArmorValue = pPlayer->m_ArmorValue();
		m_BasePlayer[index].m_iHealth = pPlayer->m_iHealth();
		m_BasePlayer[index].m_flHistory = g_pGlobalVars->curtime;
		m_BasePlayer[index].m_vecOrigin = pPlayer->m_vecOrigin();

		auto pCollideable = pPlayer->GetCollideable();
		m_BasePlayer[index].m_vecBBMaxs = pCollideable ? pCollideable->OBBMaxs() : Vector(0, 0, 0);
		m_BasePlayer[index].m_vecBBMins = pCollideable ? pCollideable->OBBMins() : Vector(0, 0, 0);

		GetHitboxInfo(pPlayer, index);
		GetWeaponInfo(pPlayer, index);
	}
};

extern CPlayerInfo g_Player;

class CBaseWeapon
{
public:
	using string = std::string;

	CBaseWeapon() {}
	CBaseWeapon(const CBaseWeapon&) = delete;

	bool	m_IsValid = false;
	bool	m_IsCanFire = false;
	bool	m_IsReloading = false;

	float	m_flRecoilIndex = 0.f;
	float	m_flInaccuracy = 0.f;
	float	m_flSpread = 0.f;
	float	m_flRange = 0.f;
	float	m_flPostponeFireReadyTime = 0.f;

	int		m_iCurrentId = WEAPON_INVALID;
	int		m_iCurrentType = WEAPONTYPE_KNIFE;
	int		m_iClip1 = 0;
	int		m_iClip2 = 0;
	int		m_iShotsFired = 0;

	string	m_sWeaponName;

	CCSWeaponInfo* m_pWeaponData = nullptr;
};

class CWeaponInfo
{
public:
	CWeaponInfo() {}
	CWeaponInfo(const CWeaponInfo&) = delete;

	void Update()
	{
		m_BaseWeapon.m_iShotsFired = g_pLocalPlayer->m_iShotsFired();

		C_BaseCombatWeapon* pWeapon = g_pLocalPlayer->m_hActiveWeapon().Get();

		if (pWeapon == nullptr)
		{
			m_BaseWeapon.m_IsValid = false;
			return;
		}

		pWeapon->UpdateAccuracyPenalty();

		m_BaseWeapon.m_IsCanFire = pWeapon->CanFire();
		m_BaseWeapon.m_IsReloading = pWeapon->IsReloading();
		m_BaseWeapon.m_flRecoilIndex = pWeapon->m_flRecoilIndex();
		m_BaseWeapon.m_flPostponeFireReadyTime = pWeapon->m_flPostponeFireReadyTime();
		m_BaseWeapon.m_flInaccuracy = pWeapon->GetInaccuracy();
		m_BaseWeapon.m_flSpread = pWeapon->GetSpread();
		m_BaseWeapon.m_iCurrentId = pWeapon->m_Item().m_iItemDefinitionIndex();
		m_BaseWeapon.m_iClip1 = pWeapon->m_iClip1();
		m_BaseWeapon.m_iClip2 = pWeapon->m_iClip2();
		m_BaseWeapon.m_sWeaponName = GetWeaponNameByIndex(m_BaseWeapon.m_iCurrentId);

		CCSWeaponInfo* pWeaponData = pWeapon->GetCSWeaponData();

		if (pWeaponData == nullptr)
		{
			m_BaseWeapon.m_IsValid = false;
			return;
		}

		m_BaseWeapon.m_pWeaponData = pWeaponData;
		m_BaseWeapon.m_iCurrentType = pWeaponData->iWeaponType;
		m_BaseWeapon.m_flRange = pWeaponData->flRange;
		m_BaseWeapon.m_IsValid = true;
	}

	void Clear()
	{
		m_BaseWeapon.m_IsValid = false;
		m_BaseWeapon.m_IsCanFire = false;
		m_BaseWeapon.m_IsReloading = false;
		m_BaseWeapon.m_flRecoilIndex = 0.f;
		m_BaseWeapon.m_flInaccuracy = 0.f;
		m_BaseWeapon.m_flSpread = 0.f;
		m_BaseWeapon.m_flRange = 0.f;
		m_BaseWeapon.m_iCurrentId = WEAPON_INVALID;
		m_BaseWeapon.m_iCurrentType = 0;
		m_BaseWeapon.m_iClip1 = 0;
		m_BaseWeapon.m_iClip2 = 0;
		m_BaseWeapon.m_iShotsFired = 0;
		m_BaseWeapon.m_sWeaponName.clear();
		m_BaseWeapon.m_pWeaponData = nullptr;
	}

	const CBaseWeapon* operator->()
	{
		return &m_BaseWeapon;
	}

	bool IsGrenade()
	{
		return m_BaseWeapon.m_iCurrentType == WEAPONTYPE_GRENADE;
	}

	bool IsGun()
	{
		switch (m_BaseWeapon.m_iCurrentType)
		{
		case WEAPONTYPE_C4:
		case WEAPONTYPE_GRENADE:
		case WEAPONTYPE_KNIFE:
		case WEAPONTYPE_UNKNOWN:
			return false;
		}

		return true;
	}

	bool IsKnife()
	{
		if (m_BaseWeapon.m_iCurrentId == WEAPON_ZEUS)
			return false;

		return m_BaseWeapon.m_iCurrentType == WEAPONTYPE_KNIFE;
	}

	bool IsRifle()
	{
		switch (m_BaseWeapon.m_iCurrentType)
		{
		case WEAPONTYPE_RIFLE:
		case WEAPONTYPE_SUBMACHINEGUN:
		case WEAPONTYPE_SHOTGUN:
		case WEAPONTYPE_MACHINEGUN:
			return true;
		}

		return false;
	}

	bool IsPistol()
	{
		return m_BaseWeapon.m_iCurrentType == WEAPONTYPE_PISTOL;
	}

	bool IsSniper()
	{
		return m_BaseWeapon.m_iCurrentType == WEAPONTYPE_SNIPER_RIFLE;
	}

	bool IsShotgun()
	{
		return m_BaseWeapon.m_iCurrentType == WEAPONTYPE_SHOTGUN;
	}

	bool HasScope()
	{
		switch (m_BaseWeapon.m_iCurrentType)
		{
		case WEAPON_G3SG1:
		case WEAPON_SCAR20:
		case WEAPON_AWP:
		case WEAPON_AUG:
		case WEAPON_SG553:
		case WEAPON_SSG08:
			return true;
		}

		return false;
	}

	bool HasSniperScope()
	{
		switch (m_BaseWeapon.m_iCurrentType)
		{
		case WEAPON_G3SG1:
		case WEAPON_SCAR20:
		case WEAPON_AWP:
		case WEAPON_SSG08:
			return true;
		}

		return false;
	}

private:
	CBaseWeapon m_BaseWeapon;
};

extern CWeaponInfo g_Weapon;