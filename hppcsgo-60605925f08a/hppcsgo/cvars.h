struct weapons_s
{
	bool	trigger;
	bool	trigger_head;
	bool	trigger_chest;
	bool	trigger_stomach;
	bool	trigger_arms;
	bool	trigger_legs;
	int		trigger_accuracy;
	float	trigger_spread_hit_chance;
	bool	trigger_automatic_penetration;
	int		trigger_automatic_penetration_minimum_damage;

	bool	aim;
	bool	aim_head;
	bool	aim_chest;
	bool	aim_stomach;
	bool	aim_arms;
	bool	aim_legs;
	bool	aim_automatic_penetration;
	int		aim_automatic_penetration_minimum_damage;
	float	aim_fov;
	float	aim_smooth_auto;
	float	aim_smooth_in_attack;
	float	aim_recoil_smooth;
	float	aim_smooth_scale_fov;
	float	aim_recoil_fov;
	int		aim_recoil_pitch;
	int		aim_recoil_yaw;
	int		aim_recoil_start;
	float	aim_delay_before_firing;
	float	aim_delay_before_aiming;
	float	aim_maximum_lock_on_time;
	int		aim_accuracy;
	float	aim_spread_hit_chance;
	int		aim_reaction_time;
};

struct legitbot_s
{
	bool	active;
	bool	friendly_fire;
	bool	aim_automatic_scope;
	bool	aim_dependence_tickrate;
	bool	smoke_check;
	bool	trigger_only_scoped;
	bool	position_adjustment;

	weapons_s weapon[65];
};

struct rageweapons_s
{
	bool	aim;
	float	aim_fov;
	float	aim_spread_hit_chance;
	bool	aim_multi_point;
	float	aim_multi_point_scale;
	int		aim_minimum_damage;
};

struct ragebot_s
{
	bool	active;
	int		active_panic_key;

	bool	aim_friendly_fire;
	bool	aim_automatic_scope;
	bool	aim_auto_fire;
	bool	aim_silent;
	bool	aim_quick_stop;

	rageweapons_s weapon[65];
};

struct visuals_s
{
	bool	active;

	bool	esp_player_enabled;
	bool	esp_player_enemies;
	bool	esp_player_teammates;
	bool	esp_player_visible_check;
	int		esp_player_box;
	int		esp_player_box_thickness;
	bool	esp_player_box_outline;
	float	esp_player_box_color_hide_t[4];
	float	esp_player_box_color_hide_ct[4];
	float	esp_player_box_color_visible_t[4];
	float	esp_player_box_color_visible_ct[4];
	int		esp_player_name;
	int		esp_player_weapon;
	bool	esp_player_weapon_ammo;
	int		esp_player_distance;
	int		esp_player_distance_measure;
	int		esp_player_health;
	int		esp_player_armor;
	bool	esp_player_skeleton;
	float	esp_player_skeleton_color[4];
	int		esp_player_out_of_pov;
	float	esp_player_out_of_pov_radius;
	float	esp_player_history_time;
	bool	esp_player_fadeout_dormant;
	bool	esp_sound_enabled;
	int		esp_sound_draw_type;
	float	esp_sound_size;
	float	esp_sound_fadeout_time;
	float	esp_sound_color_t[4];
	float	esp_sound_color_ct[4];
	float	esp_font_size;
	int		esp_font_outline;
	float	esp_font_color[4];

	bool	shared_esp;
	int		max_render_calls;
};

struct kreedz_s
{
	bool		active;
};

struct misc_s
{
	bool		automatic_pistol;
	bool		automatic_reload;
	bool		automatic_revolver;
};

struct settings_s
{
	bool		notifications_enabled;
	float		notifications_time;

	int			tooltip_language;
};

class CCvars
{
public:
	CCvars();

	legitbot_s	legitbot;
	ragebot_s	ragebot;
	visuals_s	visuals;
	kreedz_s	kreedz;
	misc_s		misc;
	settings_s	settings;

	float		gui[ImGuiCol_COUNT][4];
};

extern CCvars cvar;