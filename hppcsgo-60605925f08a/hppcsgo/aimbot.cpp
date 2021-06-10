#include "main.h"

CAimbot g_Aimbot;

std::deque<C_BasePlayer*> players_for_restore;

float GetHitChance(const Vector& point)
{
	if (!g_Weapon->m_IsValid)
		return false;

	auto spread_cone = g_Weapon->m_flInaccuracy * 256.f / DirectX::XM_PI + g_Weapon->m_flSpread;

	auto a = (point - g_pLocalPlayer->GetEyePos()).Length();
	auto b = sqrt(tan(spread_cone * DirectX::XM_PI / 180.f) * a);

	/*g_pIEngine->Con_NPrintf(1, "a: %f",a );
	g_pIEngine->Con_NPrintf(2, "b: %f", b);
	g_pIEngine->Con_NPrintf(3, "spread_cone: %f", spread_cone);*/

	if (2.2f > b)
		return 100.0f;

	//g_pIEngine->Con_NPrintf(4, "kek: %f", (2.2f / fmax(b, 2.2f)) * 100.0f);

	return (2.2f / fmax(b, 2.2f)) * 100.0f;
}

void CAimbot::Run(CUserCmd* cmd)
{
	if (!g_Weapon->m_IsValid || !g_Weapon.IsGun() || g_Weapon->m_iCurrentId == -1)
		return;

	if (g_Weapon->m_IsReloading)
		return;

	if (g_Weapon->m_iClip1 < 1)
		return;

	if (cvar.legitbot.active)
	{
		if (cvar.legitbot.smoke_check)
		{
			Vector vecForward;
			QAngle QAngles(cmd->viewangles);
			QAngles.x += g_Local->m_QAimPunchAngle.x * g_Globals.weapon_recoil_scale;
			QAngles.y += g_Local->m_QAimPunchAngle.y * g_Globals.weapon_recoil_scale;
			QAngles.Normalize();
			QAngles.AngleVectors(&vecForward, NULL, NULL);

			Vector vecEnd = g_Local->m_vecEyePos + vecForward * g_Weapon->m_flRange;

			if (g_Utils.IsBehindSmoke(g_Local->m_vecEyePos, vecEnd))
				return;
		}

		_Trigger(cmd);
		_LegitBot(cmd);
	}
	else if (cvar.ragebot.active)
	{
		_RageBot(cmd);
	}

	if (!players_for_restore.empty())
	{
		for (auto player : players_for_restore)
			g_LagComp.Restore(player);

		players_for_restore.clear();
	}
}

void CAimbot::_RageBot(CUserCmd* cmd)
{
	const auto weaponid = g_Weapon->m_iCurrentId;

	if (!cvar.ragebot.weapon[weaponid].aim)
		return;

	if (!g_Weapon->m_IsCanFire)
		return;

	std::deque<int> hitboxes;

	hitboxes.push_back(HITBOX_HEAD);

	/*hitboxes.push_back(HITBOX_NECK);
	hitboxes.push_back(HITBOX_LOWER_CHEST);
	hitboxes.push_back(HITBOX_CHEST);
	hitboxes.push_back(HITBOX_UPPER_CHEST);*/

	//hitboxes.push_back(HITBOX_PELVIS);
	/*hitboxes.push_back(HITBOX_STOMACH);

	hitboxes.push_back(HITBOX_RIGHT_THIGH);
	hitboxes.push_back(HITBOX_LEFT_THIGH);
	hitboxes.push_back(HITBOX_RIGHT_CALF);
	hitboxes.push_back(HITBOX_LEFT_CALF);
	hitboxes.push_back(HITBOX_RIGHT_FOOT);
	hitboxes.push_back(HITBOX_LEFT_FOOT);

	hitboxes.push_back(HITBOX_RIGHT_UPPER_ARM);
	hitboxes.push_back(HITBOX_RIGHT_FOREARM);
	hitboxes.push_back(HITBOX_LEFT_UPPER_ARM);
	hitboxes.push_back(HITBOX_LEFT_FOREARM);
	hitboxes.push_back(HITBOX_RIGHT_HAND);
	hitboxes.push_back(HITBOX_LEFT_HAND);*/

	CRagebotPosition target;
	target.damage = 0.f;

	for (auto i = 1; i <= g_pGlobalVars->maxClients; ++i)
	{
		const auto player = (C_BasePlayer*)C_BaseEntity::GetEntityByIndex(i);

		if (!g_Utils.IsValidPlayer(player, cvar.ragebot.aim_friendly_fire))
			continue;

		for (auto &&hitbox : hitboxes)
		{
			Vector vecHitbox = g_Player[i]->m_vecHitbox[hitbox];

			float flFOV = g_Utils.AngleBetween(g_Local->m_vecForward, vecHitbox - g_Local->m_vecEyePos);

			if (flFOV > cvar.ragebot.weapon[weaponid].aim_fov)
				continue;

			Vector vecPoint = vecHitbox;
			//multipoints
			CGameTrace tr;

			const auto current_damage = g_AutoWall.Penetrate(vecPoint, tr);
			const auto need_damage = (player->m_iHealth() / 100.0f) * cvar.ragebot.weapon[weaponid].aim_minimum_damage;

			if (current_damage < need_damage)
				continue;

			if (current_damage > target.damage)
			{
				target.player = player;
				target.point = vecPoint;
				target.damage = current_damage;
				target.lagcomp = false;
			}
		}
	}

	if (target.player)
	{
		auto targetIndex = target.player->EntIndex();
		g_pIEngine->Con_NPrintf(1, "name: %s", target.player->GetPlayerInfo().szName);
		g_pIEngine->Con_NPrintf(2, "lagcomp: %i", target.lagcomp);
		g_pIEngine->Con_NPrintf(3, "damage: %f", target.damage);

		QAngle QAimAngles, QNewAngles;

		g_Utils.VectorAngles(target.point - g_Local->m_vecEyePos, QAimAngles);

		QAimAngles.Normalize();

		QNewAngles = QAimAngles;
		QNewAngles[0] -= g_pLocalPlayer->m_aimPunchAngle()[0] * g_Globals.weapon_recoil_scale;
		QNewAngles[1] -= g_pLocalPlayer->m_aimPunchAngle()[1] * g_Globals.weapon_recoil_scale;

		QNewAngles.Normalize();

		Vector vecForward, vecRight, vecUp;

		QAimAngles.AngleVectors(&vecForward, &vecRight, &vecUp);

		int iHits = 0;
		int iHitsNeed = 255.f * (cvar.ragebot.weapon[weaponid].aim_spread_hit_chance / 100.f);

		for (int seed = 1; seed < 256; seed++)
		{
			g_Utils.RandomSeed(seed);

			float flRand1 = g_Utils.RandomFloat(0.f, 1.f);
			float flRandPi1 = g_Utils.RandomFloat(0.f, 2.f * DirectX::XM_PI);
			float flRand2 = g_Utils.RandomFloat(0.f, 1.f);
			float flRandPi2 = g_Utils.RandomFloat(0.f, 2.f * DirectX::XM_PI);

			float flInaccuracy = flRand1 * g_Weapon->m_flInaccuracy;
			float flSpread = flRand2 * g_Weapon->m_flSpread;

			if (g_Weapon->m_iCurrentId == WEAPON_REVOLVER && cmd->buttons & IN_ATTACK2)
			{
				flRand1 = 1.f - flRand1 * flRand1;
				flRand2 = 1.f - flRand2 * flRand2;
			}
			else if (g_Weapon->m_iCurrentId == WEAPON_NEGEV && g_Weapon->m_flRecoilIndex < 3.f)
			{
				for (int i = 3; i > g_Weapon->m_flRecoilIndex; --i)
				{
					flRand1 *= flRand1;
					flRand2 *= flRand2;
				}

				flRand1 = 1.f - flRand1;
				flRand2 = 1.f - flRand2;
			}

			float flSpreadX = cos(flRandPi1) * flInaccuracy + cos(flRandPi2) * flSpread;
			float flSpreadY = sin(flRandPi1) * flInaccuracy + sin(flRandPi2) * flSpread;

			QAngle QSpreadAngles;
			Vector vecSpread = (vecForward + flSpreadX * vecRight + flSpreadY * vecUp), vecSpreadDirection, vecEnd;

			vecSpread.Normalize();

			g_Utils.VectorAngles(vecSpread, QSpreadAngles);

			QSpreadAngles.AngleVectors(&vecSpreadDirection, NULL, NULL);

			vecSpreadDirection.Normalize();

			vecEnd = g_Local->m_vecEyePos + (vecSpreadDirection * g_Weapon->m_flRange);

			CGameTrace tr;
			CTraceFilterPlayersOnlySkipOne filter(g_pLocalPlayer);

			Ray_t ray;
			ray.Init(g_Local->m_vecEyePos, vecEnd);

			g_pIEngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

			if (tr.hit_entity == target.player)
				iHits++;

			if (iHits >= iHitsNeed)
				break;

			if ((255 - seed + iHits) < iHitsNeed)
				break;
		}

		g_pIEngine->Con_NPrintf(4, "hits: %i / %i", iHits, iHitsNeed);

		if (iHits < iHitsNeed)
		{
			if (cvar.ragebot.aim_automatic_scope && g_Weapon.IsSniper() && !g_pLocalPlayer->m_bIsScoped())
			{
				cmd->buttons &= ~IN_ATTACK;
				cmd->buttons |= IN_ATTACK2;
			}

			if (cvar.ragebot.aim_quick_stop)
				_QuickStop(cmd);

			return;
		}

		g_Utils.MakeAngle(false, QNewAngles, cmd);

		if (!cvar.ragebot.aim_silent)
			g_pIEngine->SetViewAngles(&QNewAngles);

		if (cvar.ragebot.aim_auto_fire)
			cmd->buttons |= IN_ATTACK;

		if (target.lagcomp)
			g_LagComp.ProcessCmd(cmd, target.history);
		//else
		//	cmd->tick_count = TIME_TO_TICKS(target.player->m_flSimulationTime() + g_LagComp.GetLerpTime());
	}
}

void CAimbot::_LegitBot(CUserCmd* cmd)
{
	_bAimbotActive = false;
	m_flLegitBotFOV = 0.f;

	if (_bTriggerActive)
		return;

	const auto weaponid = g_Weapon->m_iCurrentId;

	if (!cvar.legitbot.weapon[weaponid].aim)
		return;

	std::deque<int> hitboxes;

	if (cvar.legitbot.weapon[weaponid].aim_head)
		hitboxes.push_back(HITBOX_HEAD);
	if (cvar.legitbot.weapon[weaponid].aim_chest)
	{
		hitboxes.push_back(HITBOX_NECK);
		hitboxes.push_back(HITBOX_LOWER_CHEST);
		hitboxes.push_back(HITBOX_CHEST);
		hitboxes.push_back(HITBOX_UPPER_CHEST);
	}
	if (cvar.legitbot.weapon[weaponid].aim_stomach)
	{
		hitboxes.push_back(HITBOX_PELVIS);
		hitboxes.push_back(HITBOX_STOMACH);
	}
	if (cvar.legitbot.weapon[weaponid].aim_legs)
	{
		hitboxes.push_back(HITBOX_RIGHT_THIGH);
		hitboxes.push_back(HITBOX_LEFT_THIGH);
		hitboxes.push_back(HITBOX_RIGHT_CALF);
		hitboxes.push_back(HITBOX_LEFT_CALF);
		hitboxes.push_back(HITBOX_RIGHT_FOOT);
		hitboxes.push_back(HITBOX_LEFT_FOOT);
	}
	if (cvar.legitbot.weapon[weaponid].aim_arms)
	{
		hitboxes.push_back(HITBOX_RIGHT_UPPER_ARM);
		hitboxes.push_back(HITBOX_RIGHT_FOREARM);
		hitboxes.push_back(HITBOX_LEFT_UPPER_ARM);
		hitboxes.push_back(HITBOX_LEFT_FOREARM);
		hitboxes.push_back(HITBOX_RIGHT_HAND);
		hitboxes.push_back(HITBOX_LEFT_HAND);
	}

	if (hitboxes.empty())
		return;

	float flBestFOV = cvar.legitbot.weapon[weaponid].aim_fov;
	float flSmooth = (cmd->buttons & IN_ATTACK) ? cvar.legitbot.weapon[weaponid].aim_smooth_in_attack : cvar.legitbot.weapon[weaponid].aim_smooth_auto;

	if ((cmd->buttons & IN_ATTACK) && g_Weapon->m_iShotsFired >= cvar.legitbot.weapon[weaponid].aim_recoil_start)
	{
		if ((g_pLocalPlayer->m_aimPunchAngle().x != 0.0f && cvar.legitbot.weapon[weaponid].aim_recoil_pitch) || (g_pLocalPlayer->m_aimPunchAngle().y != 0.0f && cvar.legitbot.weapon[weaponid].aim_recoil_yaw))
		{
			flBestFOV = cvar.legitbot.weapon[weaponid].aim_recoil_fov;
			flSmooth = cvar.legitbot.weapon[weaponid].aim_recoil_smooth;
		}
	}

	if (flBestFOV <= 0.0f || flSmooth <= 0.0f)
		return;

	if (cvar.legitbot.weapon[weaponid].aim_reaction_time && abs(g_pGlobalVars->curtime - m_flLastTimeKilled) <= (cvar.legitbot.weapon[weaponid].aim_reaction_time / 1000.f))
		return;

	m_flLegitBotFOV = flBestFOV;

	Vector vecRecoil, vecSrc = g_pLocalPlayer->GetEyePos();
	{
		QAngle QAngles(cmd->viewangles);
		QAngles.x += g_pLocalPlayer->m_aimPunchAngle().x * g_Globals.weapon_recoil_scale;
		QAngles.y += g_pLocalPlayer->m_aimPunchAngle().y * g_Globals.weapon_recoil_scale;
		QAngles.Normalize();
		QAngles.AngleVectors(&vecRecoil, NULL, NULL);
	}

	CTraceFilter trFilter;
	trFilter.pSkip = g_pLocalPlayer;

	CTarget target;

	for (auto i = 1; i <= g_pGlobalVars->maxClients; ++i)
	{
		const auto player = (C_BasePlayer*)C_BaseEntity::GetEntityByIndex(i);

		if (!g_Utils.IsValidPlayer(player, cvar.legitbot.friendly_fire))
			continue;

		auto& history = g_Player.GetHistory(i);

		if (cvar.legitbot.position_adjustment && !history.empty())
		{
			for (auto record : history)
			{
				if (!g_LagComp.IsValidRecord(record))
					continue;

				for (auto &&hitbox : hitboxes)
				{
					Vector vecHitbox = record.m_vecHitbox[hitbox];

					float flFOV = g_Utils.AngleBetween(vecRecoil, vecHitbox - vecSrc);

					if (flFOV < flBestFOV)
					{
						g_LagComp.SetRecord(player, record);
						players_for_restore.push_back(player);

						CGameTrace tr;

						if (cvar.legitbot.weapon[weaponid].aim_automatic_penetration)
						{
							const auto current_damage = g_AutoWall.Penetrate(vecHitbox, tr);
							const auto need_damage = (player->m_iHealth() / 100.0f) * cvar.legitbot.weapon[weaponid].aim_automatic_penetration_minimum_damage;

							if (current_damage < need_damage)
								continue;
						}
						else
						{
							Ray_t ray;
							ray.Init(vecSrc, vecHitbox);

							g_pIEngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &trFilter, &tr);
						}

						if (tr.hit_entity == player)
						{
							flBestFOV = flFOV;
							target.player = player;
							target.hitbox = vecHitbox;
							target.history = record;
							target.lagcomp = true;
						}
					}
				}
			}
		}
		else
		{
			for (auto &&hitbox : hitboxes)
			{
				Vector vecHitbox = g_Player[i]->m_vecHitbox[hitbox];

				float flFOV = g_Utils.AngleBetween(vecRecoil, vecHitbox - vecSrc);

				if (flFOV < flBestFOV)
				{
					CGameTrace tr;

					if (cvar.legitbot.weapon[weaponid].aim_automatic_penetration)
					{
						const auto current_damage = g_AutoWall.Penetrate(vecHitbox, tr);
						const auto need_damage = (player->m_iHealth() / 100.0f) * cvar.legitbot.weapon[weaponid].aim_automatic_penetration_minimum_damage;

						if (current_damage < need_damage)
							continue;
					}
					else
					{
						Ray_t ray;
						ray.Init(vecSrc, vecHitbox);

						g_pIEngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &trFilter, &tr);
					}

					if (tr.hit_entity == player)
					{
						flBestFOV = flFOV;
						target.player = player;
						target.hitbox = vecHitbox;
						target.lagcomp = false;
					}
				}
			}
		}
	}

	{
		const auto current_target_index = target.player ? target.player->EntIndex() : -1;

		static auto prev_time = g_pGlobalVars->realtime;
		static auto prev_index = current_target_index;

		if (prev_index != current_target_index)
		{
			prev_time = g_pGlobalVars->realtime;
			prev_index = current_target_index;
		}

		const auto time_difference = abs(g_pGlobalVars->realtime - prev_time) * 1000.f;

		if (cvar.legitbot.weapon[weaponid].aim_maximum_lock_on_time > 0.0f && time_difference > cvar.legitbot.weapon[weaponid].aim_maximum_lock_on_time)
			return;

		if (cvar.legitbot.weapon[weaponid].aim_delay_before_aiming > 0.0f && time_difference <= cvar.legitbot.weapon[weaponid].aim_delay_before_aiming)
			return;

		if (cvar.legitbot.weapon[weaponid].aim_delay_before_firing > 0.0f && time_difference <= cvar.legitbot.weapon[weaponid].aim_delay_before_firing)
			cmd->buttons &= ~IN_ATTACK;
	}

	if (target.player)
	{
		if (cvar.legitbot.aim_automatic_scope && g_Weapon.IsSniper() && !g_pLocalPlayer->m_bIsScoped() && cmd->buttons & IN_ATTACK)
		{
			cmd->buttons &= ~IN_ATTACK;
			cmd->buttons |= IN_ATTACK2;
			return;
		}

		const int targetIndex = target.player->EntIndex();
		const auto player = target.player;

		QAngle QAimAngles, QSmoothAngles, QNewAngles;

		Vector vecForward = target.hitbox - vecSrc;

		g_Utils.VectorAngles(vecForward, QAimAngles);

		QNewAngles.Init(QAimAngles);

		if (g_Weapon->m_iShotsFired >= cvar.legitbot.weapon[weaponid].aim_recoil_start)
		{
			QNewAngles.x -= g_pLocalPlayer->m_aimPunchAngle().x * (cvar.legitbot.weapon[weaponid].aim_recoil_pitch / 50.0f);
			QNewAngles.y -= g_pLocalPlayer->m_aimPunchAngle().y * (cvar.legitbot.weapon[weaponid].aim_recoil_yaw / 50.0f);
		}

		QNewAngles.Normalize();

		if (cvar.legitbot.weapon[weaponid].aim_smooth_scale_fov > 0.0f && g_pLocalPlayer->m_aimPunchAngle().IsZero2D())
		{
			const float flFOV = g_Utils.AngleBetween(vecRecoil, vecForward);

			if (isfinite(flFOV) && flFOV > 0.0f)
				flSmooth = flSmooth - ((flFOV * (flSmooth / cvar.legitbot.weapon[weaponid].aim_fov) * cvar.legitbot.weapon[weaponid].aim_smooth_scale_fov) / 100.0f);
		}

		if (cvar.legitbot.aim_dependence_tickrate && g_Local->m_flTickRate > 0.0f)
			flSmooth *= (g_Local->m_flTickRate / 128.0f);

		_SmoothAimAngles(cmd->viewangles, QNewAngles, QSmoothAngles, flSmooth);

		QSmoothAngles.Normalize();

		if (flSmooth <= 0.0f)
			return;

		_bAimbotActive = true;

		g_Utils.MakeAngle(false, QSmoothAngles, cmd);

		g_pIEngine->SetViewAngles(&QSmoothAngles);

		if (cvar.legitbot.weapon[weaponid].aim_accuracy && cmd->buttons & IN_ATTACK && g_Weapon->m_IsCanFire)
		{
			QSmoothAngles.x += g_pLocalPlayer->m_aimPunchAngle().x * g_Globals.weapon_recoil_scale;
			QSmoothAngles.y += g_pLocalPlayer->m_aimPunchAngle().y * g_Globals.weapon_recoil_scale;
			QSmoothAngles.Normalize();

			if (cvar.legitbot.weapon[weaponid].aim_accuracy == 1)
			{
				Vector vecForward;

				QSmoothAngles.AngleVectors(&vecForward, NULL, NULL);

				vecForward.Normalize();

				Vector vecEnd = vecSrc + vecForward * g_Weapon->m_pWeaponData->flRange;

				CGameTrace tr;
				CTraceFilterPlayersOnlySkipOne filter(g_pLocalPlayer);

				Ray_t ray;
				ray.Init(vecSrc, vecEnd);

				g_pIEngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

				if (tr.hit_entity != target.player)
					cmd->buttons &= ~IN_ATTACK;
			}
			else if (cvar.legitbot.weapon[weaponid].aim_accuracy == 2 && cvar.legitbot.weapon[weaponid].aim_spread_hit_chance > 0.f)
			{
				bool bAllow = false;

				Vector vecForward, vecRight, vecUp;

				QSmoothAngles.AngleVectors(&vecForward, &vecRight, &vecUp);

				int iHits = 0;
				int iHitsNeed = 255.f * (cvar.legitbot.weapon[weaponid].aim_spread_hit_chance / 100.f);

				for (int seed = 1; seed < 256; seed++)
				{
					g_Utils.RandomSeed(seed);

					float flRand1 = g_Utils.RandomFloat(0.f, 1.f);
					float flRandPi1 = g_Utils.RandomFloat(0.f, 2.f * DirectX::XM_PI);
					float flRand2 = g_Utils.RandomFloat(0.f, 1.f);
					float flRandPi2 = g_Utils.RandomFloat(0.f, 2.f * DirectX::XM_PI);

					float flInaccuracy = flRand1 * g_Weapon->m_flInaccuracy;
					float flSpread = flRand2 * g_Weapon->m_flSpread;

					if (g_Weapon->m_iCurrentId == WEAPON_REVOLVER && cmd->buttons & IN_ATTACK2)
					{
						flRand1 = 1.f - flRand1 * flRand1;
						flRand2 = 1.f - flRand2 * flRand2;
					}
					else if (g_Weapon->m_iCurrentId == WEAPON_NEGEV && g_Weapon->m_flRecoilIndex < 3.f)
					{
						for (int i = 3; i > g_Weapon->m_flRecoilIndex; --i)
						{
							flRand1 *= flRand1;
							flRand2 *= flRand2;
						}

						flRand1 = 1.f - flRand1;
						flRand2 = 1.f - flRand2;
					}

					float flSpreadX = cos(flRandPi1) * flInaccuracy + cos(flRandPi2) * flSpread;
					float flSpreadY = sin(flRandPi1) * flInaccuracy + sin(flRandPi2) * flSpread;

					Vector vecSpread = (vecForward + flSpreadX * vecRight + flSpreadY * vecUp), vecSpreadDirection, vecEnd;

					vecSpread.Normalize();

					QAngle QSpreadAngles;

					g_Utils.VectorAngles(vecSpread, QSpreadAngles);

					QSpreadAngles.AngleVectors(&vecSpreadDirection, NULL, NULL);

					vecSpreadDirection.Normalize();

					vecEnd = vecSrc + vecSpreadDirection * g_Weapon->m_pWeaponData->flRange;

					CGameTrace tr;
					CTraceFilterPlayersOnlySkipOne filter(g_pLocalPlayer);

					Ray_t ray;
					ray.Init(vecSrc, vecEnd);

					g_pIEngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

					if (tr.hit_entity == target.player)
						iHits++;

					if (iHits >= iHitsNeed)
					{
						bAllow = true;
						break;
					}

					if ((255 - seed + iHits) < iHitsNeed)
						break;
				}

				if (!bAllow)
					cmd->buttons &= ~IN_ATTACK;
			}
		}

		if (target.lagcomp && cmd->buttons & IN_ATTACK && g_Weapon->m_IsCanFire)
			g_LagComp.ProcessCmd(cmd, target.history);
		//else
		//	cmd->tick_count = TIME_TO_TICKS(target.player->m_flSimulationTime() + g_LagComp.GetLerpTime());
	}
}

void CAimbot::_Trigger(CUserCmd* cmd)
{
	_bTriggerActive = false;

	if (!g_Weapon->m_IsCanFire)
		return;

	const auto weaponid = g_Weapon->m_iCurrentId;

	if (!cvar.legitbot.weapon[weaponid].trigger)
		return;

	if (cvar.legitbot.trigger_only_scoped && g_Weapon.IsSniper() && g_Local->m_iFOV == 90)
		return;

	std::deque<int> hitboxes;

	if (cvar.legitbot.weapon[weaponid].trigger_head)
		hitboxes.push_back(HITBOX_HEAD);
	if (cvar.legitbot.weapon[weaponid].trigger_chest)
	{
		hitboxes.push_back(HITBOX_NECK);
		hitboxes.push_back(HITBOX_LOWER_CHEST);
		hitboxes.push_back(HITBOX_CHEST);
		hitboxes.push_back(HITBOX_UPPER_CHEST);
	}
	if (cvar.legitbot.weapon[weaponid].trigger_stomach)
	{
		hitboxes.push_back(HITBOX_PELVIS);
		hitboxes.push_back(HITBOX_STOMACH);
	}
	if (cvar.legitbot.weapon[weaponid].trigger_legs)
	{
		hitboxes.push_back(HITBOX_RIGHT_THIGH);
		hitboxes.push_back(HITBOX_LEFT_THIGH);
		hitboxes.push_back(HITBOX_RIGHT_CALF);
		hitboxes.push_back(HITBOX_LEFT_CALF);
		hitboxes.push_back(HITBOX_RIGHT_FOOT);
		hitboxes.push_back(HITBOX_LEFT_FOOT);
	}
	if (cvar.legitbot.weapon[weaponid].trigger_arms)
	{
		hitboxes.push_back(HITBOX_RIGHT_UPPER_ARM);
		hitboxes.push_back(HITBOX_RIGHT_FOREARM);
		hitboxes.push_back(HITBOX_LEFT_UPPER_ARM);
		hitboxes.push_back(HITBOX_LEFT_FOREARM);
		hitboxes.push_back(HITBOX_RIGHT_HAND);
		hitboxes.push_back(HITBOX_LEFT_HAND);
	}

	if (hitboxes.empty())
		return;

	CTarget target;

	Vector vecForward, vecRight, vecUp;

	QAngle QAngles(cmd->viewangles);

	if (cvar.legitbot.weapon[weaponid].trigger_accuracy)
	{
		QAngles.x += g_pLocalPlayer->m_aimPunchAngle().x * g_Globals.weapon_recoil_scale;
		QAngles.y += g_pLocalPlayer->m_aimPunchAngle().y * g_Globals.weapon_recoil_scale;
	}

	QAngles.Normalize();
	QAngles.AngleVectors(&vecForward, &vecRight, &vecUp);

	Vector vecSrc = g_pLocalPlayer->GetEyePos();

	float flBestFOV = FLT_MAX;

	for (auto i = 1; i <= g_pGlobalVars->maxClients; ++i)
	{
		const auto player = (C_BasePlayer*)C_BaseEntity::GetEntityByIndex(i);

		if (!g_Utils.IsValidPlayer(player, cvar.legitbot.friendly_fire))
			continue;

		auto& history = g_Player.GetHistory(i);

		if (cvar.legitbot.position_adjustment && !history.empty())
		{
			for (auto record : history)
			{
				if (!g_LagComp.IsValidRecord(record))
					continue;

				for (auto &&hitbox : hitboxes)
				{
					float flFOV = g_Utils.AngleBetween(vecForward, record.m_vecHitbox[hitbox] - vecSrc);

					if (flFOV < flBestFOV)
					{
						flBestFOV = flFOV;
						target.history = record;
						target.player = player;
						target.lagcomp = true;
					}
				}
			}
		}
		else {
			for (auto &&hitbox : hitboxes)
			{
				float flFOV = g_Utils.AngleBetween(vecForward, g_Player[i]->m_vecHitbox[hitbox] - vecSrc);

				if (flFOV < flBestFOV)
				{
					flBestFOV = flFOV;
					target.player = player;
					target.lagcomp = false;
				}
			}
		}
	}

	if (target.player)
	{
		const int targetIndex = target.player->EntIndex();

		if (target.lagcomp)
		{
			g_LagComp.SetRecord(target.player, target.history);
			players_for_restore.push_back(target.player);
		}

		if (cvar.legitbot.weapon[weaponid].trigger_accuracy == 2 && cvar.legitbot.weapon[weaponid].trigger_spread_hit_chance > 0.f)
		{
			int iHits = 0;
			int iHitsNeed = 255.f * (cvar.legitbot.weapon[weaponid].trigger_spread_hit_chance / 100.f);

			for (int seed = 1; seed < 256; seed++)
			{
				g_Utils.RandomSeed(seed);

				float flRand1 = g_Utils.RandomFloat(0.f, 1.f);
				float flRandPi1 = g_Utils.RandomFloat(0.f, 2.f * DirectX::XM_PI);
				float flRand2 = g_Utils.RandomFloat(0.f, 1.f);
				float flRandPi2 = g_Utils.RandomFloat(0.f, 2.f * DirectX::XM_PI);

				float flInaccuracy = flRand1 * g_Weapon->m_flInaccuracy;
				float flSpread = flRand2 * g_Weapon->m_flSpread;

				if (g_Weapon->m_iCurrentId == WEAPON_REVOLVER && cmd->buttons & IN_ATTACK2)
				{
					flRand1 = 1.f - flRand1 * flRand1;
					flRand2 = 1.f - flRand2 * flRand2;
				}
				else if (g_Weapon->m_iCurrentId == WEAPON_NEGEV && g_Weapon->m_flRecoilIndex < 3.f)
				{
					for (int i = 3; i > g_Weapon->m_flRecoilIndex; --i)
					{
						flRand1 *= flRand1;
						flRand2 *= flRand2;
					}

					flRand1 = 1.f - flRand1;
					flRand2 = 1.f - flRand2;
				}

				float flSpreadX = cos(flRandPi1) * flInaccuracy + cos(flRandPi2) * flSpread;
				float flSpreadY = sin(flRandPi1) * flInaccuracy + sin(flRandPi2) * flSpread;

				Vector vecSpread = (vecForward + flSpreadX * vecRight + flSpreadY * vecUp), vecSpreadDirection, vecEnd;

				vecSpread.Normalize();

				QAngle QSpreadAngles;

				g_Utils.VectorAngles(vecSpread, QSpreadAngles);

				QSpreadAngles.AngleVectors(&vecSpreadDirection, NULL, NULL);

				vecSpreadDirection.Normalize();

				vecEnd = vecSrc + vecSpreadDirection * g_Weapon->m_pWeaponData->flRange;

				CGameTrace tr;
				CTraceFilterPlayersOnlySkipOne filter(g_pLocalPlayer);

				Ray_t ray;
				ray.Init(vecSrc, vecEnd);

				g_pIEngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

				if (tr.hit_entity == target.player)
					iHits++;

				if (iHits >= iHitsNeed)
					break;

				if ((255 - seed + iHits) < iHitsNeed)
					break;
			}
			//g_pIEngine->Con_NPrintf(4, "hits: %i / %i", iHits, iHitsNeed);
			if (iHits < iHitsNeed)
				return;
		}

		Vector vecEnd = vecSrc + vecForward * g_Weapon->m_pWeaponData->flRange;

		CGameTrace tr;

		CTraceFilter trFilter;
		trFilter.pSkip = g_pLocalPlayer;

		Ray_t ray;
		ray.Init(vecSrc, vecEnd);

		g_pIEngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &trFilter, &tr);

		if (tr.hit_entity == target.player)
		{
			for (auto &&hitbox : hitboxes)
			{
				if (tr.hitbox == hitbox)
				{
					_bTriggerActive = true;
					break;
				}
			}
		}
		else if (cvar.legitbot.weapon[weaponid].trigger_automatic_penetration)
		{
			auto current_damage = g_AutoWall.Penetrate(vecEnd, tr);
			auto need_damage = (target.player->m_iHealth() / 100.f) * cvar.legitbot.weapon[weaponid].trigger_automatic_penetration_minimum_damage;

			if (current_damage < need_damage)
				return;

			if (tr.hit_entity == target.player)
			{
				for (auto &&hitbox : hitboxes)
				{
					if (tr.hitbox == hitbox)
					{
						_bTriggerActive = true;
						break;
					}
				}
			}
		}

		if (_bTriggerActive)
		{
			cmd->buttons |= IN_ATTACK;

			if (target.lagcomp)
				g_LagComp.ProcessCmd(cmd, target.history);
			//else
			//	cmd->tick_count = TIME_TO_TICKS(target.player->m_flSimulationTime() + g_LagComp.GetLerpTime());
		}
	}
}

void CAimbot::_QuickStop(CUserCmd *cmd)
{
	// note: scoped weapons use the alternate speed member.
	const float max_speed = g_Weapon.HasScope() ? g_Weapon->m_pWeaponData->flMaxPlayerSpeedAlt : g_Weapon->m_pWeaponData->flMaxPlayerSpeed;

	const Vector velocity = g_Local->m_vecVelocity;

	if (velocity.Length2D() > max_speed * .34f)
	{
		const float_t speed = velocity.Length();

		QAngle direction;
		g_Utils.VectorAngles(velocity, direction);

		direction.y = cmd->viewangles.y - direction.y;

		Vector forward;
		direction.AngleVectors(&forward, 0, 0);
		Vector negated_direction = forward * -speed;

		const float factor = max(negated_direction.x, negated_direction.y) / 450.f;
		negated_direction *= factor;

		cmd->forwardmove = negated_direction.x;
		cmd->sidemove = negated_direction.y;
	}
}

void CAimbot::_SmoothAimAngles(const QAngle a_QAngles, const QAngle a_QAimAngles, QAngle &a_QNewAngles, const float a_flSmoothing)
{
	if (a_flSmoothing <= 1.f || isnan(a_flSmoothing))
	{
		a_QNewAngles = a_QAimAngles;
		return;
	}

	a_QNewAngles = a_QAimAngles - a_QAngles;

	a_QNewAngles.Normalize();

	a_QNewAngles /= a_flSmoothing;

	a_QNewAngles = a_QAngles + a_QNewAngles;

	a_QNewAngles.Normalize();
}