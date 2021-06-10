#include <Windows.h>
#include <math.h>
#include <string>
#include <tlhelp32.h>
#include <deque>
#include <gl/GL.h>
#include <intrin.h>

#pragma comment(lib, "OpenGL32.lib")

#define IM_PI           3.14159265358979323846f
#define IM_ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR)/sizeof(*_ARR)))
#define RAD2DEG(x) ((float)(x) * (float)(180.f / IM_PI))
#define DEG2RAD(x) ((float)(x) * (float)(IM_PI / 180.f))

#include "vector.h"
#include "qangle.h"

#include "sdk.h"
#include "client.h"
#include "offsets.h"
#include "world.h"
#include "utils.h"
#include "aimbot.h"
#include "weapons.h"
#include "drawing.h"

typedef NTSTATUS(NTAPI* pNtSetInformationThread)(HANDLE, UINT, PVOID, ULONG);

void Message(const char* fmt, ...);
bool HideThread(HANDLE hThread);