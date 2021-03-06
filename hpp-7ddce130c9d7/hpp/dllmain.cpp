#include "main.h"

CGlobalsVars g_Globals;
WNDPROC g_pWndProc = nullptr;

struct HandleData
{
	ULONG processId;
	HWND hWnd;
};

static BOOL IsBestWindow(HWND hWnd)
{
	return GetWindow(hWnd, GW_OWNER) == NULL && IsWindowVisible(hWnd);
}

static BOOL CALLBACK EnumWindowsCallback(HWND hWnd, LPARAM lParam)
{
	HandleData& data = *(HandleData*)lParam;
	ULONG processId;

	GetWindowThreadProcessId(hWnd, &processId);

	if (data.processId != processId || !IsBestWindow(hWnd))
		return TRUE;

	data.hWnd = hWnd;
	return FALSE;
}

static HWND FindWindowById(ULONG processId)
{
	HandleData data;
	data.processId = processId;
	data.hWnd = NULL;

	EnumWindows(EnumWindowsCallback, (LPARAM)&data);

	if (data.hWnd == NULL)
		TraceLog("%s: handle window by process id %lu not found.\n", __FUNCTION__, processId);

	return data.hWnd;
}

static BOOL GetSettingsPath()
{
	char* _Buffer = nullptr;
	size_t _BufferCount = 0;

	if (_dupenv_s(&_Buffer, &_BufferCount, "appdata") == 0 && _Buffer != nullptr)
	{
		g_Globals.m_sSettingsPath = _Buffer;
		g_Globals.m_sSettingsPath.append("\\Hpp Hack Reload\\");

		CreateDirectory(g_Globals.m_sSettingsPath.c_str(), 0);

		free(_Buffer);

		return TRUE;
	}

	TraceLog("> %s: failed to get settings path.\n", __FUNCTION__);

	return FALSE;
}

static void SetHighPriorityById(DWORD dwProcessId)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS);
}

DWORD WINAPI TimerLicense(LPVOID /*lpThreadParameter*/)
{
	VMP_MUTATION(__FUNCTION__);

	static auto prev_clock_time = clock();

	while (true)
	{
		if (g_Globals.m_bUnloadLibrary)
			return FALSE;

		if ((((double)clock() - prev_clock_time) / (double)CLOCKS_PER_SEC) > 300.0)
		{
			if (client_static && client_static->state != ca_active)
			{
				SendHardwareID();

				prev_clock_time = clock();
			}
		}

		Sleep(1000);
	}

	VMP_END;
	return FALSE;
}

DWORD WINAPI SetupHooks(LPVOID /*lpThreadParameter*/)
{
	while (true)
	{
		if (g_Offsets.FindModules())
		{
			if (g_Offsets.FindInterfaces())
				break;
		}

		Sleep(100);
	}

	VMP_BEGIN(__FUNCTION__);

#ifdef LICENSING
	if (!IsDLLfromLoader())
		return FALSE;
#endif

	if (CreateSocket())
	{
		SendHardwareID();

		auto prev_clock_time = clock();

		while ((((double)clock() - prev_clock_time) / (double)CLOCKS_PER_SEC) < 15.0 && !g_Globals.m_bLicenseChecked)
		{
			Sleep(100);

			if (!UpdateSocket())
			{
				TraceLog(VMP_DecryptA("websocket: server did not respond.\n"));
				break;
			}
		}

		if (!g_Globals.m_bLicenseChecked)
			return FALSE;

		auto hTimerLicenseThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)TimerLicense, NULL, NULL, NULL);

		if (!hTimerLicenseThread || !HideThread(hTimerLicenseThread))
			return FALSE;

		if (!InstallHooks())
		{
			DeleteHooks();
			return FALSE;
		}
	}

	VMP_END;
	auto prev_clock_time = clock();

	while (true)
	{
		Sleep(10);

		if (g_Globals.m_bUnloadLibrary || !g_Globals.m_bLicenseChecked)
		{
			DeleteHooks();
			return FALSE;
		}

		if (websocket && websocket->getReadyState() == WebSocket::OPEN)
		{
			UpdateSocket();

			if ((((double)clock() - prev_clock_time) / (double)CLOCKS_PER_SEC) > 5.0)
			{
				websocket->send(VMP_DecryptA("addme"));
				prev_clock_time = clock();
			}

			if ((((double)clock() - g_Globals.m_LastPacketTime) / (double)CLOCKS_PER_SEC) > 30.0)
			{
				TraceLog(VMP_DecryptA("websocket: timeout.\n"));
				g_Globals.m_bUnloadLibrary = true;
				g_Globals.m_bLicenseChecked = false;
				CloseSocket();
			}
		}
		else
		{
			auto nRetries = 0;

			while (nRetries < 3)
			{
				Sleep(15000);//Reconnect interval

				Reconnect();

				prev_clock_time = clock();

				while ((((double)clock() - prev_clock_time) / (double)CLOCKS_PER_SEC) < 15.0 && !g_Globals.m_bLicenseChecked && websocket)//Timeout 15sec for answer
				{
					Sleep(100);
					UpdateSocket();
				}

				if (g_Globals.m_bLicenseChecked && websocket && websocket->getReadyState() == WebSocket::OPEN)//Fully reconnected
					break;

				nRetries++;
			}

			if (nRetries >= 3)
			{
				DeleteHooks();
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			FreeLibraryAndExitThread(hinstDLL, NULL);
			return FALSE;
		}

		DisableThreadLibraryCalls(hinstDLL);

		g_Globals.m_hModule = hinstDLL;
		g_Globals.m_dwProcessID = GetCurrentProcessId();
		g_Globals.m_hWnd = FindWindowById(g_Globals.m_dwProcessID);
		
		if (!g_Globals.m_hModule || !g_Globals.m_dwProcessID || !g_Globals.m_hWnd || !GetSettingsPath())
		{
			FreeLibraryAndExitThread(hinstDLL, NULL);
			return FALSE;
		}
		
		g_Globals.m_sDebugFile = g_Globals.m_sSettingsPath + VMP_DecryptA("debug.log");

		SetHighPriorityById(g_Globals.m_dwProcessID);

		auto hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)SetupHooks, NULL, NULL, NULL);
#ifdef LICENSING
		if (!hThread || !HideThread(hThread))
		{
			FreeLibraryAndExitThread(hinstDLL, NULL);
			return FALSE;
		}

		CloseHandle(hinstDLL);
#endif
	}

	return TRUE;
}