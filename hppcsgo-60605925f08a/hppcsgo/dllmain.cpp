#include "main.h"
#include "notifications.h"

WNDPROC g_pWndProc;
CGlobalsVars g_Globals;

LRESULT CALLBACK WndProc_Hooked(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static HWND GetCurrentWindowHandle()
{
	D3DDEVICE_CREATION_PARAMETERS params;
	g_pID3DDevice9->GetCreationParameters(&params);
	return params.hFocusWindow;
}

static BOOL GetSettingsPath()
{
	char* _Buffer = nullptr;
	size_t _BufferCount = 0;

	if (_dupenv_s(&_Buffer, &_BufferCount, "appdata") == 0 && _Buffer != nullptr)
	{
		g_Globals.m_sSettingsPath = _Buffer;
		g_Globals.m_sSettingsPath.append("\\Hpp Hack CSGO\\");

		CreateDirectory(g_Globals.m_sSettingsPath.c_str(), 0);

		free(_Buffer);

		return TRUE;
	}

	TraceLog("> %s: failed to get settings path.\n", __FUNCTION__);

	return FALSE;
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
			if (g_pIEngine && !g_pIEngine->IsConnected())
			{
				SendHardwareID();

				prev_clock_time = clock();
			}
		}

		Sleep(1000);
	}

	return FALSE;
}

static DWORD WINAPI SetupHooks(LPVOID /*lpThreadParameter*/)
{
	while (true)
	{
		if (g_Offsets.FindModules())
			break;

		Sleep(100);
	}

	VMP_ULTRA(__FUNCTION__);

	if (CreateSocket())
	{
		SendHardwareID();

		auto prev_clock_time = clock();

		while (((clock() - prev_clock_time) / (double)CLOCKS_PER_SEC) < 15.0 && !g_Globals.m_bLicenseChecked)
		{
			Sleep(100);

			if (!UpdateSocket())
			{
				g_Notification.Push("WebSocket", "Server did not respond.");
				TraceLog("websocket: server did not respond.\n");
				break;
			}
		}

		if (!g_Globals.m_bLicenseChecked)
		{
			DeleteHooks();
			return FALSE;
		}

		const auto hTimerLicenseThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)TimerLicense, 0, 0, 0);

		if (!hTimerLicenseThread || !HideThread(hTimerLicenseThread))
			return FALSE;

		g_Globals.m_hWnd = GetCurrentWindowHandle();

		if (!InstallHooks())
		{
			DeleteHooks();
			return FALSE;
		}

		g_NetvarSys.Initialize();
		//NetvarSys::Get().Dump();
		g_Settings.Load("Default");
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

			if (((clock() - prev_clock_time) / (double)CLOCKS_PER_SEC) > 5.0)
			{
				websocket->send(VMP_DecryptA("addme"));
				prev_clock_time = clock();
			}

			if (((clock() - g_Globals.m_LastPacketTime) / (double)CLOCKS_PER_SEC) > 30.0)
			{
				g_Notification.Push("WebSocket", "Timeout.");
				TraceLog("websocket: timeout.\n");
				g_Globals.m_bUnloadLibrary = true;
				g_Globals.m_bLicenseChecked = false;
				CloseSocket();
			}
		}
		else
		{
			auto retries = 0;

			while (retries < 3)
			{
				Sleep(15000);//Reconnect interval

				Reconnect();
				g_Notification.Push("WebSocket", "Reconnecting...");

				prev_clock_time = clock();

				while (((clock() - prev_clock_time) / (double)CLOCKS_PER_SEC) < 15.0 && !g_Globals.m_bLicenseChecked && websocket)//Timeout 15sec for answer
				{
					Sleep(100);
					UpdateSocket();
				}

				if (g_Globals.m_bLicenseChecked && websocket && websocket->getReadyState() == WebSocket::OPEN)//Fully reconnected
				{
					g_Notification.Push("WebSocket", "Reconnected.");
					break;
				}

				retries++;
			}

			if (retries >= 3)
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
	VMP_ULTRA(__FUNCTION__);

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

		if (!g_Globals.m_hModule || !g_Globals.m_dwProcessID || !GetSettingsPath())
		{
			FreeLibraryAndExitThread(hinstDLL, NULL);
			return FALSE;
		}

		g_Globals.m_sDebugFile = g_Globals.m_sSettingsPath + VMP_DecryptA("debug.log");

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

	VMP_END;

	return TRUE;
}