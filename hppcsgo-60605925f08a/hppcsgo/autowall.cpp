#include "main.h"

CAutoWall g_AutoWall;

inline bool CGameTrace::DidHitWorld() const
{
	return hit_entity->EntIndex() == 0;
}

inline bool CGameTrace::DidHitNonWorldEntity() const
{
	return hit_entity != NULL && !DidHitWorld();
}

void CAutoWall::TraceLine(Vector & absStart, Vector & absEnd, unsigned int mask, IClientEntity* ignore, CGameTrace* ptr)
{
	Ray_t ray;
	ray.Init(absStart, absEnd);
	CTraceFilter filter;
	filter.pSkip = ignore;

	g_pIEngineTrace->TraceRay(ray, mask, &filter, ptr);
}

void CAutoWall::ClipTraceToPlayers(const Vector& absStart, const Vector absEnd, unsigned int mask, ITraceFilter* filter, CGameTrace* tr)
{
	static auto ClipTraceToPlayers_call = g_Offsets.FindDownPattern((PCHAR)"\x53\x8B\xDC\x83\xEC\x08\x83\xE4\xF0\x83\xC4\x04\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x81\xEC\xFF\xFF\xFF\xFF\x8B\x43\x10", (PCHAR)"xxxxxxxxxxxxxxxxxxxxxxxxx????xxx", g_Offsets.client_panorama.base, g_Offsets.client_panorama.end);

	if (!ClipTraceToPlayers_call)
		return;

	_asm
	{
		MOV		EAX, filter
		LEA		ECX, tr
		PUSH	ECX
		PUSH	EAX
		PUSH	mask
		LEA		EDX, absEnd
		LEA		ECX, absStart
		CALL	ClipTraceToPlayers_call
		ADD		ESP, 0xC
	}
}

void CAutoWall::GetBulletTypeParameters(float& maxRange, float& maxDistance, char* bulletType, bool sv_penetration_type)
{
	if (sv_penetration_type)
	{
		maxRange = 35.0;
		maxDistance = 3000.0;
	}
	else
	{
		//Play tribune to framerate. Thanks, stringcompare
		//Regardless I doubt anyone will use the old penetration system anyway; so it won't matter much.
		if (!strcmp(bulletType, ("BULLET_PLAYER_338MAG")))
		{
			maxRange = 45.0;
			maxDistance = 8000.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_762MM")))
		{
			maxRange = 39.0;
			maxDistance = 5000.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_556MM")) || !strcmp(bulletType, ("BULLET_PLAYER_556MM_SMALL")) || !strcmp(bulletType, ("BULLET_PLAYER_556MM_BOX")))
		{
			maxRange = 35.0;
			maxDistance = 4000.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_57MM")))
		{
			maxRange = 30.0;
			maxDistance = 2000.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_50AE")))
		{
			maxRange = 30.0;
			maxDistance = 1000.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_357SIG")) || !strcmp(bulletType, ("BULLET_PLAYER_357SIG_SMALL")) || !strcmp(bulletType, ("BULLET_PLAYER_357SIG_P250")) || !strcmp(bulletType, ("BULLET_PLAYER_357SIG_MIN")))
		{
			maxRange = 25.0;
			maxDistance = 800.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_9MM")))
		{
			maxRange = 21.0;
			maxDistance = 800.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_45ACP")))
		{
			maxRange = 15.0;
			maxDistance = 500.0;
		}
		if (!strcmp(bulletType, ("BULLET_PLAYER_BUCKSHOT")))
		{
			maxRange = 0.0;
			maxDistance = 0.0;
		}
	}
}

bool CAutoWall::BreakableEntity(IClientEntity* entity)
{
	const auto class_ptr = (ClientClass*)entity->GetClientClass();

	if (!class_ptr)
		return false;

	if (class_ptr == nullptr)
		return false;

	return class_ptr->m_ClassID == ClassId::ClassId_CBreakableProp || class_ptr->m_ClassID == ClassId::ClassId_CBreakableSurface;
}

bool CAutoWall::IsArmored(C_BasePlayer *player, CGameTrace *enterTrace)
{
	switch (enterTrace->hitgroup)
	{
	case HITGROUP_HEAD:
		return player->m_bHasHelmet();
	case HITGROUP_GENERIC:
	case HITGROUP_CHEST:
	case HITGROUP_STOMACH:
	case HITGROUP_LEFTARM:
	case HITGROUP_RIGHTARM:
		return true;
	default:
		return false;
	}
};

void CAutoWall::ScaleDamage(CGameTrace &enterTrace, CCSWeaponInfo *weaponData, float& currentDamage)
{
	const auto player = (C_BasePlayer*)enterTrace.hit_entity;

	if (!player)
		return;

	bool hasHeavyArmor = player->m_bHasHeavyArmor();
	int armorValue = player->m_ArmorValue();

	switch (enterTrace.hitgroup)
	{
	case HITGROUP_HEAD:
		currentDamage *= hasHeavyArmor ? 2.f : 4.f; //Heavy Armor does 1/2 damage
		break;
	case HITGROUP_STOMACH:
		currentDamage *= 1.25f;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		currentDamage *= 0.75f;
		break;
	default:
		break;
	}

	if (armorValue > 0 && IsArmored(player, &enterTrace))
	{
		float bonusValue = 1.f, armorBonusRatio = 0.5f, armorRatio = weaponData->flArmorRatio / 2.f;

		//Damage gets modified for heavy armor users
		if (hasHeavyArmor)
		{
			armorBonusRatio = 0.33f;
			armorRatio *= 0.5f;
			bonusValue = 0.33f;
		}

		auto NewDamage = currentDamage * armorRatio;

		if (hasHeavyArmor)
			NewDamage *= 0.85f;

		if (((currentDamage - (currentDamage * armorRatio)) * (bonusValue * armorBonusRatio)) > armorValue)
			NewDamage = currentDamage - (armorValue / armorBonusRatio);

		currentDamage = NewDamage;
	}
}

bool CAutoWall::TraceToExit(CGameTrace& enterTrace, CGameTrace& exitTrace, Vector startPosition, Vector direction)
{
	Vector start, end;
	float maxDistance = 90.f, rayExtension = 4.f, currentDistance = 0;
	int firstContents = 0;

	while (currentDistance <= maxDistance)
	{
		currentDistance += rayExtension;

		start = startPosition + direction * currentDistance;

		if (!firstContents)
			firstContents = g_pIEngineTrace->GetPointContents(start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);

		int pointContents = g_pIEngineTrace->GetPointContents(start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);

		if (!(pointContents & MASK_SHOT_HULL) || pointContents & CONTENTS_HITBOX && pointContents != firstContents)
		{
			end = start - (direction * rayExtension);

			TraceLine(start, end, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, &exitTrace);

			if (exitTrace.startsolid && exitTrace.surface.flags & SURF_HITBOX)
			{
				TraceLine(start, startPosition, MASK_SHOT_HULL, (IClientEntity*)exitTrace.hit_entity, &exitTrace);

				if (exitTrace.DidHit() && !exitTrace.startsolid)
				{
					start = exitTrace.endpos;
					return true;
				}

				continue;
			}

			if (exitTrace.DidHit() && !exitTrace.startsolid)
			{
				if (BreakableEntity((IClientEntity*)enterTrace.hit_entity) && BreakableEntity((IClientEntity*)exitTrace.hit_entity))
					return true;

				if (enterTrace.surface.flags & SURF_NODRAW || !(exitTrace.surface.flags & SURF_NODRAW) && (exitTrace.plane.normal.Dot(direction) <= 1.f))
				{
					float multAmount = exitTrace.fraction * 4.f;
					start -= direction * multAmount;
					return true;
				}

				continue;
			}

			if (!exitTrace.DidHit() || exitTrace.startsolid)
			{
				if (enterTrace.DidHitNonWorldEntity() && BreakableEntity((IClientEntity*)enterTrace.hit_entity))
				{
					exitTrace.startpos = enterTrace.startpos;
					exitTrace.endpos = start + direction;
					exitTrace.plane = enterTrace.plane;
					exitTrace.fraction = enterTrace.fraction;
					exitTrace.contents = enterTrace.contents;
					exitTrace.dispFlags = enterTrace.dispFlags;
					exitTrace.allsolid = enterTrace.allsolid;
					exitTrace.startsolid = enterTrace.startsolid;
					exitTrace.fractionleftsolid = enterTrace.fractionleftsolid;
					exitTrace.surface = enterTrace.surface;
					exitTrace.hitgroup = enterTrace.hitgroup;
					exitTrace.physicsbone = enterTrace.physicsbone;
					exitTrace.worldSurfaceIndex = enterTrace.worldSurfaceIndex;
					exitTrace.hit_entity = enterTrace.hit_entity;
					exitTrace.hitbox = enterTrace.hitbox;
					return true;
				}

				continue;
			}
		}
	}

	return false;
}

bool CAutoWall::HandleBulletPenetration(CCSWeaponInfo* weaponData, CGameTrace& enterTrace, Vector& eyePosition, Vector direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration)
{
	//Because there's been issues regarding this- putting this here.
	if (&currentDamage == nullptr)
		return false;

	CGameTrace exitTrace;

	//auto pEnemy = (C_BaseEntity*)enterTrace.hit_entity;

	auto enterSurfaceData = g_pIPhysSurface->GetSurfaceData(enterTrace.surface.surfaceProps);

	int enterMaterial = enterSurfaceData->game.material;

	float enterSurfPenetrationModifier = enterSurfaceData->game.flPenetrationModifier;
	float enterDamageModifier = enterSurfaceData->game.flDamageModifier;
	float thickness, modifier, lostDamage, finalDamageModifier, combinedPenetrationModifier;
	bool isSolidSurf = ((enterTrace.contents >> 3) & CONTENTS_SOLID);
	bool isLightSurf = ((enterTrace.surface.flags >> 7) & SURF_LIGHT);

	if (possibleHitsRemaining <= 0
		|| (enterTrace.surface.name == (const char*)0x2227c261 && exitTrace.surface.name == (const char*)0x2227c868)
		|| (!possibleHitsRemaining && !isLightSurf && !isSolidSurf && enterMaterial != CHAR_TEX_GRATE && enterMaterial != CHAR_TEX_GLASS)
		|| weaponData->flPenetration <= 0.f
		|| !TraceToExit(enterTrace, exitTrace, enterTrace.endpos, direction)
		&& !(g_pIEngineTrace->GetPointContents(enterTrace.endpos, MASK_SHOT_HULL, nullptr) & MASK_SHOT_HULL))
	{
		return false;
	}

	const auto exitSurfaceData = g_pIPhysSurface->GetSurfaceData(exitTrace.surface.surfaceProps);
	int exitMaterial = exitSurfaceData->game.material;
	float exitSurfPenetrationModifier = exitSurfaceData->game.flPenetrationModifier;
	float exitDamageModifier = exitSurfaceData->game.flDamageModifier;

	if (sv_penetration_type)
	{
		if (enterMaterial == CHAR_TEX_GRATE || enterMaterial == CHAR_TEX_GLASS)
		{
			combinedPenetrationModifier = 3.f;
			finalDamageModifier = 0.05f;
		}
		else if (isSolidSurf || isLightSurf)
		{
			combinedPenetrationModifier = 1.f;
			finalDamageModifier = 0.16f;
		}
		else if (enterMaterial == CHAR_TEX_FLESH /*&& (g_pLocalPlayer->m_iTeamNum() == pEnemy->m_iTeamNum() && ff_damage_reduction_bullets == 0.f)*/)
		{
			if (ff_damage_bullet_penetration == 0.f)
				return false;

			combinedPenetrationModifier = ff_damage_bullet_penetration;
			finalDamageModifier = 0.16f;
		}
		else
		{
			combinedPenetrationModifier = (enterSurfPenetrationModifier + exitSurfPenetrationModifier) / 2.f;
			finalDamageModifier = 0.16f;
		}

		if (enterMaterial == exitMaterial)
		{
			if (exitMaterial == CHAR_TEX_CARDBOARD || exitMaterial == CHAR_TEX_WOOD)
				combinedPenetrationModifier = 3.f;
			else if (exitMaterial == CHAR_TEX_PLASTIC)
				combinedPenetrationModifier = 2.f;
		}

		thickness = (exitTrace.endpos - enterTrace.endpos).LengthSqr();
		modifier = fmaxf(1.f / combinedPenetrationModifier, 0.f);

		lostDamage = fmaxf(
			((modifier * thickness) / 24.f)
			+ ((currentDamage * finalDamageModifier)
				+ (fmaxf(3.75f / penetrationPower, 0.f) * 3.f * modifier)), 0.f);

		if (lostDamage > currentDamage)
			return false;

		if (lostDamage > 0.f)
			currentDamage -= lostDamage;

		if (currentDamage < 1.f)
			return false;

		eyePosition = exitTrace.endpos;
		--possibleHitsRemaining;

		return true;
	}
	else
	{
		combinedPenetrationModifier = 1.f;

		if (isSolidSurf || isLightSurf)
			finalDamageModifier = 0.99f;
		else
		{
			finalDamageModifier = fminf(enterDamageModifier, exitDamageModifier);
			combinedPenetrationModifier = fminf(enterSurfPenetrationModifier, exitSurfPenetrationModifier);
		}

		if (enterMaterial == exitMaterial && (exitMaterial == CHAR_TEX_METAL || exitMaterial == CHAR_TEX_WOOD))
			combinedPenetrationModifier += combinedPenetrationModifier;

		thickness = (exitTrace.endpos - enterTrace.endpos).LengthSqr();

		if (sqrt(thickness) <= combinedPenetrationModifier * penetrationPower)
		{
			currentDamage *= finalDamageModifier;
			eyePosition = exitTrace.endpos;
			--possibleHitsRemaining;
			return true;
		}
		return false;
	}
}

bool CAutoWall::FireBullet(C_BaseCombatWeapon* pWeapon, Vector& direction, float& currentDamage, CGameTrace &final_trace)
{
	if (!pWeapon)
		return false;

	auto weapon_data = pWeapon->GetCSWeaponData();

	if (!weapon_data)
		return false;

	static auto penetrationSystem = g_pICvar->FindVar(("sv_penetration_type"));
	static auto damageReductionBullets = g_pICvar->FindVar(("ff_damage_reduction_bullets"));
	static auto damageBulletPenetration = g_pICvar->FindVar(("ff_damage_bullet_penetration"));

	if (!penetrationSystem || !damageReductionBullets || !damageBulletPenetration)
		return false;

	auto sv_penetration_type = penetrationSystem->GetBool();
	auto ff_damage_reduction_bullets = damageReductionBullets->GetFloat();
	auto ff_damage_bullet_penetration = damageBulletPenetration->GetFloat();

	float currentDistance = 0.f, penetrationPower, penetrationDistance, maxRange = weapon_data->flRange, rayExtension = 40.f;

	GetBulletTypeParameters(penetrationPower, penetrationDistance, weapon_data->szBulletType, sv_penetration_type);

	if (sv_penetration_type)
		penetrationPower = weapon_data->flPenetration;

	int possibleHitsRemaining = 4;

	currentDamage = weapon_data->iDamage;

	Vector vecEye = g_Local->m_vecEyePos;

	CGameTrace enterTrace;
	CTraceFilter filter;

	filter.pSkip = g_pLocalPlayer;

	while (possibleHitsRemaining > 0 && currentDamage >= 1.f)
	{
		maxRange -= currentDistance;

		Vector end = vecEye + direction * maxRange;

		TraceLine(vecEye, end, MASK_SHOT_HULL | CONTENTS_HITBOX, (IClientEntity*)g_pLocalPlayer, &enterTrace);
		ClipTraceToPlayers(vecEye, end + direction * rayExtension, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &enterTrace); //  | CONTENTS_HITBOX
		
		auto enterSurfaceData = g_pIPhysSurface->GetSurfaceData(enterTrace.surface.surfaceProps);

		float enterSurfPenetrationModifier = enterSurfaceData->game.flPenetrationModifier;

		//int enterMaterial = enterSurfaceData->game.material;

		if (enterTrace.fraction == 1.f)
			break;

		currentDistance += enterTrace.fraction * maxRange;

		currentDamage *= pow(weapon_data->flRangeModifier, (currentDistance / 500.f));

		if (currentDistance > penetrationDistance && weapon_data->flPenetration > 0.f || enterSurfPenetrationModifier < 0.1f)
			break;

		bool canDoDamage = (enterTrace.hitgroup != HITGROUP_GEAR && enterTrace.hitgroup != HITGROUP_GENERIC);

		if (canDoDamage)
		{
			final_trace.startpos = enterTrace.startpos;
			final_trace.endpos = enterTrace.endpos;
			final_trace.plane = enterTrace.plane;
			final_trace.fraction = enterTrace.fraction;
			final_trace.contents = enterTrace.contents;
			final_trace.dispFlags = enterTrace.dispFlags;
			final_trace.allsolid = enterTrace.allsolid;
			final_trace.startsolid = enterTrace.startsolid;
			final_trace.fractionleftsolid = enterTrace.fractionleftsolid;
			final_trace.surface = enterTrace.surface;
			final_trace.hitgroup = enterTrace.hitgroup;
			final_trace.physicsbone = enterTrace.physicsbone;
			final_trace.worldSurfaceIndex = enterTrace.worldSurfaceIndex;
			final_trace.hit_entity = enterTrace.hit_entity;
			final_trace.hitbox = enterTrace.hitbox;

			ScaleDamage(enterTrace, weapon_data, currentDamage);
			return true;
		}

		if (!HandleBulletPenetration(weapon_data, enterTrace, vecEye, direction, possibleHitsRemaining, currentDamage, penetrationPower, sv_penetration_type, ff_damage_reduction_bullets, ff_damage_bullet_penetration))
			break;
	}

	return false;
}

float CAutoWall::Penetrate(const Vector &vecPoint, CGameTrace &final_trace)
{
	if (g_Local->m_IsDead)
		return -1.f;

	QAngle QAngles;
	Vector vecDirection;

	g_Utils.VectorAngles(vecPoint - g_Local->m_vecEyePos, QAngles);

	QAngles.AngleVectors(&vecDirection, 0, 0);

	vecDirection.Normalize();

	float flCurrentDamage = 0.f;

	if (FireBullet(g_pLocalPlayer->m_hActiveWeapon(), vecDirection, flCurrentDamage, final_trace))
		return flCurrentDamage;

	return -1.f;
}