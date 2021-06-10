// rename_me.h: include file for standart system include files or include files for specific project.

#pragma once

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN // Exclude rarely used components from Windows headers.
// Windows Header Files.
#include <Windows.h>
#include <TlHelp32.h>
// Runtime C Header Files.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string>
#include <algorithm>
#include <sstream>
#include <vector>
#include <deque>
#include <ctime>
#include <thread>
#include <functional>
#include <wininet.h>
#include <wbemidl.h>
#include <comutil.h>
#include <tchar.h>
#include <io.h>
#include <shellapi.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
// D3D Header Files.
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "ws2_32")

//#define LICENSING
//#define CHECK_FOR_UPDATES

#define BUILD_VERSION	1
#define CHEATS_COUNT	2

#ifdef LICENSING
#include "vmprotectsdk.h"
#endif

#ifdef LICENSING
#define VM	VMProtectBeginMutation
#define VV	VMProtectBeginVirtualization
#define VU	VMProtectBeginUltra
#define VE	VMProtectEnd
#define V	VMProtectDecryptStringA
#define W_V VMProtectDecryptStringW
#else
#define VM(e)
#define VV(e)
#define VU(e)
#define VE() 
#define V(e) e
#define W_V(e) e
#endif

#define DELETE_PTR(p) delete p; p = nullptr
#define DELETE_ARR_PTR(p) delete[] p; p = nullptr
#define DELETE_UNIQUE_PTR(p) if (p.get()) p.reset()

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#define IMGUI_INCLUDE_IMCONFIG_H

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imconfig.h"

#include "utils.h"
#include "page_list.h"
#include "drawing.h"
#include "license.h"
#include "guard.h"
#include "enums.h"
#include "easywsclient/easywsclient.hpp"

using easywsclient::WebSocket;

constexpr auto WINDOW_WIDTH = 700.f;
constexpr auto WINDOW_HEIGHT = 350.f;

struct Direct3DImageData
{
	UINT Width;
	UINT Height;
	IDirect3DTexture9* pTexture;
};

using ImageList = std::vector<Direct3DImageData>;
using CheatList = std::vector<CheatInfo>;

extern ImageList g_ImageList;
extern CheatList g_CheatList;
extern LicenseInfo g_LicenseInfo;

struct InjectData
{
	InjectState		state;		// state of injecting
	int				buildnum;	// build number from build list
	float			prevtime;	// last time of changed injection state
	std::string		response;	// response from the server
	DWORD			processId;	// identify of process
	bool			injecting;	// inject in processing
	bool			nocache;	// do not create cache file
	bool			close;		// close loader after inject
};

struct GlobalVars
{
	float			curtime;	// current time
	InjectData		injectData;	// inject data
	DWORD			dwDownloadedBytes;
	bool			bShutdown;	// set true to shutdown loader
};

extern GlobalVars g_GlobalVars;

#include "blackbone/BlackBone/Config.h"
#include "blackbone/BlackBone/Process/Process.h"
#include "blackbone/BlackBone/PE/PEImage.h"
#include "blackbone/BlackBone/Misc/Utils.h"
#include "blackbone/BlackBone/Patterns/PatternSearch.h"