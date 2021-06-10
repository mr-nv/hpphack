#include "main.h"

CAimbot g_Aimbot;

CTarget::CTarget() :
	m_iHitbox(-1),
	m_pPlayer(nullptr)
{
}

void CAimbot::Run(usercmd_s* cmd)
{
	if (!g_Weapon.IsGun())
		return;

	_Trigger(cmd);
	//_LegitBot(cmd);
}

void CAimbot::_Trigger(usercmd_s* cmd)
{
	_bTriggerActive = false;

	if (!g_Weapon.CanAttack())
		return;

	std::deque<int> hitboxes;

	hitboxes.push_back(HITBOX_HEAD);

	hitboxes.push_back(HITBOX_LOWER_CHEST);
	hitboxes.push_back(HITBOX_CHEST);
	hitboxes.push_back(HITBOX_UPPER_CHEST);
	hitboxes.push_back(HITBOX_NECK);
	
	hitboxes.push_back(HITBOX_STOMACH);

	if (hitboxes.empty())
		return;

	QAngle QAngles(cmd->viewangles);

	Vector vecSpreadDir;

	QAngles[0] += client_state->punchangle[0] * 2.f;
	QAngles[1] += client_state->punchangle[1] * 2.f;

	QAngles.Normalize();

	QAngles.AngleVectors(&vecSpreadDir, 0, 0);

	vecSpreadDir.Normalize();

	for (auto i = 1; i <= MAX_CLIENTS; ++i)
	{
		if (i == g_Local.m_iIndex)
			continue;

		auto const pPlayer = g_World.GetPlayer(i);

		if (!pPlayer)
			continue;

		if (pPlayer->m_iTeam == g_Local.m_iTeam)
			continue;

		if (pPlayer->m_bIsDead)
			continue;

		if (!pPlayer->m_bIsInPVS)
			continue;

		for (auto&& hitbox : hitboxes)
		{
			if (g_Utils.IsBoxIntersectingRay(pPlayer->m_vecHitboxPoints[hitbox][0], pPlayer->m_vecHitboxPoints[hitbox][1], g_Local.m_vecEye, vecSpreadDir) ||
				g_Utils.IsBoxIntersectingRay(pPlayer->m_vecHitboxPoints[hitbox][6], pPlayer->m_vecHitboxPoints[hitbox][3], g_Local.m_vecEye, vecSpreadDir) ||
				g_Utils.IsBoxIntersectingRay(pPlayer->m_vecHitboxPoints[hitbox][7], pPlayer->m_vecHitboxPoints[hitbox][4], g_Local.m_vecEye, vecSpreadDir) ||
				g_Utils.IsBoxIntersectingRay(pPlayer->m_vecHitboxPoints[hitbox][2], pPlayer->m_vecHitboxPoints[hitbox][5], g_Local.m_vecEye, vecSpreadDir))
			{
				pmtrace_s tr;
				g_Engine.pEventAPI->EV_SetTraceHull(2);
				g_Engine.pEventAPI->EV_PlayerTrace(g_Local.m_vecEye, g_Local.m_vecEye + vecSpreadDir * g_Weapon.GetDistance(), PM_NORMAL, -1, &tr);

				if (g_Engine.pEventAPI->EV_IndexFromTrace(&tr) == pPlayer->m_iIndex)
					_bTriggerActive = true;

				if (_bTriggerActive)
				{
					cmd->buttons |= IN_ATTACK;
					return;
				}
			}
		}
	}
}

void CAimbot::_LegitBot(usercmd_s* cmd)
{
	_bAimbotActive = false;

	if (_bTriggerActive)
		return;

	if (g_Weapon.data.m_iInReload)
		return;

	std::deque<int> hitboxes;

	hitboxes.push_back(HITBOX_HEAD);

	hitboxes.push_back(HITBOX_LOWER_CHEST);
	hitboxes.push_back(HITBOX_CHEST);
	hitboxes.push_back(HITBOX_UPPER_CHEST);
	hitboxes.push_back(HITBOX_NECK);


	hitboxes.push_back(HITBOX_STOMACH);


	if (hitboxes.empty())
		return;

	float flBestFOV = 6;

	if (!client_state->punchangle.IsZero2D())
		flBestFOV = 5;

	if (flBestFOV <= 0.0f)
		return;

	float flSmooth = cmd->buttons & IN_ATTACK ? 1 : 40;

	QAngle QAngles(cmd->viewangles);
	QAngles[0] += client_state->punchangle[0] * 2.f;
	QAngles[1] += client_state->punchangle[1] * 2.f;
	QAngles.Normalize();

	Vector vecRecoil;
	QAngles.AngleVectors(&vecRecoil, 0, 0);

	auto best_target = CTarget();

	for (auto i = 1; i <= MAX_CLIENTS; ++i)
	{
		if (i == g_Local.m_iIndex)
			continue;

		auto * const pPlayer = g_World.GetPlayer(i);

		if (!pPlayer)
			continue;

		if (pPlayer->m_iTeam == g_Local.m_iTeam)
			continue;

		if (pPlayer->m_bIsDead)
			continue;

		if (!pPlayer->m_bIsInPVS)
			continue;

		for (auto&& hitbox : hitboxes)
		{
			Vector vecHitbox = pPlayer->m_vecHitbox[hitbox];

			Vector vecForward = vecHitbox - g_Local.m_vecEye;

			const float flFOV = vecRecoil.AngleBetween(vecForward);

			if (flFOV < flBestFOV)
			{
				pmtrace_s tr;
				g_Engine.pEventAPI->EV_SetTraceHull(2);
				g_Engine.pEventAPI->EV_PlayerTrace(g_Local.m_vecEye, vecHitbox, PM_NORMAL, -1, &tr);

				if (g_Engine.pEventAPI->EV_IndexFromTrace(&tr) == pPlayer->m_iIndex)
				{
					flBestFOV = flFOV;

					best_target.m_pPlayer = pPlayer;
					best_target.m_iHitbox = hitbox;
				}
			}
		}
	}
	if (best_target.m_pPlayer)
	{
		const auto player = best_target.m_pPlayer;
		const auto hitbox = best_target.m_iHitbox;

		QAngle QAimAngles, QSmoothAngles;

		Vector vecAimOrigin = player->m_vecHitbox[hitbox];

		Vector vecForward = vecAimOrigin - g_Local.m_vecEye;

		g_Utils.VectorAngles(vecForward, QAimAngles);

		QAngle QNewAngles(QAimAngles);

		QNewAngles[0] -= client_state->punchangle[0] * 2;
		QNewAngles[1] -= client_state->punchangle[1] * 2;

		QNewAngles.Normalize();

		/*if (g_Globals.m_flFrameTime > 0.f)
		{
			flSmooth *= 1.f / g_Globals.m_flFrameTime;
			flSmooth /= 100.f;
		}*/

		_SmoothAimAngles(QAngle(cmd->viewangles), QNewAngles, QSmoothAngles, flSmooth);

		QSmoothAngles.Normalize();

		if (flSmooth <= 0.f)
			return;

		_bAimbotActive = true;

		g_Utils.MakeAngle(false, QSmoothAngles, cmd);

		g_Engine.SetViewAngles(QSmoothAngles);
	}
}

void CAimbot::_SmoothAimAngles(const QAngle a_QAngles, const QAngle a_QAimAngles, QAngle& a_QNewAngles, const float a_flSmoothing)
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