#include "framework.h"

CGuard::CGuard()
{
}

CGuard::~CGuard()
{
}

void CGuard::Run()
{
	if (CheckProcessHacker())
		g_GlobalVars.bShutdown = true;

	if (CheckCheatEngine())
		g_GlobalVars.bShutdown = true;

	if (CheckIDAPro())
		g_GlobalVars.bShutdown = true;

	if (CheckOllyDbg())
		g_GlobalVars.bShutdown = true;

	if (CheckWireshark())
		g_GlobalVars.bShutdown = true;

	if (CheckHttpAnalyzer())
		g_GlobalVars.bShutdown = true;

	if (Checkx96dbg())
		g_GlobalVars.bShutdown = true;

	if (CheckProcessExplorer())
		g_GlobalVars.bShutdown = true;
}

bool CGuard::CheckProcessHacker()
{
	if (FindWindow(V("ProcessHacker"), NULL))
		return true;

	return false;
}

bool CGuard::CheckCheatEngine()
{
	HWND hWnd = FindWindow(V("Window"), NULL);

	if (!hWnd)
		return false;

	char szWindowName[256];
	GetWindowText(hWnd, szWindowName, sizeof(szWindowName));

	if (strstr(szWindowName, V("Cheat Engine")))
		return true;

	return false;
}

bool CGuard::CheckIDAPro()
{
	HWND hWnd = FindWindow(V("Qt5QWindowIcon"), NULL);

	if (!hWnd)
		return false;

	char szWindowName[256];
	GetWindowText(hWnd, szWindowName, sizeof(szWindowName));

	if (szWindowName[0] == 'I' && szWindowName[1] == 'D' && szWindowName[2] == 'A')
		return true;

	return false;
}

bool CGuard::CheckOllyDbg()
{
	if (FindWindow(V("OllyDbg"), NULL))
		return true;

	return false;
}

bool CGuard::CheckWireshark()
{
	HWND hWnd = FindWindow(V("Qt5QWindowIcon"), NULL);

	if (!hWnd)
		return false;

	char szWindowName[256];
	GetWindowText(hWnd, szWindowName, sizeof(szWindowName));

	if (strstr(szWindowName, V("Wireshark")))
		return true;

	return false;
}

bool CGuard::CheckHttpAnalyzer()
{
	if (FindWindow(V("TStdHttpAnalyzerForm"), NULL))
		return true;

	return false;
}

bool CGuard::Checkx96dbg()
{
	HWND hWnd = FindWindow(V("Qt5QWindowIcon"), NULL);

	if (!hWnd)
		return false;

	char szWindowName[256];
	GetWindowText(hWnd, szWindowName, sizeof(szWindowName));

	if (szWindowName[0] == 'x' && szWindowName[3] == 'd' && szWindowName[4] == 'b' && szWindowName[5] == 'g')
		return true;

	return false;
}

bool CGuard::CheckProcessExplorer()
{
	if (FindWindow(V("PROCEXPL"), NULL))
		return true;

	return false;
}