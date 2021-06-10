
class CUtils;
class CUtils
{
private:
	DWORD			_dwOldPageProtection;
	Vector			_vecPreviousAngles;

public:
	bool			IsBoxIntersectingRay(const Vector& vecBoundBoxMin, const Vector& vecBoundBoxMax, const Vector& vecOrigin, const Vector& vecDelta);
	bool			CalcScreen(Vector a_vecOrigin, float* a_pflVecScreen);

	int				LookupSequence(model_s* a_pModel, const char** a_pcszLabel, const int a_nSize);

	void			GetHitboxes(const cl_entity_s* a_pGameEntity);
	void			VectorTransform(Vector in1, float in2[3][4], float* out, int xyz, float multi);
	void			VectorTransform(Vector in1, float in2[3][4], float* out);

	void			FixMoveStart(usercmd_s* cmd);
	void			FixMoveEnd(usercmd_s* cmd);

	void			VectorAngles(const float* forward, float* angles);
	void			MakeAngle(bool bAddAngles, float* fAngles, usercmd_s* cmd);
	void			NormalizeAngles(float* angles);
};

extern CUtils g_Utils;