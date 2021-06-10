class CUtils;
class CUtils
{
public:
	bool	IsBoxIntersectingRay(const Vector& vecBoundBoxMin, const Vector& vecBoundBoxMax, const Vector& vecOrigin, const Vector& vecDelta);
	bool	IsBehindSmoke(Vector src, Vector rem);
	bool	IsVisible(int iEntIndex, const Vector& vecTarget, bool bSmokeCheck = true);
	bool	IsValidPlayer(C_BasePlayer *player, bool teammates = false);

	float   RandomFloat(float min, float max);
	float   AngleBetween(Vector a, Vector b);
	float	raySphereIntersect(Vector r0, Vector rd, Vector s0, float sr);

	void	MatrixCopy(const matrix3x4_t& in, matrix3x4_t& out);
	void	VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out);
	void	VectorTransform(Vector in1, const matrix3x4_t &in2, Vector & out, int xyz, float multi);
	void	ClampAngles(QAngle& angles);
	void	VectorAngles(const Vector& forward, QAngle& angles);
	void	RandomSeed(int iSeed);
	void	NormalizeAngles(float* flAngles);
	void	MakeAngle(bool a_bAddAngles, float* a_flAngles, CUserCmd* cmd);
	void	FixMoveEnd(CUserCmd* cmd);
	void	FixMoveStart(CUserCmd* cmd);

	bool	IsGun(const std::string& name);

	bool	WorldToScreen(const Vector& vecOrigin, float* flScreenPos);

private:
	QAngle	_QPreviousAngles;

	bool	ScreenTransform(const Vector& vecPoint, float* flScreenPos);
};

extern CUtils g_Utils;

bool  FileExists(const char* _FileName);
void  TraceLog(const char *fmt, ...);

void  UnloadCheatDLL(void);
bool  IsUnloadingCheatDLL(void);

float fInterp(float s1, float s2, float s3, float f1, float f3);


static std::map<int, const char*>& GetWpnData()
{
	static std::map<int, const char*> wpn_data =
	{
		/* PISTOLS */
			{WEAPON_DEAGLE,"deagle"},
			{WEAPON_ELITE,"elite"},
			{WEAPON_FIVESEVEN,"fiveseven"},
			{WEAPON_GLOCK,"glock"},
			{WEAPON_USPS,"usp-s"},
			{WEAPON_CZ75,"cz75"},
			{WEAPON_TEC9,"tec9"},
			{WEAPON_P2000,"p2000"},
			{WEAPON_P250,"p250"},
			{WEAPON_REVOLVER,"revolver"},
			/* RIFLES */
			{WEAPON_AK47,"ak47"},
			{WEAPON_AUG,"aug"},
			{WEAPON_FAMAS,"famas"},
			{WEAPON_M4A4,"m4a4"},
			{WEAPON_SG553,"sg553"},
			{WEAPON_GALIL,"galil"},
			{WEAPON_M4A1S,"m4a1-s"},
			/* SUBMACHINE GUNS */
			{WEAPON_MAC10,"mac10"},
			{WEAPON_P90,"p90"},
			{WEAPON_MP5SD,"mp5-sd"},
			{WEAPON_UMP45,"ump45"},
			{WEAPON_BIZON,"bizon"},
			{WEAPON_MP7,"mp7"},
			{WEAPON_MP9,"mp9"},
			/* MACHINE GUNS */
			{WEAPON_M249,"m249"},
			{WEAPON_NEGEV,"negev"},
			/* SNIPERS */
			{WEAPON_AWP,"awp"},
			{WEAPON_G3SG1,"g3sg1"},
			{WEAPON_SCAR20,"scar20"},
			{WEAPON_SSG08,"ssg08"},
			/* SHOTGUNS */
			{WEAPON_MAG7,"mag7"},
			{WEAPON_SAWEDOFF,"sawedoff"},
			{WEAPON_NOVA,"nova"},
			{WEAPON_XM1014,"xm1014"},
			/* GRENADES */
			{WEAPON_FLASHBANG,"flashbang"},
			{WEAPON_HEGRENADE,"hegrenade"},
			{WEAPON_SMOKEGRENADE,"smokegrenade"},
			{WEAPON_MOLOTOV,"molotov"},
			{WEAPON_DECOY,"decoy"},
			{WEAPON_INC,"incgrenade"},
			/* OTHER */
			{WEAPON_ZEUS,"zeus"},
			{WEAPON_C4,"c4"},
			/* KNIFES */
			{WEAPON_KNIFE,"knife"},
			{WEAPON_KNIFE_T,"knife"},
			{WEAPON_KNIFE_BAYONET,"knife"},
			{WEAPON_KNIFE_FLIP,"knife"},
			{WEAPON_KNIFE_GUT,"knife"},
			{WEAPON_KNIFE_KARAMBIT,"knife"},
			{WEAPON_KNIFE_M9_BAYONET,"knife"},
			{WEAPON_KNIFE_TACTICAL,"knife"},
			{WEAPON_KNIFE_FALCHION,"knife"},
			{WEAPON_KNIFE_SURVIVAL_BOWIE,"knife"},
			{WEAPON_KNIFE_BUTTERFLY,"knife"},
			{WEAPON_KNIFE_PUSH,"knife"},
			{WEAPON_KNIFE_GYPSY_JACKKNIFE,"knife"},
			{WEAPON_KNIFE_STILETTO,"knife"},
			{WEAPON_KNIFE_URSUS,"knife"},
			{WEAPON_KNIFE_WIDOWMAKER,"knife"},
			/* GLOVES */
			{GLOVE_STUDDED_BLOODHOUND,"glove"},
			{GLOVE_T_SIDE,"glove"},
			{GLOVE_CT_SIDE,"glove"},
			{GLOVE_SPORTY,"glove"},
			{GLOVE_SLICK,"glove"},
			{GLOVE_LEATHER_WRAP,"glove"},
			{GLOVE_MOTORCYCLE,"glove"},
			{GLOVE_SPECIALIST,"glove"},
			{GLOVE_HYDRA,"glove"}
	};

	return wpn_data;
}

static auto* GetWeaponNameByIndex(int index)
{
	const auto& wpn_data = GetWpnData();
	const auto entry = wpn_data.find(index);
	return entry == end(wpn_data) ? "unknown" : entry->second;
}

static int GetWeaponIndexByName(const char* name)
{
	const auto& wpn_data = GetWpnData();
	return std::find_if(wpn_data.begin(), wpn_data.end(),
		[&](const std::pair<int, const char*> a) {return a.second == name; })->first;
}