class CBaseLocalPlayer
{
public:
	int			m_iIndex;
	int			m_iTeam;
	int			m_iFOV;
	int			m_iMessageNum;

	bool		m_bIsDead;

	float		m_flVelocity;

	Vector		m_vecEye;
};

class CBasePlayer
{
public:
	int			m_iIndex;
	int			m_iTeam;
	int			m_iHealth;

	bool		m_bIsInPVS;
	bool		m_bIsDead;

	Vector		m_vecOrigin;
	Vector		m_vecBoundBoxMaxs;
	Vector		m_vecBoundBoxMins;
	Vector		m_vecHitbox[21];
	Vector		m_vecHitboxMin[21];
	Vector		m_vecHitboxMax[21];
	Vector		m_vecHitboxPoints[21][8];
};

class CWorld
{
public:
	CBasePlayer* GetPlayer(const int index = 0);

	void		Update();
	void		Reset();
	void		Clear();

private:
	CBasePlayer m_Player[MAX_CLIENTS - 1];

	void		UpdateLocalPlayer();
	void		UpdatePlayers();
};

extern CBaseLocalPlayer g_Local;
extern CWorld g_World;