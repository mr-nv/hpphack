#include "main.h"

CMiscellaneous g_Misc;

void CMiscellaneous::AntiAim(CUserCmd* cmd, bool& bSendPacket)
{
	if (!cvar.ragebot.active)
		return;

	//g_pIEngine->Con_NPrintf(8, "choked: %i", g_pClientState->m_nChokedCommands);

	if (g_Weapon->m_IsCanFire && cmd->buttons & IN_ATTACK)
		return;

	if (cmd->buttons & IN_USE)
		return;

	C_BasePlayer *pBestPlayer = nullptr;
	float flBestFOV = FLT_MAX;

	for (auto i = 1; i <= g_pGlobalVars->maxClients; ++i)
	{
		const auto player = (C_BasePlayer*)C_BaseEntity::GetEntityByIndex(i);

		if (!g_Utils.IsValidPlayer(player, cvar.ragebot.aim_friendly_fire))
			continue;

		float flFOV = g_Utils.AngleBetween(g_Local->m_vecForward, g_Player[i]->m_vecOrigin - g_Local->m_vecEyePos);

		if (flFOV > flBestFOV)
			continue;

		flBestFOV = flFOV;
		pBestPlayer = player;
	}

	if (pBestPlayer)
	{
		/*static int kek = 2;

		if (kek > 0)
		{
			bSendPacket = false;
			kek--;
		}
		else
			kek = 2;
		*/

		QAngle QAngles;
		g_Utils.VectorAngles(pBestPlayer->m_vecOrigin() - g_Local->m_vecEyePos, QAngles);

		g_Utils.FixMoveStart(cmd);

		cmd->viewangles.x = 89.f;
		cmd->viewangles.y = QAngles.y;
		cmd->viewangles.y += 180.f;

		g_Utils.FixMoveEnd(cmd);
	}
}

void CMiscellaneous::FakeLag(CUserCmd* cmd, bool& bSendPacket)
{

}

/*void CMiscellaneous::_FakeDuck(CUserCmd* cmd)
{
	int fakelag_limit = vars_t::get().misc.fakelag.limit >= 14 ? 14 : vars_t::get().misc.fakelag.limit;
	int choked_goal = fakelag_limit / 2;
	bool should_crouch = clagmanager::get().getchokedticks() >= choked_goal;
	g_pClientState -
		if (globals::lplayer->getflags() & fl_onground)
		{
			globals::cmd->buttons |= in_bullrush;
			if (should_crouch)
				globals::cmd->buttons |= in_duck;
			else
				globals::cmd->buttons &= ~in_duck;
		}
}*/

void CMiscellaneous::AutoReload(CUserCmd *cmd)
{
	if (cvar.misc.automatic_reload && cmd->buttons & IN_ATTACK && g_Weapon->m_iClip1 < 1 && g_Weapon.IsGun())
	{
		cmd->buttons &= ~IN_ATTACK;
		cmd->buttons |= IN_RELOAD;
	}
}

void CMiscellaneous::AutoPistol(CUserCmd *cmd)
{
	if (cvar.misc.automatic_pistol && cmd->buttons & IN_ATTACK && g_Weapon.IsPistol() && !g_Weapon->m_IsReloading)
	{
		if (!g_Weapon->m_IsCanFire)
			cmd->buttons &= ~IN_ATTACK;
	}
}

void CMiscellaneous::AutoRevolver(CUserCmd *cmd)
{
	if (cvar.misc.automatic_revolver && g_Weapon->m_iCurrentId == WEAPON_REVOLVER && g_Weapon->m_IsCanFire)
	{
		if (!(TIME_TO_TICKS(g_pGlobalVars->curtime - g_Weapon->m_flPostponeFireReadyTime) <= -1))
			return;

		cmd->buttons |= IN_ATTACK;
	}
}