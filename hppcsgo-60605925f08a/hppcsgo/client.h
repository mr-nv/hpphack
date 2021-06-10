bool InstallHooks(void);
void DeleteHooks(void);

using EndScene = long(__stdcall *)(IDirect3DDevice9*);
using Reset = long(__stdcall *)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
using LockCursor_t = void(__thiscall*)(ISurface*);
using EmitSound1 = void(__thiscall*)(void*, IRecipientFilter&, int, int, const char*, unsigned int, 
	const char*, float, int, float, int, int, const Vector*, const Vector*, void*, bool, float, int, int);
using CreateMove = void(__thiscall*)(IBaseClientDLL*, int, float, bool);
using FrameStageNotify = void(__thiscall*)(IBaseClientDLL*, ClientFrameStage_t);
using PaintTraverse = void(__thiscall*)(IPanel*, vgui::VPANEL, bool, bool);
using IsHltv_t = bool(__thiscall *)();
using KeyInput_t = int(__stdcall*)(int, ButtonCode_t, const char*);
using DispatchUserMessage = bool(__thiscall*)(void*, int, int, int, void*);

class C_LocalPlayer
{
	friend bool operator==(const C_LocalPlayer& lhs, void* rhs);
public:
	C_LocalPlayer() : m_local(nullptr) {}

	operator bool() const { return *m_local != nullptr; }
	operator C_BasePlayer* () const { return *m_local; }

	C_BasePlayer* operator->() { return *m_local; }

private:
	C_BasePlayer** m_local;
};

extern IVEngineClient*       g_pIEngine;
extern IBaseClientDLL*       g_pIClient;
extern IClientEntityList*    g_pIEntityList;
extern CGlobalVarsBase*      g_pGlobalVars;
extern IEngineTrace*         g_pIEngineTrace;
extern ICvar*                g_pICvar;
extern IPanel*               g_pIVGuiPanel;
extern IClientMode*          g_pIClientMode;
extern IVDebugOverlay*       g_pIDebugOverlay;
extern ISurface*             g_pIVGuiSurface;
extern CInput*               g_pInput;
extern IVModelInfoClient*    g_pIModelInfo;
extern IVModelRender*        g_pIModelRender;
extern IVRenderView*         g_pIRenderView;
extern IMaterialSystem*      g_pIMatSystem;
extern IGameEventManager2*   g_pIGameEvents;
extern IMoveHelper*          g_pIMoveHelper;
extern IMDLCache*            g_pIMdlCache;
extern IPrediction*          g_pIPrediction;
extern CGameMovement*        g_pIGameMovement;
extern IEngineSound*         g_pIEngineSound;
extern CGlowObjectManager*   g_pGlowObjManager;
extern IViewRender*          g_pIViewRender;
extern IDirect3DDevice9*     g_pID3DDevice9;
extern CClientState*         g_pClientState;
extern IPhysicsSurfaceProps* g_pIPhysSurface;
extern IInputSystem*         g_pIInputSystem;
extern C_LocalPlayer		 g_pLocalPlayer;
extern ISteamClient			*g_pISteamClient;
extern ISteamScreenshots	*g_pISteamScreenshots;
extern ISteamUtils			*g_pISteamUtils;
extern ISteamFriends		*g_pISteamFriends;
extern ISteamUser			*g_pISteamUser;

extern WNDPROC				 g_pWndProc;