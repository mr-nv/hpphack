class COffsets;
class COffsets {
public:
	Module			client_panorama, engine, shaderapidx9, vstdlib, vguimatsurface, vgui2, materialsystem, datacache, vphysics, inputsystem, steamclient;

	DWORD			m_dwCloudOffsets[2];

	bool			FindModules(void);
	bool			FindInterfaces(void);
	bool			FindModuleByName(const char* name, Module* module);
	bool			FindSteamInterfaces(CreateInterfaceFn steamclient);

	uintptr_t		FindDownPattern(PCHAR pattern, uintptr_t start, uintptr_t end, uintptr_t offset = 0);
	uintptr_t		FindDownPattern(PCHAR pattern, PCHAR mask, uintptr_t start, uintptr_t end, uintptr_t offset = 0);
	uintptr_t		FindUpPattern(PCHAR pattern, uintptr_t start, uintptr_t end, uintptr_t offset = 0);
	uintptr_t		FindUpPattern(PCHAR pattern, PCHAR mask, uintptr_t start, uintptr_t end, uintptr_t offset = 0);

	int				*predictionRandomSeed;
	C_BasePlayer	*predictionPlayer;
};
extern COffsets g_Offsets;
