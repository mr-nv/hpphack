#include "main.h"

GameEvents *g_GameEvents = new GameEvents();

GameEvents::GameEvents()
{

}

void GameEvents::Register()
{
	if (g_pIGameEvents && !g_pIGameEvents->FindListener(this, "player_hurt"))
		g_pIGameEvents->AddListener(this, "player_hurt", false);
}

void GameEvents::Unregister()
{
	if(g_pIGameEvents)
		g_pIGameEvents->RemoveListener(this);
}

void GameEvents::FireGameEvent(IGameEvent* event)
{
	if (!event)
		return;

	const char *name = event->GetName();

	if (strstr(name, "player_death"))
	{
		const auto userid = event->GetInt("userid");
		const auto attacker = event->GetInt("attacker");
		const auto assister = event->GetInt("assister");
		const auto weapon = event->GetString("weapon");
		const auto weapon_itemid = event->GetString("weapon_itemid");
		const auto weapon_fauxitemid = event->GetString("weapon_fauxitemid");
		const auto weapon_originalowner_xuid = event->GetString("weapon_originalowner_xuid");
		const auto headshot = event->GetBool("headshot");
		const auto dominated = event->GetInt("dominated");
		const auto revenge = event->GetInt("revenge");
		const auto penetrated = event->GetInt("penetrated");
		const auto noreplay = event->GetBool("noreplay");

		if (g_pIEngine->GetPlayerForUserID(attacker) == g_Local->m_iEntIndex && g_pIEngine->GetPlayerForUserID(userid) != g_Local->m_iEntIndex)
			g_Aimbot.m_flLastTimeKilled = g_pGlobalVars->curtime;
	}
	else if (strstr(name, "player_hurt"))
	{
		const auto userid = event->GetInt("userid");
		const auto attacker = event->GetInt("attacker");
		const auto health = event->GetInt("health");
		const auto armor = event->GetInt("armor");
		const auto weapon = event->GetString("weapon");
		const auto dmg_health = event->GetInt("dmg_health");
		const auto dmg_armor = event->GetInt("dmg_armor");
		const auto hitgroup = event->GetInt("hitgroup");

		if (health == 0 && g_pIEngine->GetPlayerForUserID(attacker) == g_Local->m_iEntIndex && g_pIEngine->GetPlayerForUserID(userid) != g_Local->m_iEntIndex)
			g_Aimbot.m_flLastTimeKilled = g_pGlobalVars->curtime;
	}
}

int GameEvents::GetEventDebugID()
{
	return EVENT_DEBUG_ID_INIT;
}