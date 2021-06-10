class CAimbot;
class CAimbot {
private:
	bool		_bTriggerActive;
	bool		_bAimbotActive;
	QAngle		_RCSLastPunch;

	void		_Trigger(usercmd_s* cmd);
	void		_LegitBot(usercmd_s* cmd);
	void		_SmoothAimAngles(const QAngle a_QAngles, const QAngle a_QAimAngles, QAngle& a_QNewAngles, const float a_flSmoothing);

public:
	void		Run(usercmd_s* cmd);
};
extern CAimbot g_Aimbot;

class CTarget;
class CTarget {
public:
	int		m_iHitbox;
	CBasePlayer* m_pPlayer;

	CTarget();
};