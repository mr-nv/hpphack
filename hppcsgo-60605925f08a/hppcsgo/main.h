#include <Windows.h>
#include <string>
#include <ctime>
#include <io.h>
#include <d3d9.h>
#include <array>
#include <deque>
#include <mutex>
#include <DirectXMath.h>
#include <wbemidl.h>
#include <comutil.h>
#include <map>

#define LICENSING

#ifdef LICENSING
#include "VMProtectSDK.h"
#define VMP_BEGIN(e) VMProtectBegin(e)
#define VMP_VIRTUAL(e) VMProtectBeginVirtualization(e)
#define VMP_MUTATION(e) VMProtectBeginMutation(e)
#define VMP_ULTRA(e) VMProtectBeginUltra(e)
#define VMP_DecryptA(e) VMProtectDecryptStringA(e)
#define VMP_DecryptW(e) VMProtectDecryptStringW(e)
#define VMP_END VMProtectEnd()
#else
#define VMP_BEGIN(e)
#define VMP_VIRTUAL(e)
#define VMP_MUTATION(e)
#define VMP_ULTRA(e)
#define VMP_DecryptA(e) (e)
#define VMP_DecryptW(e) (e)
#define VMP_END
#endif

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "comsuppw.lib")  

#define RAD2DEG(x)		DirectX::XMConvertToDegrees(x)
#define DEG2RAD(x)		DirectX::XMConvertToRadians(x)
#define M_PI			3.14159265358979323846f

#define VERIFY_PTR(p)	if (p == nullptr) return
#define VERIFY(v)		if (!(v)) return

#include "ValveSDK/netvars.hpp"
#include "ValveSDK.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/impl/imgui_impl_dx9.h"
#include "ImGui/impl/imgui_impl_win32.h"

#include "easywsclient.hpp"

using easywsclient::WebSocket;

#include "vfunc_hook.hpp"
#include "client.h"
#include "offsets.h"
#include "struct.h"
#include "utils.h"
#include "cvars.h"
#include "security.h"
#include "globals.h"
#include "world.h"
#include "settings.h"
#include "aimbot.h"
#include "gameevents.h"
#include "autowall.h"
#include "lagcomp.h"
#include "prediction.h"
#include "miscellaneous.h"