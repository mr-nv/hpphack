#include "ValveSDK/Misc/Enums.hpp"
#include "ValveSDK/Misc/vfunc.hpp"

#include "ValveSDK/Math/VMatrix.hpp"
#include "ValveSDK/Math/QAngle.hpp"
#include "ValveSDK/Math/Vector.hpp"
#include "ValveSDK/Misc/Studio.hpp"

#include "ValveSDK/Interfaces/IAppSystem.hpp"
#include "ValveSDK/Interfaces/IBaseClientDll.hpp"
#include "ValveSDK/Interfaces/IClientEntity.hpp"
#include "ValveSDK/Interfaces/IClientEntityList.hpp"
#include "ValveSDK/Interfaces/IClientMode.hpp"
#include "ValveSDK/Interfaces/IClientRenderable.hpp"
#include "ValveSDK/Interfaces/IVEngineClient.hpp"
#include "ValveSDK/Interfaces/IEngineTrace.hpp"
#include "ValveSDK/Interfaces/IConVar.hpp"
#include "ValveSDK/Interfaces/ICvar.hpp"
#include "ValveSDK/Interfaces/IVModelInfoClient.hpp"
#include "ValveSDK/Interfaces/IVModelRender.hpp"
#include "ValveSDK/Interfaces/IRenderView.hpp"
#include "ValveSDK/Interfaces/IVDebugOverlay.hpp"
#include "ValveSDK/Interfaces/ISurface.hpp"
#include "ValveSDK/Interfaces/CInput.hpp"
#include "ValveSDK/Interfaces/IViewRender.hpp"
#include "ValveSDK/Interfaces/CClientState.hpp"
#include "ValveSDK/Interfaces/IPhysics.hpp"
#include "ValveSDK/Interfaces/IInputSystem.hpp"
#include "ValveSDK/Interfaces/IGameEventmanager.hpp"
#include "ValveSDK/Interfaces/IMaterialSystem.hpp"
#include "ValveSDK/Interfaces/IMoveHelper.hpp"
#include "ValveSDK/Interfaces/IPrediction.hpp"
#include "ValveSDK/Interfaces/IPanel.hpp"
#include "ValveSDK/Interfaces/IMDLCache.hpp"
#include "ValveSDK/Interfaces/IEngineSound.hpp"

#include "ValveSDK/Misc/Convar.hpp"
#include "ValveSDK/Misc/CUserCmd.hpp"
#include "ValveSDK/Misc/glow_outline_effect.hpp"

#include "ValveSDK/archtypes.h"

#include "ValveSDK/steam/isteamclient.h"
#include "ValveSDK/steam/isteamremotestorage.h"
#include "ValveSDK/steam/isteamscreenshots.h"
#include "ValveSDK/steam/isteamutils.h"
#include "ValveSDK/steam/isteamfriends.h"
#include "ValveSDK/steam/isteamuser.h"

struct Module {
	HMODULE		handle;
	uintptr_t	base;
	uintptr_t	size;
	uintptr_t	end;
};

#define CREATEINTERFACE_PROCNAME	"CreateInterface"
// All interfaces derive from this.
class IBaseInterface
{
public:
	virtual			~IBaseInterface() {}
};

CreateInterfaceFn CaptureFactory(Module* module);
void *CaptureInterface(CreateInterfaceFn Interface, const char* InterfaceName);

namespace index
{
	constexpr auto EmitSound1 = 5;
	constexpr auto EmitSound2 = 6;
	constexpr auto EndScene = 42;
	constexpr auto Reset = 16;
	constexpr auto PaintTraverse = 41;
	constexpr auto CreateMove = 22;
	constexpr auto PlaySound = 82;
	constexpr auto FrameStageNotify = 37;
	constexpr auto DrawModelExecute = 21;
	constexpr auto DoPostScreenSpaceEffects = 44;
	constexpr auto SvCheatsGetBool = 13;
	constexpr auto OverrideView = 18;
	constexpr auto LockCursor = 67;
	constexpr auto IsHltv = 93;
	constexpr auto KeyInput = 21;
	constexpr auto DispatchUserMessage = 38;
}