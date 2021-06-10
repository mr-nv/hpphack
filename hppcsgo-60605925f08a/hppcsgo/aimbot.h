class CAimbot;
class CAimbot {
private:
	bool		_bTriggerActive;
	bool		_bAimbotActive;

	void		_Trigger(CUserCmd* cmd);
	void		_RageBot(CUserCmd* cmd);
	void		_LegitBot(CUserCmd* cmd);
	void		_SmoothAimAngles(const QAngle a_QAngles, const QAngle a_QAimAngles, QAngle &a_QNewAngles, const float a_flSmoothing);
	void		_QuickStop(CUserCmd *cmd);

public:
	float		m_flLegitBotFOV;
	float		m_flLastTimeKilled;

	void		Run(CUserCmd* cmd);
};
extern CAimbot g_Aimbot;

class CTarget;
class CTarget {
public:
	Vector			hitbox;
	C_BasePlayer	*player;
	CHistory		history;
	bool			lagcomp;
};

class CRagebotPosition;
class CRagebotPosition {
public:
	Vector			point;
	C_BasePlayer	*player;
	bool			lagcomp;
	CHistory		history;
	float			damage;
};

class CRageHitbox;
class CRageHitbox {
public:
	int				num;
	int				group;
	bool			skip;
};