// hpp.cpp: Defines the entry point for the applicaton.

#include "framework.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
			return FALSE;

		// ...

#if defined(LICENSING)
		CloseHandle(hinstDLL);
#endif
		return TRUE;
	}

	return FALSE;
}