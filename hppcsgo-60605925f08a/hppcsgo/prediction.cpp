#include "main.h"

CPredictionSystem g_PredictionSystem;

void CPredictionSystem::Start(CUserCmd *userCMD, C_BasePlayer* player)
{
	*g_Offsets.predictionRandomSeed = MD5_PseudoRandom(userCMD->command_number) & 0x7FFFFFFF;
	g_Offsets.predictionPlayer = player;

	m_flOldCurTime = g_pGlobalVars->curtime;
	m_flOldFrametime = g_pGlobalVars->frametime;

	g_pGlobalVars->curtime = player->m_nTickBase() * g_pGlobalVars->interval_per_tick;
	g_pGlobalVars->frametime = g_pGlobalVars->interval_per_tick;

	//Here we're doing CBasePlayer::UpdateButtonState // NOTE: hard to tell when offsets changed, think of more longterm solution or just dont do this.
	moveData.m_nButtons = userCMD->buttons;
	int buttonsChanged = userCMD->buttons ^ *reinterpret_cast<int*>(uintptr_t(player) + 0x31E8);
	*reinterpret_cast<int*>(uintptr_t(player) + 0x31DC) = (uintptr_t(player) + 0x31E8);
	*reinterpret_cast<int*>(uintptr_t(player) + 0x31E8) = userCMD->buttons;
	*reinterpret_cast<int*>(uintptr_t(player) + 0x31E0) = userCMD->buttons & buttonsChanged;  //m_afButtonPressed ~ The changed ones still down are "pressed"
	*reinterpret_cast<int*>(uintptr_t(player) + 0x31E4) = buttonsChanged & ~userCMD->buttons; //m_afButtonReleased ~ The ones not down are "released"

	g_pIGameMovement->StartTrackPredictionErrors(player);

	memset(&moveData, 0, sizeof(CMoveData));
	g_pIMoveHelper->SetHost(player);
	g_pIPrediction->SetupMove(player, userCMD, g_pIMoveHelper, &moveData);
	g_pIGameMovement->ProcessMovement(player, &moveData);
	g_pIPrediction->FinishMove(player, userCMD, &moveData);
}

void CPredictionSystem::End(C_BasePlayer* player)
{
	g_pIGameMovement->FinishTrackPredictionErrors(player);
	g_pIMoveHelper->SetHost(nullptr);

	*g_Offsets.predictionRandomSeed = -1;
	g_Offsets.predictionPlayer = nullptr;

	g_pGlobalVars->curtime = m_flOldCurTime;
	g_pGlobalVars->frametime = m_flOldFrametime;
}