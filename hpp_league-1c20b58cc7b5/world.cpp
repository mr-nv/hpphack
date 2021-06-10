#include "main.h"

CBaseLocalPlayer g_Local;
CWorld g_World;

bool IsAlive(const cl_entity_s* pGameEntity)
{
	if (pmove->view_ofs.z == PM_DEAD_VIEWHEIGHT)
		return false;

	if (pGameEntity->curstate.iuser1)
		return false;

	return true;
}

void CWorld::UpdateLocalPlayer()
{
	const auto* const pGameEntity = g_Engine.GetLocalPlayer();

	g_Local.m_bIsDead = true;
	g_Local.m_iIndex = pGameEntity->index;
	g_Local.m_iMessageNum = pGameEntity->curstate.messagenum;
	g_Local.m_flVelocity = pmove->velocity.Length2D();

	if (g_Local.m_iTeam == UNASSIGNED && g_PlayerExtraInfo[g_Local.m_iIndex].teamnumber)
		g_Local.m_iTeam = static_cast<int>(g_PlayerExtraInfo[g_Local.m_iIndex].teamnumber);

	if (g_Local.m_bIsDead = !IsAlive(pGameEntity))
		return;

	g_Local.m_vecEye = pmove->origin + pmove->view_ofs;
}

bool IsPlayerDead(const cl_entity_s * pGameEntity)
{
	static const char* pcszSequences[] =
	{
		"death1",
		"death2",
		"death3",
		"head",
		"gutshot",
		"left",
		"back",
		"right",
		"forward",
		"crouch_die"
	};

	if (!g_PlayerExtraInfo[pGameEntity->index].dead)
		return false;

	return (pGameEntity->curstate.sequence == g_Utils.LookupSequence(pGameEntity->model, pcszSequences, IM_ARRAYSIZE(pcszSequences)) ||
		(pGameEntity->curstate.sequence > 100 && pGameEntity->curstate.sequence < 111));
}

void CWorld::UpdatePlayers()
{
	for (int i = 1; i <= MAX_CLIENTS; i++)
	{
		const auto* const pGameEntity = g_Engine.GetEntityByIndex(i);

		if (!pGameEntity)
			continue;

		if (pGameEntity->index < 1 || pGameEntity->index > MAX_CLIENTS)
			continue;

		if (pGameEntity->index == g_Local.m_iIndex)
			continue;

		auto pPlayer = GetPlayer(pGameEntity->index);

		pPlayer->m_bIsInPVS = false;
		pPlayer->m_iIndex = pGameEntity->index;

		if (!pGameEntity->model)
			continue;

		if (!pGameEntity->player)
			continue;

		if (pPlayer->m_iTeam == UNASSIGNED && g_PlayerExtraInfo[pPlayer->m_iIndex].teamnumber)
			pPlayer->m_iTeam = g_PlayerExtraInfo[pPlayer->m_iIndex].teamnumber;

		if (pGameEntity->curstate.messagenum < g_Local.m_iMessageNum)
			continue;

		if ((pPlayer->m_bIsDead = IsPlayerDead(pGameEntity)))
			continue;

		pPlayer->m_bIsInPVS = true;
		pPlayer->m_vecOrigin = pGameEntity->origin;
		pPlayer->m_vecBoundBoxMaxs = pGameEntity->curstate.maxs;
		pPlayer->m_vecBoundBoxMins = pGameEntity->curstate.mins;

		if (!pPlayer->m_vecBoundBoxMins[0] || !pPlayer->m_vecBoundBoxMins[1] || !pPlayer->m_vecBoundBoxMins[2])	// Bypass nulling
			pPlayer->m_vecBoundBoxMins = -Vector(16, 16, pGameEntity->curstate.usehull == 1 ? 18.0F : 36.0F);	// default bbox player

		if (!pPlayer->m_vecBoundBoxMaxs[0] || !pPlayer->m_vecBoundBoxMaxs[1] || !pPlayer->m_vecBoundBoxMaxs[2])	// Bypass nulling
			pPlayer->m_vecBoundBoxMaxs = Vector(16, 16, pGameEntity->curstate.usehull == 1 ? 18.0F : 36.0F);	// default bbox player
	}
}

CBasePlayer* CWorld::GetPlayer(const int index)
{
	if (index == 0)
		return m_Player;

	return index < g_Local.m_iIndex ? &m_Player[index - 1] : &m_Player[index - 2];
}

void CWorld::Update()
{
	UpdateLocalPlayer();
	UpdatePlayers();
}

void CWorld::Clear()
{
	RtlSecureZeroMemory(&g_Local, sizeof(g_Local));
	RtlSecureZeroMemory(m_Player, sizeof(m_Player));
}

void CWorld::Reset()
{
	static bool bResetState, bPrevResetState;

	if (!bResetState && client_static->state != ca_active)
		bResetState = true;

	if (bResetState && client_static->state == ca_active)
		bResetState = false;

	if (bResetState != bPrevResetState)
	{
		Clear();

		g_Local.m_bIsDead = true;

		for (int i = 0; i < IM_ARRAYSIZE(m_Player); i++)
			m_Player[i].m_bIsDead = true;

		bPrevResetState = bResetState;
	}

	if (!g_Local.m_iFOV)
		g_Local.m_iFOV = DEFAULT_FOV;
}