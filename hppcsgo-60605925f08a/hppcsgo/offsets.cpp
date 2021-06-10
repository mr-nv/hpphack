#include "main.h"

COffsets g_Offsets;

bool COffsets::FindModules()
{
	if (!FindModuleByName("client_panorama.dll", &client_panorama))
		return false;

	if (!FindModuleByName("engine.dll", &engine))
		return false;

	if (!FindModuleByName("shaderapidx9.dll", &shaderapidx9))
		return false;

	if (!FindModuleByName("vstdlib.dll", &vstdlib))
		return false;

	if (!FindModuleByName("vguimatsurface.dll", &vguimatsurface))
		return false;

	if (!FindModuleByName("vgui2.dll", &vgui2))
		return false;

	if (!FindModuleByName("materialsystem.dll", &materialsystem))
		return false;

	if (!FindModuleByName("datacache.dll", &datacache))
		return false;

	if (!FindModuleByName("vphysics.dll", &vphysics))
		return false;

	if (!FindModuleByName("inputsystem.dll", &inputsystem))
		return false;

	if (!FindModuleByName("steamclient.dll", &steamclient))
		return false;

	return true;
}

bool COffsets::FindInterfaces()
{
	const auto factory_client_panorama = CaptureFactory(&client_panorama);
	const auto factory_engine = CaptureFactory(&engine);
	const auto factory_shaderapidx9 = CaptureFactory(&shaderapidx9);
	const auto factory_vstdlib = CaptureFactory(&vstdlib);
	const auto factory_vguimatsurface = CaptureFactory(&vguimatsurface);
	const auto factory_vgui2 = CaptureFactory(&vgui2);
	const auto factory_materialsystem = CaptureFactory(&materialsystem);
	const auto factory_datacache = CaptureFactory(&datacache);
	const auto factory_vphysics = CaptureFactory(&vphysics);
	const auto factory_inputsystem = CaptureFactory(&inputsystem);
	const auto factory_steamclient = CaptureFactory(&steamclient);

	if (!factory_client_panorama || !factory_engine || !factory_shaderapidx9 || !factory_vstdlib || !factory_vguimatsurface || !factory_vgui2 || !factory_materialsystem || !factory_datacache || !factory_vphysics || !factory_inputsystem)
		return false;

	g_pIClient = (IBaseClientDLL*)CaptureInterface(factory_client_panorama, "VClient018");
	g_pIEntityList = (IClientEntityList*)CaptureInterface(factory_client_panorama, "VClientEntityList003");
	g_pIPrediction = (IPrediction*)CaptureInterface(factory_client_panorama, "VClientPrediction001");
	g_pIGameMovement = (CGameMovement*)CaptureInterface(factory_client_panorama, "GameMovement001");
	g_pIMdlCache = (IMDLCache*)CaptureInterface(factory_datacache, "MDLCache004");
	g_pIEngine = (IVEngineClient*)CaptureInterface(factory_engine, "VEngineClient014");
	g_pIModelInfo = (IVModelInfoClient*)CaptureInterface(factory_engine, "VModelInfoClient004");
	g_pIModelRender = (IVModelRender*)CaptureInterface(factory_engine, "VEngineModel016");
	g_pIRenderView = (IVRenderView*)CaptureInterface(factory_engine, "VEngineRenderView014");
	g_pIEngineTrace = (IEngineTrace*)CaptureInterface(factory_engine, "EngineTraceClient004");
	g_pIDebugOverlay = (IVDebugOverlay*)CaptureInterface(factory_engine, "VDebugOverlay004");
	g_pIGameEvents = (IGameEventManager2*)CaptureInterface(factory_engine, "GAMEEVENTSMANAGER002");
	g_pIEngineSound = (IEngineSound*)CaptureInterface(factory_engine, "IEngineSoundClient003");
	g_pIMatSystem = (IMaterialSystem*)CaptureInterface(factory_materialsystem, "VMaterialSystem080");
	g_pICvar = (ICvar*)CaptureInterface(factory_vstdlib, "VEngineCvar007");
	g_pIVGuiPanel = (IPanel*)CaptureInterface(factory_vgui2, "VGUI_Panel009");
	g_pIVGuiSurface = (ISurface*)CaptureInterface(factory_vguimatsurface, "VGUI_Surface031");
	g_pIPhysSurface = (IPhysicsSurfaceProps*)CaptureInterface(factory_vphysics, "VPhysicsSurfaceProps001");
	g_pIInputSystem = (IInputSystem*)CaptureInterface(factory_inputsystem, "InputSystemVersion001");

	g_pGlobalVars		= **(CGlobalVarsBase***)((*(uintptr_t**)g_pIClient)[0] + 0x1B);
	g_pIClientMode		= **(IClientMode***)((*(uintptr_t**)g_pIClient)[10] + 0x5u);
	g_pInput			= *(CInput**)(FindDownPattern((PCHAR)"\xB9\xFF\xFF\xFF\xFF\xF3\x0F\x11\x04\x24\xFF\x50\x10", (PCHAR)"x????xxxxxxxx", client_panorama.base, client_panorama.end, 1));
	g_pIMoveHelper		= **(IMoveHelper***)(FindDownPattern((PCHAR)"\x8B\x0D\xFF\xFF\xFF\xFF\x8B\x45\xFF\x51\x8B\xD4\x89\x02\x8B\x01", (PCHAR)"xx????xx?xxxxxxx", client_panorama.base, client_panorama.end, 2));
	g_pGlowObjManager	= *(CGlowObjectManager**)(FindDownPattern((PCHAR)"\x0F\x11\x05\xFF\xFF\xFF\xFF\x83\xC8\x01", (PCHAR)"xxx????xxx", client_panorama.base, client_panorama.end, 3));
	g_pIViewRender		= *(IViewRender**)(FindDownPattern((PCHAR)"\xA1\xFF\xFF\xFF\xFF\xB9\xFF\xFF\xFF\xFF\xC7\x05\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x10", (PCHAR)"x????x????xx????????xx", client_panorama.base, client_panorama.end, 1));
	g_pID3DDevice9		= **(IDirect3DDevice9***)(FindDownPattern((PCHAR)"\xA1\xFF\xFF\xFF\xFF\x50\x8B\x08\xFF\x51\x0C", (PCHAR)"x????xxxxxx", shaderapidx9.base, shaderapidx9.end, 1));
	g_pClientState		= **(CClientState***)(FindDownPattern((PCHAR)"\xA1\xFF\xFF\xFF\xFF\x8B\x80\xFF\xFF\xFF\xFF\xC3", (PCHAR)"x????xx????x", engine.base, engine.end, 2));
	//g_pClientState		= **(CClientState***)(FindDownPattern((PCHAR)"\x8B\x3D\xFF\xFF\xFF\xFF\x8A\xF9", (PCHAR)"xx????xx", engine.base, engine.end, 2));

	g_pLocalPlayer		= *(C_LocalPlayer*)(FindDownPattern((PCHAR)"\x8B\x0D\xFF\xFF\xFF\xFF\x83\xFF\xFF\x74\x07", (PCHAR)"xx????xxxxx", client_panorama.base, client_panorama.end, m_dwCloudOffsets[1]));
	
	predictionRandomSeed= *(int**)(FindDownPattern((PCHAR)"\xA3\xFF\xFF\xFF\xFF\x66\x0F\x6E\x86", (PCHAR)"x????xxxx", client_panorama.base, client_panorama.end, 1));
	predictionPlayer	= *reinterpret_cast<C_BasePlayer**>(FindDownPattern((PCHAR)"\x89\x35\xFF\xFF\xFF\xFF\xF3\x0F\x10\x48\x20", (PCHAR)"xx????xxxxx", client_panorama.base, client_panorama.end, 2));

#ifndef LICENSING
	g_pICvar->ConsolePrintf("build: %i\n", g_pIEngine->GetEngineBuildNumber());
#endif
	if (!g_pIClient || !g_pID3DDevice9 || !g_pIEngine || !g_pInput || !g_pIClientMode || !g_pIMoveHelper || !g_pGlobalVars)
		return false;

	/*if (g_pIEngine->GetEngineBuildNumber() != 13703)
		return false;*/

	if (!FindSteamInterfaces(factory_steamclient))
		return false;

	return true;
}

bool COffsets::FindSteamInterfaces(CreateInterfaceFn steamclient)
{
	if (steamclient)
	{
		g_pISteamClient = (ISteamClient*)(CaptureInterface(steamclient, STEAMCLIENT_INTERFACE_VERSION));

		if (g_pISteamClient)
		{
			HSteamPipe hSteamPipe = g_pISteamClient->CreateSteamPipe();

			if (hSteamPipe)
			{
				HSteamUser hSteamUser = g_pISteamClient->ConnectToGlobalUser(hSteamPipe);

				if (hSteamUser)
				{
					g_pISteamScreenshots = g_pISteamClient->GetISteamScreenshots(hSteamUser, hSteamPipe, STEAMSCREENSHOTS_INTERFACE_VERSION);
					g_pISteamUtils = g_pISteamClient->GetISteamUtils(hSteamPipe, STEAMUTILS_INTERFACE_VERSION);
					g_pISteamFriends = g_pISteamClient->GetISteamFriends(hSteamUser, hSteamPipe, STEAMFRIENDS_INTERFACE_VERSION);
					g_pISteamUser = g_pISteamClient->GetISteamUser(hSteamUser, hSteamPipe, STEAMUSER_INTERFACE_VERSION);

					if (g_pISteamScreenshots && g_pISteamUtils && g_pISteamFriends && g_pISteamUser)
						return true;
				}
				else {
					g_pISteamClient->BReleaseSteamPipe(hSteamPipe);
					g_pISteamClient->BShutdownIfAllPipesClosed();
					TraceLog("%s: steamuser not found.\n", __FUNCTION__);
				}
			}
			else
				TraceLog("%s: steampipe not found.\n", __FUNCTION__);
		}
		else
			TraceLog("%s: interface steamclient not found.\n", __FUNCTION__);
	}
	else
		TraceLog("%s: factory steamclient not found.\n", __FUNCTION__);

	return false;
}

uintptr_t COffsets::FindUpPattern(PCHAR pattern, uintptr_t start, uintptr_t end, uintptr_t offset)
{
	if (!start || !end)
		return 0;

	size_t patternLength = strlen(pattern);

	for (auto i = start; i > end - patternLength; --i)
	{
		bool found = true;

		for (size_t idx = 0; idx < patternLength; ++idx)
		{
			if (pattern[idx] != *(PCHAR)(i + idx))
			{
				found = false;
				break;
			}
		}

		if (found)
			return i + offset;
	}

	return 0;
}

uintptr_t COffsets::FindUpPattern(PCHAR pattern, PCHAR mask, uintptr_t start, uintptr_t end, uintptr_t offset)
{
	if (!start || !end)
		return 0;

	size_t patternLength = strlen(pattern);

	for (auto i = start; i > end - patternLength; --i)
	{
		bool found = true;

		for (size_t idx = 0; idx < patternLength; ++idx)
		{
			if (mask[idx] == 'x' && pattern[idx] != *(PCHAR)(i + idx))
			{
				found = false;
				break;
			}
		}

		if (found)
			return i + offset;
	}

	return 0;
}

uintptr_t COffsets::FindDownPattern(PCHAR pattern, PCHAR mask, uintptr_t start, uintptr_t end, uintptr_t offset)
{
	if (!start || !end)
		return 0;

	size_t patternLength = strlen(pattern);

	for (auto i = start; i < end - patternLength; ++i)
	{
		bool found = true;

		for (size_t idx = 0; idx < patternLength; ++idx)
		{
			if (mask[idx] == 'x' && pattern[idx] != *(PCHAR)(i + idx))
			{
				found = false;
				break;
			}
		}

		if (found)
			return i + offset;
	}

	return 0;
}

uintptr_t COffsets::FindDownPattern(PCHAR pattern, uintptr_t start, uintptr_t end, uintptr_t offset)
{
	if (!start || !end)
		return 0;

	size_t patternLength = strlen(pattern);

	for (auto i = start; i < end - patternLength; ++i)
	{
		bool found = true;

		for (size_t idx = 0; idx < patternLength; ++idx)
		{
			if (pattern[idx] != *(PCHAR)(i + idx))
			{
				found = false;
				break;
			}
		}

		if (found)
			return i + offset;
	}

	return 0;
}

bool COffsets::FindModuleByName(const char* name, Module* module)
{
	if (!name || !*name || !module)
		return false;

	HMODULE hModuleDll = GetModuleHandle(name);

	if (hModuleDll == INVALID_HANDLE_VALUE)
		return false;

	MEMORY_BASIC_INFORMATION mem;

	if (!VirtualQuery(hModuleDll, &mem, sizeof(mem)))
		return false;

	if (mem.State != MEM_COMMIT)
		return false;

	if (!mem.AllocationBase)
		return false;

	IMAGE_DOS_HEADER *dos = (IMAGE_DOS_HEADER *)mem.AllocationBase;
	IMAGE_NT_HEADERS *pe = (IMAGE_NT_HEADERS *)((uintptr_t)dos + (uintptr_t)dos->e_lfanew);

	if (pe->Signature != IMAGE_NT_SIGNATURE)
		return false;

	module->handle = hModuleDll;
	module->base = (uintptr_t)(mem.AllocationBase);
	module->size = (uintptr_t)(pe->OptionalHeader.SizeOfImage);
	module->end = module->base + module->size - 1;

	return true;
}