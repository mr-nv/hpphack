class CGlobalsVars;
class CGlobalsVars {
public:
	CGlobalsVars() { RtlSecureZeroMemory(this, sizeof(*this)); }

	HMODULE			m_hModule;
	DWORD			m_dwProcessID;
	HWND			m_hWnd;
	DWORD			m_LastPacketTime;

	std::string		m_sSettingsPath;
	std::string		m_sDebugFile;
	std::string		m_sOnlineUsers;
	std::string		m_sUUID;

	int				m_iSocketServerID;

	int				m_iScreenWidth;
	int				m_iScreenHeight;

	float			m_flScreenWidth;
	float			m_flScreenHeight;

	bool			m_bUnloadLibrary;
	bool			m_bSendCommand;
	bool			m_bLicenseChecked;

	float			weapon_recoil_scale;
};
extern CGlobalsVars g_Globals;