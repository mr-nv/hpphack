#include "ValveSDK/misc/EHandle.hpp"
#include "ValveSDK/Interfaces/IRefCounted.hpp"

#define MAX_CLIENTS 64

#define NETVAR(type, name, table, netvar)                           \
    type& name##() const {                                          \
        static int _##name = g_NetvarSys.GetOffset(table, netvar);     \
        return *(type*)((uintptr_t)this + _##name);                 \
    }

#define PNETVAR(type, name, table, netvar)                           \
    type* name##() const {                                          \
        static int _##name = g_NetvarSys.GetOffset(table, netvar);     \
        return (type*)((uintptr_t)this + _##name);                 \
    }

#define NETPROP(name, table, netvar) static RecvProp* name() \
{ \
	static auto prop_ptr = g_NetvarSys.GetNetvarProp(table,netvar); \
	return prop_ptr; \
}

struct CAnimState {

	char pad_0x0000[0x18]; //0x0000
	float anim_update_timer; //0x0018 
	char pad_0x001C[0xC]; //0x001C
	float started_moving_time; //0x0028 
	float last_move_time; //0x002C 
	char pad_0x0030[0x10]; //0x0030
	float last_lby_time; //0x0040 
	char pad_0x0044[0x8]; //0x0044
	float run_amount; //0x004C 
	char pad_0x0050[0x10]; //0x0050
	void *entity; //0x0060 
	__int32 active_weapon; //0x0064 
	__int32 last_active_weapon; //0x0068 
	float last_client_side_animation_update_time; //0x006C 
	__int32 last_client_side_animation_update_framecount; //0x0070 
	float eye_timer; //0x0074 
	float eye_angles_y; //0x0078 
	float eye_angles_x; //0x007C 
	float goal_feet_yaw; //0x0080 
	float current_feet_yaw; //0x0084 
	float torso_yaw; //0x0088 
	float last_move_yaw; //0x008C 
	float lean_amount; //0x0090 
	char pad_0x0094[0x4]; //0x0094
	float feet_cycle; //0x0098 
	float feet_yaw_rate; //0x009C 
	char pad_0x00A0[0x4]; //0x00A0
	float duck_amount; //0x00A4 
	float landing_duck_amount; //0x00A8 
	char pad_0x00AC[0x4]; //0x00AC
	Vector current_origin;
	Vector last_origin;
	float velocity_x; //0x00C8 
	float velocity_y; //0x00CC 
	char pad_0x00D0[0x10]; //0x00D0
	float move_direction_1; //0x00E0 
	float move_direction_2; //0x00E4 
	char pad_0x00E8[0x4]; //0x00E8
	float m_velocity; //0x00EC 
	float jump_fall_velocity; //0x00F0 
	float clamped_velocity; //0x00F4 
	float feet_speed_forwards_or_sideways; //0x00F8 
	float feet_speed_unknown_forwards_or_sideways; //0x00FC 
	float last_time_started_moving; //0x0100 
	float last_time_stopped_moving; //0x0104 
	bool on_ground; //0x0108 
	bool hit_in_ground_animation; //0x010C 
	char pad_0x0110[0x4]; //0x0110
	float last_origin_z; //0x0114 
	float head_from_ground_distance_standing; //0x0118 
	float stop_to_full_running_fraction; //0x011C 
	char pad_0x0120[0x14]; //0x0120
	__int32 is_not_moving; //0x0134 
	char pad_0x0138[0x20]; //0x0138
	float last_anim_update_time; //0x0158 
	float moving_direction_x; //0x015C 
	float moving_direction_y; //0x0160 
	float moving_direction_z; //0x0164 
	char pad_0x0168[0x44]; //0x0168
	__int32 started_moving; //0x01AC 
	char pad_0x01B0[0x8]; //0x01B0
	float lean_yaw; //0x01B8 
	char pad_0x01BC[0x8]; //0x01BC
	float poses_speed; //0x01C4 
	char pad_0x01C8[0x8]; //0x01C8
	float ladder_speed; //0x01D0 
	char pad_0x01D4[0x8]; //0x01D4
	float ladder_yaw; //0x01DC 
	char pad_0x01E0[0x8]; //0x01E0
	float some_pose; //0x01E8 
	char pad_0x01EC[0x14]; //0x01EC
	float body_yaw; //0x0200 
	char pad_0x0204[0x8]; //0x0204
	float body_pitch; //0x020C 
	char pad_0x0210[0x8]; //0x0210
	float death_yaw; //0x0218 
	char pad_0x021C[0x8]; //0x021C
	float stand; //0x0224 
	char pad_0x0228[0x8]; //0x0228
	float jump_fall; //0x0230 
	char pad_0x0234[0x8]; //0x0234
	float aim_blend_stand_idle; //0x023C 
	char pad_0x0240[0x8]; //0x0240
	float aim_blend_crouch_idle; //0x0248 
	char pad_0x024C[0x8]; //0x024C
	float strafe_yaw; //0x0254 
	char pad_0x0258[0x8]; //0x0258
	float aim_blend_stand_walk; //0x0260 
	char pad_0x0264[0x8]; //0x0264
	float aim_blend_stand_run; //0x026C 
	char pad_0x0270[0x8]; //0x0270
	float aim_blend_crouch_walk; //0x0278 
	char pad_0x027C[0x8]; //0x027C
	float move_blend_walk; //0x0284 
	char pad_0x0288[0x8]; //0x0288
	float move_blend_run; //0x0290 
	char pad_0x0294[0x8]; //0x0294
	float move_blend_crouch; //0x029C 
	char pad_0x02A0[0x4]; //0x02A0
	float speed; //0x02A4 
	__int32 moving_in_any_direction; //0x02A8 
	float acceleration; //0x02AC 
	char pad_0x02B0[0x74]; //0x02B0
	float crouch_height; //0x0324 
	__int32 is_full_crouched; //0x0328 
	char pad_0x032C[0x4]; //0x032C
	float velocity_subtract_x; //0x0330 
	float velocity_subtract_y; //0x0334 
	float velocity_subtract_z; //0x0338 
	float standing_head_height; //0x033C 
	char pad_0x0340[0x4]; //0x0340
}; //Size=0x0344

enum CSWeaponType
{
	WEAPONTYPE_KNIFE = 0,
	WEAPONTYPE_PISTOL,
	WEAPONTYPE_SUBMACHINEGUN,
	WEAPONTYPE_RIFLE,
	WEAPONTYPE_SHOTGUN,
	WEAPONTYPE_SNIPER_RIFLE,
	WEAPONTYPE_MACHINEGUN,
	WEAPONTYPE_C4,
	WEAPONTYPE_PLACEHOLDER,
	WEAPONTYPE_GRENADE,
	WEAPONTYPE_UNKNOWN
};

struct datamap_t;

class C_BaseEntity : public IClientEntity
{
public:
	datamap_t * GetDataDescMap() {
		typedef datamap_t*(__thiscall *o_GetPredDescMap)(void*);
		return CallVFunction<o_GetPredDescMap>(this, 15)(this);
	}

	datamap_t *GetPredDescMap() {
		typedef datamap_t*(__thiscall *o_GetPredDescMap)(void*);
		return CallVFunction<o_GetPredDescMap>(this, 17)(this);
	}
	static __forceinline C_BaseEntity* GetEntityByIndex(int index) { return static_cast<C_BaseEntity*>(g_pIEntityList->GetClientEntity(index)); }
	static __forceinline C_BaseEntity* get_entity_from_handle(CBaseHandle h) { return static_cast<C_BaseEntity*>(g_pIEntityList->GetClientEntityFromHandle(h)); }

	NETVAR(int32_t, m_nModelIndex, "DT_BaseEntity", "m_nModelIndex");
	NETVAR(int32_t, m_iTeamNum, "DT_BaseEntity", "m_iTeamNum");
	NETVAR(Vector, m_vecOrigin, "DT_BaseEntity", "m_vecOrigin");
	NETVAR(Vector, m_vecAngles, "DT_BaseEntity", "m_vecAngles");
	NETVAR(bool, m_bShouldGlow, "DT_DynamicProp", "m_bShouldGlow");
	NETVAR(CHandle<C_BasePlayer>, m_hOwnerEntity, "DT_BaseEntity", "m_hOwnerEntity");
	NETVAR(bool, m_bSpotted, "DT_BaseEntity", "m_bSpotted");
	NETVAR(float_t, m_flC4Blow, "DT_PlantedC4", "m_flC4Blow");

	const matrix3x4_t& m_rgflCoordinateFrame()
	{
		static auto _m_rgflCoordinateFrame = g_NetvarSys.GetOffset("DT_BaseEntity", "m_CollisionGroup") - 0x30;
		return *(matrix3x4_t*)((uintptr_t)this + _m_rgflCoordinateFrame);
	}

	bool IsPlayer();
	bool IsLoot();
	bool IsWeapon();
	bool IsPlantedC4();
	bool IsDefuseKit();
	//bool isSpotted();
};

class C_BaseWeaponWorldModel : public C_BaseEntity
{
public:
	NETVAR(int32_t, m_nModelIndex, "DT_BaseWeaponWorldModel", "m_nModelIndex");
};

class C_EconItemView
{
private:
	using str_32 = char[32];
public:
	NETVAR(int32_t, m_bInitialized, "DT_BaseAttributableItem", "m_bInitialized");
	NETVAR(short, m_iItemDefinitionIndex, "DT_BaseAttributableItem", "m_iItemDefinitionIndex");
	NETVAR(int32_t, m_iEntityLevel, "DT_BaseAttributableItem", "m_iEntityLevel");
	NETVAR(int32_t, m_iAccountID, "DT_BaseAttributableItem", "m_iAccountID");
	NETVAR(int32_t, m_iItemIDLow, "DT_BaseAttributableItem", "m_iItemIDLow");
	NETVAR(int32_t, m_iItemIDHigh, "DT_BaseAttributableItem", "m_iItemIDHigh");
	NETVAR(int32_t, m_iEntityQuality, "DT_BaseAttributableItem", "m_iEntityQuality");
	NETVAR(str_32, m_iCustomName, "DT_BaseAttributableItem", "m_szCustomName");
};

class C_BaseAttributableItem : public C_BaseEntity
{
public:
	NETVAR(uint64_t, m_OriginalOwnerXuid, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
	NETVAR(int32_t, m_OriginalOwnerXuidLow, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
	NETVAR(int32_t, m_OriginalOwnerXuidHigh, "DT_BaseAttributableItem", "m_OriginalOwnerXuidHigh");
	NETVAR(int32_t, m_nFallbackStatTrak, "DT_BaseAttributableItem", "m_nFallbackStatTrak");
	NETVAR(int32_t, m_nFallbackPaintKit, "DT_BaseAttributableItem", "m_nFallbackPaintKit");
	NETVAR(int32_t, m_nFallbackSeed, "DT_BaseAttributableItem", "m_nFallbackSeed");
	NETVAR(float_t, m_flFallbackWear, "DT_BaseAttributableItem", "m_flFallbackWear");

	NETVAR(C_EconItemView, m_Item2, "DT_BaseAttributableItem", "m_Item");

	C_EconItemView& m_Item()
	{
		// Cheating. It should be this + m_Item netvar but then the netvars inside C_EconItemView wont work properly.
		// A real fix for this requires a rewrite of the netvar manager
		return *(C_EconItemView*)this;
	}

	void SetGloveModelIndex(int modelIndex);
};

class C_BaseCombatWeapon : public C_BaseAttributableItem
{
public:
	NETVAR(float_t, m_flNextPrimaryAttack, "DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
	NETVAR(float_t, m_flNextSecondaryAttack, "DT_BaseCombatWeapon", "m_flNextSecondaryAttack");
	NETVAR(int32_t, m_iClip1, "DT_BaseCombatWeapon", "m_iClip1");
	NETVAR(int32_t, m_iClip2, "DT_BaseCombatWeapon", "m_iClip2");
	NETVAR(float_t, m_flRecoilIndex, "DT_WeaponCSBase", "m_flRecoilIndex");
	NETVAR(int32_t, m_iViewModelIndex, "DT_BaseCombatWeapon", "m_iViewModelIndex");
	NETVAR(int32_t, m_iWorldModelIndex, "DT_BaseCombatWeapon", "m_iWorldModelIndex");
	NETVAR(int32_t, m_iWorldDroppedModelIndex, "DT_BaseCombatWeapon", "m_iWorldDroppedModelIndex");
	NETVAR(bool, m_bPinPulled, "DT_BaseCSGrenade", "m_bPinPulled");
	NETVAR(float_t, m_fThrowTime, "DT_BaseCSGrenade", "m_fThrowTime");
	NETVAR(float_t, m_flPostponeFireReadyTime, "DT_WeaponCSBase", "m_flPostponeFireReadyTime");
	NETVAR(CHandle<C_BaseWeaponWorldModel>, m_hWeaponWorldModel, "DT_BaseCombatWeapon", "m_hWeaponWorldModel");

	CCSWeaponInfo* GetCSWeaponData();
	bool HasBullets();
	bool CanFire();
	bool IsGrenade();
	bool IsKnife();
	bool IsReloading();
	bool IsRifle();
	bool IsPistol();
	bool IsSniper();
	bool IsGun();
	float GetInaccuracy();
	float GetSpread();
	void UpdateAccuracyPenalty();
	CUtlVector<IRefCounted*>& m_CustomMaterials();
	bool* m_bCustomMaterialInitialized();
};

class C_PlantedC4
{
public:
	NETVAR(bool, m_bBombTicking, "DT_PlantedC4", "m_bBombTicking");
	NETVAR(bool, m_bBombDefused, "DT_PlantedC4", "m_bBombDefused");
	NETVAR(float, m_flC4Blow, "DT_PlantedC4", "m_flC4Blow");
	NETVAR(float, m_flTimerLength, "DT_PlantedC4", "m_flTimerLength");
	NETVAR(float, m_flDefuseLength, "DT_PlantedC4", "m_flDefuseLength");
	NETVAR(float, m_flDefuseCountDown, "DT_PlantedC4", "m_flDefuseCountDown");
	NETVAR(CHandle<C_BasePlayer>, m_hBombDefuser, "DT_PlantedC4", "m_hBombDefuser");
};

class C_BasePlayer : public C_BaseEntity
{
public:
	static __forceinline C_BasePlayer* GetPlayerByUserId(int id) { return static_cast<C_BasePlayer*>(GetEntityByIndex(g_pIEngine->GetPlayerForUserID(id))); }
	static __forceinline C_BasePlayer* GetPlayerByIndex(int i) { return static_cast<C_BasePlayer*>(GetEntityByIndex(i)); }

	NETVAR(bool, m_bHasDefuser, "DT_CSPlayer", "m_bHasDefuser");
	NETVAR(bool, m_bGunGameImmunity, "DT_CSPlayer", "m_bGunGameImmunity");
	NETVAR(int32_t, m_iShotsFired, "DT_CSPlayer", "m_iShotsFired");
	NETVAR(QAngle, m_angEyeAngles, "DT_CSPlayer", "m_angEyeAngles[0]");
	NETVAR(int, m_ArmorValue, "DT_CSPlayer", "m_ArmorValue");
	NETVAR(bool, m_bHasHeavyArmor, "DT_CSPlayer", "m_bHasHeavyArmor");
	NETVAR(bool, m_bHasHelmet, "DT_CSPlayer", "m_bHasHelmet");
	NETVAR(bool, m_bIsScoped, "DT_CSPlayer", "m_bIsScoped");;
	NETVAR(float, m_flLowerBodyYawTarget, "DT_CSPlayer", "m_flLowerBodyYawTarget");
	NETVAR(int32_t, m_iHealth, "DT_BasePlayer", "m_iHealth");
	NETVAR(int32_t, m_lifeState, "DT_BasePlayer", "m_lifeState");
	NETVAR(int32_t, m_fFlags, "DT_BasePlayer", "m_fFlags");
	NETVAR(int32_t, m_nTickBase, "DT_BasePlayer", "m_nTickBase");
	NETVAR(int32_t, m_iFOV, "DT_BasePlayer", "m_iFOV");
	NETVAR(int32_t, m_iDefaultFOV, "DT_BasePlayer", "m_iDefaultFOV");
	NETVAR(Vector, m_vecViewOffset, "DT_BasePlayer", "m_vecViewOffset[0]");
	NETVAR(QAngle, m_viewPunchAngle, "DT_BasePlayer", "m_viewPunchAngle");
	NETVAR(QAngle, m_aimPunchAngle, "DT_BasePlayer", "m_aimPunchAngle");
	NETVAR(CHandle<C_BaseViewModel>, m_hViewModel, "DT_BasePlayer", "m_hViewModel[0]");
	NETVAR(Vector, m_vecVelocity, "DT_BasePlayer", "m_vecVelocity[0]");
	NETVAR(float, m_flMaxspeed, "DT_BasePlayer", "m_flMaxspeed");
	NETVAR(ObserverMode_t, m_iObserverMode, "DT_BasePlayer", "m_iObserverMode");
	NETVAR(CHandle<C_BasePlayer>, m_hObserverTarget, "DT_BasePlayer", "m_hObserverTarget");
	NETVAR(float, m_flFlashMaxAlpha, "DT_CSPlayer", "m_flFlashMaxAlpha");
	NETVAR(int32_t, m_nHitboxSet, "DT_BaseAnimating", "m_nHitboxSet");
	NETVAR(CHandle<C_BaseCombatWeapon>, m_hActiveWeapon, "DT_BaseCombatCharacter", "m_hActiveWeapon");
	NETVAR(int32_t, m_iAccount, "DT_CSPlayer", "m_iAccount");
	NETVAR(float, m_flFlashDuration, "DT_CSPlayer", "m_flFlashDuration");
	NETVAR(float, m_flSimulationTime, "DT_BaseEntity", "m_flSimulationTime");
	NETVAR(float, m_flCycle, "DT_ServerAnimationData", "m_flCycle");
	NETVAR(int, m_nSequence, "DT_BaseViewModel", "m_nSequence");
	PNETVAR(char, m_szLastPlaceName, "DT_BasePlayer", "m_szLastPlaceName");
	NETPROP(m_flLowerBodyYawTargetProp, "DT_CSPlayer", "m_flLowerBodyYawTarget");

	NETVAR(QAngle, m_angAbsAngles, "DT_BaseEntity", "m_angAbsAngles");
	NETVAR(Vector, m_angAbsOrigin, "DT_BaseEntity", "m_angAbsOrigin");
	NETVAR(float, m_flDuckSpeed, "DT_BaseEntity", "m_flDuckSpeed");
	NETVAR(float, m_flDuckAmount, "DT_BaseEntity", "m_flDuckAmount");

	std::array<float, 24> m_flPoseParameter() const {
		static int _m_flPoseParameter = g_NetvarSys.GetOffset("DT_BaseAnimating", "m_flPoseParameter");
		return *(std::array<float, 24>*)((uintptr_t)this + _m_flPoseParameter);
	}

	PNETVAR(CHandle<C_BaseCombatWeapon>, m_hMyWeapons, "DT_BaseCombatCharacter", "m_hMyWeapons");
	PNETVAR(CHandle<C_BaseAttributableItem>, m_hMyWearables, "DT_BaseCombatCharacter", "m_hMyWearables");

	/*CUserCmd*& m_pCurrentCommand();

	void InvalidateBoneCache();
	int GetNumAnimOverlays();
	AnimationLayer *GetAnimOverlays();
	AnimationLayer *GetAnimOverlay(int i);
	int GetSequenceActivity(int sequence);
	CCSGOPlayerAnimState *GetPlayerAnimState();

	static void UpdateAnimationState(CCSGOPlayerAnimState *state, QAngle angle);
	static void ResetAnimationState(CCSGOPlayerAnimState *state);
	void CreateAnimationState(CCSGOPlayerAnimState *state);

	float_t &m_surfaceFriction()
	{
		static unsigned int _m_surfaceFriction = Utils::FindInDataMap(GetPredDescMap(), "m_surfaceFriction");
		return *(float_t*)((uintptr_t)this + _m_surfaceFriction);
	}
	Vector &m_vecBaseVelocity()
	{
		static unsigned int _m_vecBaseVelocity = Utils::FindInDataMap(GetPredDescMap(), "m_vecBaseVelocity");
		return *(Vector*)((uintptr_t)this + _m_vecBaseVelocity);
	}

	float_t &m_flMaxspeed()
	{
		static unsigned int _m_flMaxspeed = Utils::FindInDataMap(GetPredDescMap(), "m_flMaxspeed");
		return *(float_t*)((uintptr_t)this + _m_flMaxspeed);
	}
	*/
	Vector        GetEyePos();
	player_info_t GetPlayerInfo();
	bool          IsAlive();
	Vector        GetHitboxPos(int hitbox_id);
	Vector        GetBonePos(int bone);
	/*bool		  IsFlashed();
	bool          HasC4();	
	mstudiobbox_t * GetHitbox(int hitbox_id);
	bool          GetHitboxPos(int hitbox, Vector &output);
	
	bool          CanSeePlayer(C_BasePlayer* player, int hitbox);
	bool          CanSeePlayer(C_BasePlayer* player, const Vector& pos);
	void		UpdateClientSideAnimation();

	int m_nMoveType();

	QAngle * GetVAngles();

	float GetFlashBangTime();

	float_t m_flSpawnTime();*/

	void InvalidateBoneCache();
	void UpdateClientSideAnimation();
	void SetAbsOrigin(const Vector &origin);
	void SetAbsAngles(const QAngle &angles);
	IClientRenderable* GetClientRenderable();
	matrix3x4_t* GetBoneCache();
	int* GetBoneCacheCount();
	float GetMaxDesynsAngle();
	CAnimState *AnimState();
};