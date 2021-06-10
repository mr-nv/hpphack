#include "main.h"

COffsets g_Offsets;

bool COffsets::FindModules(void)
{
	if (!FindModuleByName("hw.dll", &hw))
		return false;

	if (!FindModuleByName("client.dll", &client))
		return false;

	return true;
}

bool COffsets::FindInterfaces(void)
{
	auto factory_hw = CaptureFactory(&hw);

	if (!factory_hw)
		return false;

	return true;
}

bool COffsets::FindOffsets(void)
{
	if (!(g_pClient = (cl_clientfunc_t*)(Client())))
		return false;

	if (!(g_pEngine = (cl_enginefunc_t*)(Engine())))
		return false;

	if (!(g_pStudio = (engine_studio_api_t*)(Studio())))
		return false;

	if (!(pmove = (playermove_t*)(PlayerMove())))
		return false;

	if (!(client_state = (client_state_t*)(ClientState())))
		return false;

	if (!(client_static = (client_static_t*)(ClientStatic())))
		return false;

	if (!(g_PlayerExtraInfo = (extra_player_info_t*)(ExtraPlayerInfo())))
		return false;

	if (!(m_dwInitPoint = FindInitPoint()))
		return false;

	RtlCopyMemory(&g_Client, g_pClient, sizeof(g_Client));
	RtlCopyMemory(&g_Engine, g_pEngine, sizeof(g_Engine));
	RtlCopyMemory(&g_Studio, g_pStudio, sizeof(g_Studio));

	return true;
}

uintptr_t COffsets::FindInitPoint(void)
{
	auto ptr = FindDownPattern((PCHAR)"\x8B\x44\x24\x04\x48\x83\xF8\x1D", client.base, client.end, 21);

	if (AddressNotInSpace(ptr, client.base, client.end))
		return NULL;

	return ptr;
}

uintptr_t COffsets::ExtraPlayerInfo(void)
{
	auto ptr = (uintptr_t)(g_pClient->HUD_GETPLAYERTEAM_FUNCTION);

	ptr = FindUpPattern((PCHAR)"\x0F\xBF\x04\xFF\xFF\xFF\xFF\xFF\xC3", (PCHAR)"xxx?????x", ptr, client.base);

	ptr = *(uintptr_t*)(ptr + 4);

	ptr = (ptr - offsetof(extra_player_info_t, team_id));

	if (AddressNotInSpace(ptr, client.base, client.end))
		return NULL;
	
	return ptr;
}

PVOID COffsets::Studio(void)
{
	auto ptr_studio = (engine_studio_api_t*) * (DWORD*)((DWORD)g_pClient->HUD_GetStudioModelInterface + 0x30);

	if (AddressNotInSpace((uintptr_t)(ptr_studio), client.base, client.end))
	{
		ptr_studio = (engine_studio_api_t*) * (DWORD*)((DWORD)g_pClient->HUD_GetStudioModelInterface + 0x1A);

		if (AddressNotInSpace((uintptr_t)(ptr_studio), client.base, client.end))
			return NULL;
	}

	return ptr_studio;
}

PVOID COffsets::Engine(void)
{
	auto ptr = *(PDWORD)(FindPush((PCHAR) "sprites/voiceicon.spr", client.base, client.end) + 0x07);

	if (AddressNotInSpace(ptr, client.base, client.end))
		return NULL;

	return (PVOID)(ptr);
}

PVOID COffsets::Client(void)
{
	auto ptr = FindDownPattern((PCHAR) "ScreenFade", hw.base, hw.end);
	auto ptr_client = (PVOID) * (PDWORD)(FindReference(hw.base, hw.end, ptr) + 0x13); // 0x13 for cloud

	if (AddressNotInSpace((uintptr_t)(ptr_client), hw.base, hw.end))
		return NULL;
	return ptr_client;
}

PVOID COffsets::PlayerMove(void)
{
	auto ptr = FindDownPattern((PCHAR) "ScreenFade", hw.base, hw.end);
	auto ptr_player_move = (PVOID) * (PDWORD)(FindReference(hw.base, hw.end, ptr) + 0x18);

	if (AddressNotInSpace((uintptr_t)ptr_player_move, hw.base, hw.end))
		return NULL;

	return ptr_player_move;
}

PVOID COffsets::ClientStatic(void)
{
	auto ptr = FindPush((PCHAR) "in :  %i %.2f k/s", hw.base, hw.end);

	ptr = FindUpPattern((PCHAR) "\xA1\xFF\xFF\xFF\xFF\x83\xEC\x08", (PCHAR) "x????xxx", ptr, hw.base);

	ptr = *(uintptr_t*)(ptr + 1);

	ptr = (uintptr_t)(client_static_t*)(ptr - offsetof(client_static_t, netchan.incoming_sequence));

	if (AddressNotInSpace(ptr, hw.base, hw.end))
		return NULL;

	return (PVOID)ptr;
}

PVOID COffsets::ClientState(void)
{
	auto ptr = FindDownPattern((PCHAR)"\x8D\x34\x95\xFF\xFF\xFF\xFF\x56\xC7\x46\x38\x00\x00\x80\xBF\x89\x7E\x40\x89\x7E\x44\x89\x7E\x48", (PCHAR)"xxx????xxxxxxxxxxxxxxxxx", hw.base, hw.end);

	ptr = *(DWORD*)(ptr + 3);

	ptr = (uintptr_t)(client_state_t*)(ptr - offsetof(client_state_t, commands));

	if (AddressNotInSpace(ptr, hw.base, hw.end))
		return NULL;

	return (PVOID)ptr;
}

uintptr_t COffsets::FindPush(PCHAR message, uintptr_t start, uintptr_t end, uintptr_t offset)
{
	char bPushAddrPattern[] = { 0x68, 0x00, 0x00, 0x00, 0x00, 0x00 };
	auto ptr = FindDownPattern(message, start, end);
	*(PDWORD)& bPushAddrPattern[1] = ptr;
	ptr = FindDownPattern((PCHAR)bPushAddrPattern, start, end);
	return ptr + offset;
}

bool COffsets::AddressNotInSpace(uintptr_t ptr, uintptr_t low, uintptr_t high)
{
	if (low > high)
	{
		auto reverse = high;
		high = low;
		low = reverse;
	}

	return ((ptr < low) || (ptr > high));
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

	IMAGE_DOS_HEADER * dos = (IMAGE_DOS_HEADER*)mem.AllocationBase;
	IMAGE_NT_HEADERS * pe = (IMAGE_NT_HEADERS*)((uintptr_t)dos + (uintptr_t)dos->e_lfanew);

	if (pe->Signature != IMAGE_NT_SIGNATURE)
		return false;

	module->handle = hModuleDll;
	module->base = (uintptr_t)(mem.AllocationBase);
	module->size = (uintptr_t)(pe->OptionalHeader.SizeOfImage);
	module->end = module->base + module->size - 1;

	return true;
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

uintptr_t COffsets::FindReference(uintptr_t start, uintptr_t end, uintptr_t ptr)
{
	char szPattern[] = { 0x68, 0x00, 0x00, 0x00, 0x00, 0x00 };
	*(PDWORD)& szPattern[1] = ptr;
	return FindDownPattern(szPattern, start, end, 0);
}