class CMiscellaneous;
class CMiscellaneous {
private:
	bool		_bFakeLag;

public:
	int			m_nChoked;

	void		AutoReload(CUserCmd *cmd);
	void		AutoPistol(CUserCmd *cmd);
	void		AutoRevolver(CUserCmd *cmd);
	void		AntiAim(CUserCmd* cmd, bool& bSendPacket);
	void		FakeLag(CUserCmd* cmd, bool& bSendPacket);
};
extern CMiscellaneous g_Misc;