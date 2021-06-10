class CPredictionSystem : public Singleton<CPredictionSystem>
{

public:
	/*CPredictionSystem() {
		predictionRandomSeed = *(int**)(g_Offsets.FindDownPattern((PCHAR)"\x8B\x0D\xFF\xFF\xFF\xFF\xBA\xFF\xFF\xFF\xFF\xE8\xFF\xFF\xFF\xFF\x83\xC4\x04", 
			(PCHAR)"xx????x????x????xxx", g_Offsets.client_panorama.base, g_Offsets.client_panorama.end, 2));
		predictionPlayer = *reinterpret_cast<C_BasePlayer**>(g_Offsets.FindDownPattern((PCHAR)"\x89\x3D\xFF\xFF\xFF\xFF\xF3\x0F\x10\x47", 
			(PCHAR)"xx????xxxx", g_Offsets.client_panorama.base, g_Offsets.client_panorama.end, 2));

	}*/

	void Start(CUserCmd *userCMD, C_BasePlayer* player);
	void End(C_BasePlayer* player);

	inline float GetOldCurTime() { return m_flOldCurTime; }

private:

	float m_flOldCurTime;
	float m_flOldFrametime;

	CMoveData moveData;
};

extern CPredictionSystem g_PredictionSystem;