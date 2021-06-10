#include "main.h"
#include "notifications.h"
#include "ValveSDK\bf_reader.h"
#include "render\menu\menu.h"
#include "functions\visuals\visuals.h"

static vfunc_hook direct3d_hook;
static vfunc_hook sound_hook;
static vfunc_hook client_hook;
static vfunc_hook client_mode_hook;
static vfunc_hook vguisurf_hook;
static vfunc_hook vguipanel_hook;
static vfunc_hook engine_hook;

IVEngineClient*			g_pIEngine = nullptr;
IEngineTrace*			g_pIEngineTrace = nullptr;
IBaseClientDLL*			g_pIClient = nullptr;
IClientEntityList*		g_pIEntityList = nullptr;
IClientMode*			g_pIClientMode = nullptr;
ICvar*					g_pICvar = nullptr;
CGlobalVarsBase*		g_pGlobalVars = nullptr;
IPanel*					g_pIVGuiPanel = nullptr;
IVDebugOverlay*			g_pIDebugOverlay = nullptr;
ISurface*				g_pIVGuiSurface = nullptr;
CInput*					g_pInput = nullptr;
IVModelInfoClient*		g_pIModelInfo = nullptr;
IVModelRender*			g_pIModelRender = nullptr;
IVRenderView*			g_pIRenderView = nullptr;
IMaterialSystem*		g_pIMatSystem = nullptr;
IGameEventManager2*		g_pIGameEvents = nullptr;
IMoveHelper*			g_pIMoveHelper = nullptr;
IMDLCache*				g_pIMdlCache = nullptr;
IPrediction*			g_pIPrediction = nullptr;
CGameMovement*			g_pIGameMovement = nullptr;
IEngineSound*			g_pIEngineSound = nullptr;
CGlowObjectManager*		g_pGlowObjManager = nullptr;
IViewRender*			g_pIViewRender = nullptr;
IDirect3DDevice9*		g_pID3DDevice9 = nullptr;
CClientState*			g_pClientState = nullptr;
IPhysicsSurfaceProps*	g_pIPhysSurface = nullptr;
IInputSystem*			g_pIInputSystem = nullptr;
ISteamClient*			g_pISteamClient = nullptr;
ISteamScreenshots*		g_pISteamScreenshots = nullptr;
ISteamUtils*			g_pISteamUtils = nullptr;
ISteamFriends*			g_pISteamFriends = nullptr;
ISteamUser*				g_pISteamUser = nullptr;
C_LocalPlayer			g_pLocalPlayer;
CLocalInfo				g_Local;
CPlayerInfo				g_Player;
CWeaponInfo				g_Weapon;

static void __stdcall hkEmitSound1(IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSoundEntry, unsigned int nSoundEntryHash,
	const char* pSample, float flVolume, int nSeed, float flAttenuation, int iFlags, int iPitch, const Vector* pOrigin, const Vector* pDirection,
	void* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, int unk)
{
	static const auto oEmitSound1 = sound_hook.get_original<EmitSound1>(index::EmitSound1);

	oEmitSound1(g_pIEngineSound, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, nSeed, flAttenuation,iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity, unk);
}

static void __stdcall hkFrameStageNotify(ClientFrameStage_t stage)
{
	static const auto oFrameStageNotify = client_hook.get_original<FrameStageNotify>(index::FrameStageNotify);

	oFrameStageNotify(g_pIClient, stage);

	if (IsUnloadingCheatDLL())
		return;

	if (g_Local->m_IsConnected && !g_pIEngine->IsConnected())
	{
		CheckTime();

		g_Local.Clear();
		g_Player.Clear();
		g_Weapon.Clear();
	}

	if (g_pIEngine->IsConnected())
	{
		switch (stage)
		{
		case FRAME_NET_UPDATE_END:
			g_Visuals.GetActiveSounds();
			g_LagComp.Store();
			break;
		}
	}
}

static void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket)
{
	static const auto oCreateMove = client_hook.get_original<CreateMove>(index::CreateMove);

	oCreateMove(g_pIClient, sequence_number, input_sample_frametime, active);

	auto* cmd = g_pInput->GetUserCmd(sequence_number);
	auto* verified = g_pInput->GetVerifiedCmd(sequence_number);

	if (!cmd || !cmd->command_number || IsUnloadingCheatDLL() || !g_pLocalPlayer)
		return;

	g_PredictionSystem.Start(cmd, g_pLocalPlayer);
	{
		g_Local.Update(cmd);
		g_Player.Update();
		g_Weapon.Update();
		//g_LagComp.Store();

		if (!g_Local->m_IsDead)
		{
			g_Misc.AutoRevolver(cmd);
			g_Aimbot.Run(cmd);
			g_Misc.AutoReload(cmd);
			g_Misc.AutoPistol(cmd);
			g_Misc.FakeLag(cmd, bSendPacket);
			g_Misc.AntiAim(cmd, bSendPacket);
		}
	}
	g_PredictionSystem.End(g_pLocalPlayer);

	g_Utils.NormalizeAngles(cmd->viewangles);
	g_Utils.ClampAngles(cmd->viewangles);

	verified->m_cmd = *cmd;
	verified->m_crc = cmd->GetChecksum();
}

__declspec(naked) static void __stdcall hkCreateMove_Proxy(int sequence_number, float input_sample_frametime, bool active)
{
	__asm
	{
		push ebp
		mov  ebp, esp
		push ebx
		lea  ecx, [esp]
		push ecx
		push dword ptr[active]
		push dword ptr[input_sample_frametime]
		push dword ptr[sequence_number]
		call hkCreateMove
		pop  ebx
		pop  ebp
		retn 0Ch
	}
}

static void __stdcall hkLockCursor()
{
	static const auto oLockCursor = vguisurf_hook.get_original<LockCursor_t>(index::LockCursor);

	g_Menu.DrawCursor();

	if (g_Menu.IsOpened())
		return;

	oLockCursor(g_pIVGuiSurface);
}

static long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	static const auto oReset = direct3d_hook.get_original<Reset>(index::Reset);

	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto ret = oReset(device, pPresentationParameters);

	if (ret >= 0)
		ImGui_ImplDX9_CreateDeviceObjects();

	return ret;
}

static long __stdcall hkEndScene(IDirect3DDevice9 * pDevice)
{
	static const auto oEndScene = direct3d_hook.get_original<EndScene>(index::EndScene);

	if (!IsUnloadingCheatDLL() && !g_pIEngine->IsTakingScreenshot() && ImGui::GetCurrentContext())
	{
		DWORD colorwrite, srgbwrite;
		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
		pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);

		g_pIEngine->GetScreenSize(g_Globals.m_iScreenWidth, g_Globals.m_iScreenHeight);
		g_Globals.m_flScreenWidth = static_cast<float>(g_Globals.m_iScreenWidth);
		g_Globals.m_flScreenHeight = static_cast<float>(g_Globals.m_iScreenHeight);

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		auto* pDrawList = g_Render.RenderScene();

		g_Menu.Draw();
		g_Notification.Draw();

		ImGui::Render(pDrawList);
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData(), g_pIEngine->IsInGame());

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);
	}

	return oEndScene(pDevice);
}

static void __stdcall hkPaintTraverse(vgui::VPANEL panel, bool forceRepaint, bool allowForce)
{
	static auto oPaintTraverse = vguipanel_hook.get_original<PaintTraverse>(index::PaintTraverse);

	oPaintTraverse(g_pIVGuiPanel, panel, forceRepaint, allowForce);

	static unsigned FocusOverlayPanel;

	if (!FocusOverlayPanel)
	{
		if (!strcmp(g_pIVGuiPanel->GetName(panel), "FocusOverlayPanel"))
			FocusOverlayPanel = panel;
	}
	else
	{
		if (FocusOverlayPanel == panel)
		{
			static int nMaxRenderInterval, nRenderIntervalCount = INT_MAX;

			if (nRenderIntervalCount < nMaxRenderInterval)
			{
				nRenderIntervalCount++;
				return;
			}

			if (!g_pGlobalVars || g_pGlobalVars->frametime <= 0.f)
				return;

			if (!cvar.visuals.max_render_calls)
				return;

			const int nFramesPerSecond = static_cast<int>(floor(1.f / g_pGlobalVars->frametime));
			nMaxRenderInterval = nFramesPerSecond / cvar.visuals.max_render_calls;
			nRenderIntervalCount = 0;

			auto LRenderScene = [&]() -> void
			{
				if (cvar.visuals.active && g_Local->m_IsConnected)
					g_Visuals.Draw();

				g_Menu.FadeBackgroundInit(g_Menu.IsOpened());
				g_Menu.FadeBackgroundRun();


			};

			g_Render.BeginScene(LRenderScene);
		}
	}
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc_Hooked(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_KEYDOWN && wParam == VK_INSERT)
		g_Menu.Toggle();

	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		if (g_Menu.IsOpened() && !g_Menu.IsKeyAssignment())
			return FALSE;
	}

	return CallWindowProcA(g_pWndProc, hWnd, msg, wParam, lParam);
}

static bool __fastcall hkIsHltv(void)
{
	static const auto accumulate_layers_call = g_Offsets.FindDownPattern((PCHAR)"\x84\xC0\x75\x0D\xF6\x87", g_Offsets.client_panorama.base, g_Offsets.client_panorama.end);
	static const auto oIsHltv = engine_hook.get_original<IsHltv_t>(index::IsHltv);

	const auto ret = oIsHltv();

	if (!g_pIEngine->IsInGame() || !g_pLocalPlayer)
		return ret;

	if ((uintptr_t)_ReturnAddress() == accumulate_layers_call)
	{
		//g_pICvar->ConsolePrintf("patch 0x%X\n", accumulate_layers_call);
		return true;
	}

	return ret;
}

// TODO: Ubrat' eto otsuda
static bool GetKeyToggle(const int keyvar, const int keynum, const int down, bool* state)
{
	if (down && keynum == keyvar)
	{
		*state = !*state;
		return true;
	}

	return false;
}

static int __stdcall hkKeyInput(int down, ButtonCode_t keynum, const char* pszCurrentBinding)
{
	static const auto oKeyInput = client_hook.get_original<KeyInput_t>(index::KeyInput);

	if (cvar.ragebot.active_panic_key && GetKeyToggle(cvar.ragebot.active_panic_key, keynum, down, &cvar.ragebot.active))
		return 0;

	return oKeyInput(down, keynum, pszCurrentBinding);
}

static bool __stdcall hkDispatchUserMessage(int msg_type, int unk1, int nBytes, void* msg_data)
{
	static const auto oDispatchUserMessage = client_hook.get_original<DispatchUserMessage>(index::DispatchUserMessage);
	return oDispatchUserMessage(g_pIClient, msg_type, unk1, nBytes, msg_data);
}

bool InstallHooks()
{
	g_Render.Initialize();

	if (!client_hook.setup(g_pIClient))
		return false;

	if (!engine_hook.setup(g_pIEngine))
		return false;

	if (!sound_hook.setup(g_pIEngineSound))
		return false;

	if (!vguisurf_hook.setup(g_pIVGuiSurface))
		return false;

	if (!vguipanel_hook.setup(g_pIVGuiPanel))
		return false;

	if (!direct3d_hook.setup(g_pID3DDevice9))
		return false;

	g_pWndProc = reinterpret_cast<WNDPROC>(SetWindowLongA(g_Globals.m_hWnd, GWLP_WNDPROC, reinterpret_cast<LONG>(WndProc_Hooked)));

	if (!g_pWndProc)
	{
		TraceLog("%s: wndproc handler not hooked.\n", __FUNCTION__);
		return false;
	}

	client_hook.hook_index(index::CreateMove, hkCreateMove_Proxy);
	client_hook.hook_index(index::FrameStageNotify, hkFrameStageNotify);
	client_hook.hook_index(index::KeyInput, hkKeyInput);
	client_hook.hook_index(index::DispatchUserMessage, hkDispatchUserMessage);
	sound_hook.hook_index(index::EmitSound1, hkEmitSound1);
	direct3d_hook.hook_index(index::Reset, hkReset);
	direct3d_hook.hook_index(index::EndScene, hkEndScene);
	vguipanel_hook.hook_index(index::PaintTraverse, hkPaintTraverse);
	vguisurf_hook.hook_index(index::LockCursor, hkLockCursor);
	engine_hook.hook_index(index::IsHltv, hkIsHltv);

	g_GameEvents->Register();

	return true;
}

void DeleteHooks()
{
	g_Globals.m_bUnloadLibrary = true;
	g_Globals.m_bLicenseChecked = false;
	CloseSocket();

	g_Menu.Close();

	if (g_pID3DDevice9 && g_pWndProc)
		SetWindowLongA(g_Globals.m_hWnd, GWL_WNDPROC, reinterpret_cast<LONG>(g_pWndProc));

	g_GameEvents->Unregister();

	direct3d_hook.unhook_all();
	sound_hook.unhook_all();
	client_hook.unhook_all();
	vguisurf_hook.unhook_all();
	vguipanel_hook.unhook_all();
	engine_hook.unhook_all();

	Sleep(1000);

	g_Render.Shutdown();

	//g_pICvar->FindVar("crosshair")->SetValue(true);

	Sleep(1000);

	FreeLibraryAndExitThread(g_Globals.m_hModule, 0);
}