#include "main.h"

cl_clientfunc_t g_Client, * g_pClient = nullptr;
cl_enginefunc_t g_Engine, * g_pEngine = nullptr;
engine_studio_api_t g_Studio, * g_pStudio = nullptr;
playermove_t* pmove = nullptr;
client_state_t* client_state = nullptr;
client_static_t* client_static = nullptr;
extra_player_info_t* g_PlayerExtraInfo = nullptr;

SCREENINFO g_Screen;

static void CL_CreateMove(float frametime, usercmd_s* cmd, int active)
{
	g_Client.CL_CreateMove(frametime, cmd, active);

	g_World.Update();
	g_Weapon.Update();

	if (!g_Local.m_bIsDead)
	{
		g_Aimbot.Run(cmd);
	}
}

static void HUD_PostRunCmd(local_state_s* from, local_state_s* to, usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed)
{
	g_Client.HUD_PostRunCmd(from, to, cmd, runfuncs, time, random_seed);

	g_Weapon.ItemPreFrame(to, cmd, runfuncs, time, random_seed);
}

static void StudioSetRemapColors (int top, int bottom)
{
	g_Utils.GetHitboxes(g_pStudio->GetCurrentEntity());

	g_Studio.StudioSetRemapColors(top, bottom);
}

static void HUD_Frame(double time)
{
	g_World.Reset();

	g_Screen.iSize = sizeof(SCREENINFO);
	g_Engine.pfnGetScreenInfo(&g_Screen);

	

	g_Client.HUD_Frame(time);
}

static void HUD_Redraw(float time, int intermission)
{
	static bool active = false;

	g_Client.HUD_Redraw(time, intermission);

	if (GetAsyncKeyState(VK_F5) & 0x1)
		active = !active;

	if (!active)
		return;

	for (int i = 1; i < g_Engine.GetMaxClients(); i++)
	{
		if (g_Local.m_iIndex == i)
			continue;

		const auto* pPlayer = g_World.GetPlayer(i);

		if (g_Local.m_iTeam == pPlayer->m_iTeam)
			continue;

		if (pPlayer->m_bIsDead)
			continue;

		if (!pPlayer->m_bIsInPVS)
			continue;

		Vector vecTop = pPlayer->m_vecHitbox[11];
		vecTop.z += 9.f;

		Vector vecBot = pPlayer->m_vecOrigin;
		vecBot.z += pPlayer->m_vecBoundBoxMins.z - 1.f;

		float flScreenBot[2], flScreenTop[2];

		if (!g_Utils.CalcScreen(vecTop, flScreenTop) || 
			!g_Utils.CalcScreen(vecBot, flScreenBot))
			continue;

		int h = static_cast<int>(floorf(flScreenBot[1] - flScreenTop[1]));
		int w = static_cast<int>(floorf(h * 0.55f));
		int x = static_cast<int>(floorf(flScreenBot[0] - w * 0.5f));
		int y = static_cast<int>(floorf(flScreenTop[1]));

		g_Drawing.DrawBox(x, y, w, h, 1, 64, 255, 64, 255);
	}
}

void Hook()
{
	g_pClient->CL_CreateMove = CL_CreateMove;
	g_pClient->HUD_PostRunCmd = HUD_PostRunCmd;
	g_pClient->HUD_Frame = HUD_Frame;
	g_pClient->HUD_Redraw = HUD_Redraw;

	g_pStudio->StudioSetRemapColors = StudioSetRemapColors;
	//g_pEngine->Con_Printf("hooked\n");
}

void UnHook()
{
	if (g_pStudio)
		* g_pStudio = g_Studio;

	if (g_pClient)
		* g_pClient = g_Client;

	if (g_pEngine)
		* g_pEngine = g_Engine;
}