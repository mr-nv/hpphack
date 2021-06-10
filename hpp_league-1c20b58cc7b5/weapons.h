enum Penerations
{
	WALL_PEN0,
	WALL_PEN1,
	WALL_PEN2,
};

class CBaseWeaponData
{
public:
	CBaseWeaponData() { RtlSecureZeroMemory(this, sizeof(*this)); }

	bool				m_bDelayFire;

	unsigned int		m_iRandomSeed;

	int					m_iWeaponState;
	int					m_iFlags;
	int					m_iClip;
	int					m_iWeaponID;
	int					m_iInReload;
	int					m_iPenetration;
	int					m_iBulletType;
	int					m_iDamage1;
	int					m_iDamage2;
	int					m_iUser3;

	float				m_flAccuracy;
	float				m_flSpread;
	float				m_flNextPrimaryAttack;
	float				m_flNextSecondaryAttack;
	float				m_flNextAttack;
	float				m_flPenetrationDistance;
	float				m_flWallPierce1;
	float				m_flWallPierce2;
};

class CBasePlayerWeapon
{
private:
	void				_UpdateAccuracy();
	void				_UpdateWeaponData();

public:
	CBaseWeaponData		data;

	void				ItemPreFrame(local_state_s* to, usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed);
	void				Update();

	bool				CanAttack(void);
	bool				IsGun(void);
	bool				IsKnife(void);
	bool				IsNonAttack(void);
	bool				IsNade(void);
	bool				IsC4(void);
	bool				IsSniper(void);
	bool				IsPistol(void);
	bool				IsRifle(void);
	bool				IsShotGun(void);
	bool				IsMachineGun(void);
	bool				IsSubMachineGun(void);
	bool				IsSilenced(void);
	bool				IsInBurst(void);
	bool				IsLeftElite(void);
	bool				IsFreezePeriod(void);

	int					GetClassType(void);
	int					GetPenetration(void);
	int					GetBulletType(void);
	int					GetDamage(void);

	float				GetWallPierce(void);
	float				GetDistance(void);
};
extern CBasePlayerWeapon g_Weapon;