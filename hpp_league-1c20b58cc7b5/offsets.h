class COffsets;
class COffsets {
public:
	Module			hw, client, gameui, vgui2, gameoverlayrenderer, steamclient;

	DWORD			m_dwInitPoint;

	bool			FindModules(void);
	bool			FindInterfaces(void);
	bool			FindOffsets(void);
	bool			FindModuleByName(const char* name, Module* module);
	bool			AddressNotInSpace(uintptr_t ptr, uintptr_t low, uintptr_t high);

	PVOID			Client(void);
	PVOID			Engine(void);
	PVOID			Studio(void);
	PVOID			PlayerMove(void);
	PVOID			ClientState(void);
	PVOID			ClientStatic(void);
	PVOID			StudioRenderModel(void);

	uintptr_t		FindInitPoint(void);
	uintptr_t		ExtraPlayerInfo(void);
	uintptr_t		FindReference(uintptr_t start, uintptr_t end, uintptr_t ptr);
	uintptr_t		FindDownPattern(PCHAR pattern, uintptr_t start, uintptr_t end, uintptr_t offset = 0);
	uintptr_t		FindDownPattern(PCHAR pattern, PCHAR mask, uintptr_t start, uintptr_t end, uintptr_t offset = 0);
	uintptr_t		FindUpPattern(PCHAR pattern, uintptr_t start, uintptr_t end, uintptr_t offset = 0);
	uintptr_t		FindUpPattern(PCHAR pattern, PCHAR mask, uintptr_t start, uintptr_t end, uintptr_t offset = 0);
	uintptr_t		FindPush(PCHAR message, uintptr_t start, uintptr_t end, uintptr_t offset = 0);
};
extern COffsets g_Offsets;
