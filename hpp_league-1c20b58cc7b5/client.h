extern cl_clientfunc_t g_Client, * g_pClient;
extern cl_enginefunc_t g_Engine, * g_pEngine;
extern engine_studio_api_t g_Studio, * g_pStudio;
extern playermove_t* pmove;
extern client_state_t* client_state;
extern client_static_t* client_static;
extern extra_player_info_t* g_PlayerExtraInfo;
extern SCREENINFO g_Screen;

void Hook();
void UnHook();

using CreateMove = void(__thiscall*)(cl_clientfunc_t*, float, usercmd_s*, int);