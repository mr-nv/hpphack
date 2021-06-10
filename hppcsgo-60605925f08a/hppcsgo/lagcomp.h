#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / g_pGlobalVars->interval_per_tick ) )
#define TICKS_TO_TIME( dt )		( g_pGlobalVars->interval_per_tick * (float)(dt) )

#define LAG_COMPENSATION_TELEPORTED_DISTANCE_SQR ( 64.0f * 64.0f )

// Only keep 1 second of data
#define LAG_COMPENSATION_DATA_TIME	1.0f

class CLagCompensation;
class CLagCompensation {
public:
	void		Store();
	void		Restore(C_BasePlayer *player);
	void		SetRecord(C_BasePlayer *player, CHistory record);
	void		ProcessCmd(CUserCmd* cmd, CHistory record);
	void		SetInterpolationFlags(C_BasePlayer* player, int flag);

	bool		IsValidRecord(CHistory record);

	float		GetLerpTime();
};
extern CLagCompensation g_LagComp;
