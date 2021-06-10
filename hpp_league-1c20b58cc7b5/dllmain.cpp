#include "main.h"

static const auto user = 100609239;

static unsigned __int16 getVolumeHash() 
{
	DWORD serialNum = 0;
	GetVolumeInformation(NULL, NULL, 0, &serialNum, NULL, NULL, NULL, 0);
	unsigned __int16 hash = (unsigned __int16)((serialNum + (serialNum >> 16)) & 0xFFFF);
	return hash;
}

static unsigned __int16 getCpuHash()
{
	int cpuinfo[4] = { 0, 0, 0, 0 };
	__cpuid(cpuinfo, 0);
	unsigned __int16 hash = 0;
	unsigned __int16* ptr = (unsigned __int16*)(&cpuinfo[0]);

	for (unsigned __int32 i = 0; i < 8; i++)
		hash += ptr[i];

	return hash;
}

DWORD WINAPI SetupHooks(LPVOID lpThreadParameter)
{
	while (true)
	{
		if (g_Offsets.FindModules() && g_Offsets.FindInterfaces())
			break;

		Sleep(100);
	}

	auto hwid = 1337 * getVolumeHash() + 228 * getCpuHash() + 1488 * getCpuHash() + 696 * getVolumeHash();

	if (hwid == user)
	{
		Sleep(5000);

		if (g_Offsets.FindOffsets())
		{
			g_pEngine->Con_Printf("injected\n");

			Hook();

			while (true)
			{
				if (GetAsyncKeyState(VK_END))
					break;

				Sleep(1);
			}
		}
	}

	UnHook();

	Sleep(1000);

	FreeLibraryAndExitThread((HMODULE)lpThreadParameter, 0);
	return TRUE;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			FreeLibraryAndExitThread(hinstDLL, 0);
			return FALSE;
		}

		DisableThreadLibraryCalls(hinstDLL);

		auto hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)SetupHooks, hinstDLL, 0, 0);

		if (!hThread || !HideThread(hThread))
		{
			FreeLibraryAndExitThread(hinstDLL, 0);
			return FALSE;
		}

		CloseHandle(hinstDLL);
	}

	return TRUE;
}

bool HideThread(HANDLE hThread)
{
	const auto NtDll = GetModuleHandle("ntdll.dll");

	if (!NtDll)
		return false;

	const auto NtSIT = (pNtSetInformationThread)GetProcAddress(NtDll, "NtSetInformationThread");

	if (!NtSIT)
		return false;

	const auto Status = hThread ? NtSIT(hThread, 0x11, 0, 0) : NtSIT(GetCurrentThread(), 0x11, 0, 0);

	return Status == 0;
}