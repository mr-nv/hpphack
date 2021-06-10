// rename_me.cpp: Defines the entry point for the applicaton.

#include "framework.h"
#include "image_logo.h"

#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

static HINSTANCE g_hInstance;
static LPCSTR g_lpszClassName;
static HWND g_hWnd;
static IDirect3D9* g_pD3D9;
static IDirect3DDevice9* g_pD3DDevice9;
static D3DPRESENT_PARAMETERS g_d3dpp;

ImageList g_ImageList;
CheatList g_CheatList;
LicenseInfo g_LicenseInfo;
GlobalVars g_GlobalVars;

// Send announcements of functions included in this code module:
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

bool RegisterWindowClass()
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_CLASSDC;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = NULL;
	wcex.cbWndExtra = NULL;
	wcex.hInstance = g_hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = V("Direct3DWindow");
	wcex.hIconSm = NULL;

	g_lpszClassName = wcex.lpszClassName;

	return RegisterClassEx(&wcex);
}

bool CreateWindowHandle()
{
	const auto screen_width = GetSystemMetrics(SM_CXSCREEN);
	const auto screen_height = GetSystemMetrics(SM_CYSCREEN);

	const auto window_width = static_cast<int>(WINDOW_WIDTH);
	const auto window_height = static_cast<int>(WINDOW_HEIGHT);

	const auto window_pos_x = (screen_width - window_width) / 2;
	const auto window_pos_y = (screen_height - window_height) / 2;

	g_hWnd = CreateWindow(g_lpszClassName, V("Hpp Loader"), WS_POPUP, window_pos_x,
		window_pos_y, window_width, window_height, NULL, NULL, g_hInstance, NULL);

	if (!g_hWnd)
		return false;

	ShowWindow(g_hWnd, SW_SHOWDEFAULT);
	return UpdateWindow(g_hWnd);
}

HRESULT CreateDirect3DTexture(UINT Width, UINT Height)
{
	Direct3DImageData image;

	image.Width = Width;
	image.Height = Height;

	const auto ret = D3DXCreateTextureFromFileInMemoryEx(g_pD3DDevice9, images::logo,
		sizeof(images::logo), Width, Height, D3DX_DEFAULT, NULL, D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, NULL, NULL, NULL, &image.pTexture);

	g_ImageList.push_back(image);

	return ret;
}

bool InitializeDirect3D()
{
	g_pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);

	if (!g_pD3D9)
		return false;

	// Initialize direct3d present parms.
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (g_pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pD3DDevice9))
		return false;

	if (CreateDirect3DTexture(17, 20))
		return false;

	if (CreateDirect3DTexture(72, 88))
		return false;

	if (CreateDirect3DTexture(86, 109))
		return false;

	return true;
}

bool InitializeDearImGui()
{
	if (!ImGui::CreateContext())
		return false;

	ImGui_ImplDX9_Init(g_pD3DDevice9);
	ImGui_ImplWin32_Init(g_hWnd);

	return true;
}

bool Initialize()
{
	if (!RegisterWindowClass())
		return false;

	if (!CreateWindowHandle())
		return false;

	if (!InitializeDirect3D())
		return false;

	if (!InitializeDearImGui())
		return false;

	return true;
}

void ShutdownDearImGui()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();
}

#define RELEASE_D3D(p) if (p) { p->Release(); p = nullptr; }

void ReleaseDirect3D()
{
	RELEASE_D3D(g_pD3DDevice9);
	RELEASE_D3D(g_pD3D9);
}

void Shutdown()
{
	ShutdownDearImGui();
	ReleaseDirect3D();
}

void DeleteTemporaryFile()
{
	auto sCurrentExeFilePath = Utils::GetExeDirectory() + ".tmp";

	if (Utils::FileExists(sCurrentExeFilePath))
		DeleteFile(sCurrentExeFilePath.c_str());
}

bool CheckForUpdates()
{
#if defined(CHECK_FOR_UPDATES)
	const auto url = V("/hpp/sockets/loader.php?version=") + std::to_string(BUILD_VERSION);
	const auto response = GetUrlData(url);

	if (response.empty())
		return false;

	const auto version = std::stoi(response);

	if (version > BUILD_VERSION)
		return true;
#endif
	return false;
}

void UpdateLoaderBuild()
{
	const auto sCurrentExeFileName = Utils::GetExeDirectory();
	const auto sTemporaryExeFileName = sCurrentExeFileName + ".tmp";

	MoveFile(sCurrentExeFileName.c_str(), sTemporaryExeFileName.c_str());

	FILE* pFile = fopen(sCurrentExeFileName.c_str(), "wb+");

	if (pFile)
	{
		auto response = GetUrlData(V("/hpp/sockets/loader.php?download=yes"));

		if (!response.empty())
		{
			response = HexToString(response);
			fwrite(response.c_str(), sizeof(char), response.size(), pFile);
			response.clear();
		}

		fclose(pFile);
	}

	if (Utils::FileExists(sCurrentExeFileName))
		ShellExecute(NULL, NULL, sCurrentExeFileName.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void LicenseThread()
{
	auto pLicense = std::make_unique<CLicense>();

	g_LicenseInfo.bIsAviableUpdate = CheckForUpdates();

	if (g_LicenseInfo.bIsAviableUpdate)
	{
		UpdateLoaderBuild();

		g_GlobalVars.bShutdown = true;
	}
	else
	{
		g_LicenseInfo.sHardwareId = pLicense->GetHardwareId();

		if (g_LicenseInfo.sHardwareId.empty())
		{
			g_GlobalVars.bShutdown = true;
		}
		else
		{
			std::string response;

			// Try to get an answer (if the Internet connection is lost).
			while ((response = pLicense->GetBuildsResponse(g_LicenseInfo.sHardwareId)).empty())
				std::this_thread::sleep_for(std::chrono::milliseconds::duration(100));

			auto buffer = response;

			for (int i = 0; i < CHEATS_COUNT; i++)
			{
				if ((buffer = FindArgument(response, i + 1, "^")).length() < 2)
					break;

				if ((buffer = FindArgument(buffer, 1)).length() < 2)
					break;

				if (!buffer.compare(V("hpp_cs16")))
				{
					CheatInfo cheat;
					cheat.name = "Hpp Hack: Reload";
					cheat.version = "5.3.5 Stable";
					cheat.game = "Counter-Strike 1.6";
					cheat.url = V("&product=1");
					cheat.folder = "Hpp Hack Reload";
					cheat.process = "hl.exe";
					cheat.description = "Support:^- Valve Anti-Cheat^- All Server-Side Anti-Cheats^";
					cheat.changelog =
						"Change log of [21.05.2019]:\n"
						"\t- Fixed func1, func2, func3\n"
						"\t- Added func1, func2\n"
						"\t- Removed func1";
					g_CheatList.push_back(cheat);
				}

				if (!buffer.compare(V("hpp_csgo")))
				{
					CheatInfo cheat;
					cheat.name = "Hpp Hack: CS:GO";
					cheat.version = "1.0.0 Beta";
					cheat.game = "Counter-Strike: Global Offensive";
					cheat.url = V("&product=2");
					cheat.folder = "Hpp Hack CSGO";
					cheat.process = "csgo.exe";
					cheat.description = "Support:^- Valve Anti-Cheat^";
					cheat.changelog =
						"Change log of [21.05.2019]:\n"
						"\t- Fixed func1, func2, func3\n"
						"\t- Added func1, func2\n"
						"\t- Removed func1";
					g_CheatList.push_back(cheat);
				}
			}

			g_LicenseInfo.bIsHaveSubscription = g_CheatList.size() > 0;
			g_LicenseInfo.bIsUpdatedCheatList = true;
		}
	}
}

void GuardThread()
{
	auto pGuard = std::make_unique<CGuard>();

	while (!g_GlobalVars.bShutdown)
	{
		pGuard->Run();
		std::this_thread::sleep_for(std::chrono::milliseconds::duration(100));
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	g_hInstance = hInstance;

	DeleteTemporaryFile();

	if (Initialize())
	{
		std::thread license_thread(LicenseThread);
		std::thread guard_thread(GuardThread);
		license_thread.detach();
		guard_thread.detach();

		auto pDrawing = std::make_unique<CDrawing>();

		MSG msg;
		memset(&msg, 0, sizeof(MSG));
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				continue;
			}

			/*__int64 time;
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&time));
			g_GlobalVars.curtime = static_cast<float>(time) / _XTIME_TICKS_PER_TIME_T*/

			g_GlobalVars.curtime = static_cast<float>(GetTickCount()) / 1000.f;

			ImGui_ImplDX9_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			pDrawing->Run();

			auto draw_settings = pDrawing->GetDrawSettings();

			D3DCOLOR d3dcolor;
			switch (draw_settings.themenum)
			{
			case WhiteTheme:
				d3dcolor = D3DCOLOR_RGBA(255, 255, 255, 255);
				break;
			case DarkTheme:
				d3dcolor = D3DCOLOR_RGBA(18, 18, 18, 255);
			}

			g_pD3DDevice9->SetRenderState(D3DRS_ZENABLE, 0);
			g_pD3DDevice9->SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
			g_pD3DDevice9->SetRenderState(D3DRS_SCISSORTESTENABLE, 0);
			g_pD3DDevice9->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, d3dcolor, 1, 0);

			if (g_pD3DDevice9->BeginScene() == D3D_OK)
			{
				ImGui::Render();
				g_pD3DDevice9->EndScene();
			}

			if (g_pD3DDevice9->Present(nullptr, nullptr, nullptr, nullptr) == D3DERR_DEVICELOST &&
				g_pD3DDevice9->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
			{
				ImGui_ImplDX9_InvalidateDeviceObjects();
				g_pD3DDevice9->Reset(&g_d3dpp);
				ImGui_ImplDX9_CreateDeviceObjects();
			}

			if (g_GlobalVars.bShutdown) break;
		}

		DELETE_UNIQUE_PTR(pDrawing);		
	}

	Shutdown();

	std::this_thread::sleep_for(std::chrono::milliseconds::duration(100));

	return 0;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return 0;

	switch (message)
	{
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void DownloadThread()
{
	g_GlobalVars.injectData.response.clear();

	std::string url;
	url.append(V("/hpp/sockets/server.php?req=download&hwid="));
	url.append(g_LicenseInfo.sHardwareId);
	url.append(g_CheatList[g_GlobalVars.injectData.buildnum].url);

	auto flags = INTERNET_FLAG_SECURE;

	if (g_GlobalVars.injectData.nocache)
		flags |= INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_NO_CACHE_WRITE;

	g_GlobalVars.injectData.response = GetUrlData(url, flags);
	g_GlobalVars.injectData.state = InjectState::LookingProcess;
	g_GlobalVars.injectData.prevtime = g_GlobalVars.curtime;
}

void LookingProcessThread()
{	
	g_GlobalVars.injectData.processId = FindProcessId(g_CheatList[g_GlobalVars.injectData.buildnum].process);

	if (!g_GlobalVars.injectData.processId && !g_CheatList[g_GlobalVars.injectData.buildnum].game.compare("Counter-Strike 1.6"))
		g_GlobalVars.injectData.processId = FindProcessId("cs.exe");
}

void InjectingThread()
{
	static const char uszPatch[] = { 0x37,0x37,0x37,0x37,0x37,0x37,0x37,0x37,0x37,0x37 };
	static blackbone::PatternSearch ps{ "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" };

	if (!g_GlobalVars.injectData.response.empty())
	{
		g_GlobalVars.injectData.response = HexToString(g_GlobalVars.injectData.response);

		blackbone::Process process;
		process.Attach(g_GlobalVars.injectData.processId);

		if (!g_CheatList[g_GlobalVars.injectData.buildnum].game.compare("Counter-Strike 1.6"))
		{
			auto blacbone_module = process.modules().GetModule(W_V(L"hw.dll"));

			if (!blacbone_module)
				blacbone_module = process.modules().GetModule(W_V(L"engine.dll"));

			if (blacbone_module)
			{
				std::vector<blackbone::ptr_t> results;

				auto& memory = process.memory();
				ps.SearchRemote(process, blacbone_module->baseAddress, blacbone_module->size, results);

				if (!results.empty())
				{
					const auto address = results.front();

					DWORD dwOldProtect = 0;

					if (NT_SUCCESS(memory.Protect(address, sizeof(uszPatch), PAGE_READWRITE, &dwOldProtect)))
					{
						memory.Write(results.front(), "\x37\x37\x37\x37\x37\x37\x37\x37\x37\x37");
						memory.Protect(address, sizeof(uszPatch), dwOldProtect);

						process.mmap().MapImage(g_GlobalVars.injectData.response.size(), (PVOID)g_GlobalVars.injectData.response.c_str(),
							false, blackbone::CreateLdrRef | blackbone::RebaseProcess | blackbone::NoDelayLoad | blackbone::NoTLS);
					}
				}
			}
		}
		else 
		{
			process.mmap().MapImage(g_GlobalVars.injectData.response.size(), (PVOID)g_GlobalVars.injectData.response.c_str(),
				false, blackbone::CreateLdrRef | blackbone::RebaseProcess | blackbone::NoDelayLoad | blackbone::NoTLS);
		}
	}

	g_GlobalVars.injectData.buildnum = 0;
	g_GlobalVars.injectData.prevtime = 0.f;
	g_GlobalVars.injectData.processId = 0;
	g_GlobalVars.injectData.injecting = false;
	g_GlobalVars.injectData.response.clear();
	g_GlobalVars.injectData.state = InjectState::None;

	if (g_GlobalVars.injectData.close)
		g_GlobalVars.bShutdown = true;
}