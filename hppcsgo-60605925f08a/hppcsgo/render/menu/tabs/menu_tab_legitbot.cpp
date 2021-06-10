#include "..\menu_consts.h"
#include "menu_tabs.h"

#include "..\..\..\cvars.h"

void CMenuTabs::TabLegitBot()
{
	BeginTab(&cvar.legitbot.active, "Active##Legitbot", 0, true);

	if (cvar.legitbot.active)
		cvar.ragebot.active = false;

	WeaponCombos();

	auto* weapon = &cvar.legitbot.weapon[m_current_weapon_id];

	BeginTab();
	BeginColumn();

	BeginChild("Aimbot##Legitbot", ImVec2(GetChildWidth(), 442));
	PushId("Legitbot.Aimbot");
	Checkbox(&weapon->aim, "Enabled", "Global on/off aimbot on weapon");
	PushCondition(weapon->aim);
	Checkbox(&weapon->aim_head, "Head", "");
	Checkbox(&weapon->aim_chest, "Chest", "");
	Checkbox(&weapon->aim_stomach, "Stomach", "");
	Checkbox(&weapon->aim_arms, "Arms", "");
	Checkbox(&weapon->aim_legs, "Legs", "");
	SliderFloat(&weapon->aim_maximum_lock_on_time, "Maximum lock-on time", 0.f, 10000.f,
		weapon->aim_maximum_lock_on_time ? "%.0f ms" : "infinity", 1.f, "The aimbot time when the target is in the field of view of aimbot.");
	SliderInt(&weapon->aim_reaction_time, "Reaction time", 0, 1000, "%.0f ms", "Delay before aiming to the next player after killing the previous one.");
	SliderFloat(&weapon->aim_fov, "Maximum FOV", 0.f, 180.f, u8"%.1f°", 2.f, "Maximal field of view of the aimbot.");
	SliderFloat(&weapon->aim_smooth_auto, "Smooth (auto aim)", 0.f, 300.f, "%.2f", 2.f, "Smoothing auto aiming aimbot.");
	SliderFloat(&weapon->aim_smooth_in_attack, "Smooth in attack", 0.f, 300.f, "%.2f", 2.f, "Smoothing aiming aimbot in attack.");
	SliderFloat(&weapon->aim_smooth_scale_fov, "Smooth scale - FOV", 0.f, 100.f, "%.0f%%", 1.f); // что ето такое
	SliderFloat(&weapon->aim_recoil_fov, "Recoil compensation maximum FOV", 0.f, 180.f, u8"%.1f°", 2.f, "Maximal field of view of the aimbot when working recoil compensation.");
	SliderFloat(&weapon->aim_recoil_smooth, "Recoil compensation smooth", 0.f, 300.f, "%.2f", 2.f);
	SliderInt(&weapon->aim_recoil_pitch, "Recoil compensation pitch", 0, 100, "%.0f%%", "The percentage of efficiency of recoil compensation (pitch angle).");
	SliderInt(&weapon->aim_recoil_yaw, "Recoil compensation yaw", 0, 100, "%.0f%%", "The percentage of efficiency of recoil compensation (yaw angle).");
	SliderInt(&weapon->aim_recoil_start, "Recoil compensation start", 0, 15, weapon->aim_recoil_start ? "%.0f bullet" : "if punchangle2D > 0");
	SliderFloat(&weapon->aim_delay_before_firing, "Delay before firing", 0.f, 1000.f, "%.0f ms", 2.f, "Delay before firing.");
	SliderFloat(&weapon->aim_delay_before_aiming, "Delay before aiming", 0.f, 1000.f, "%.0f ms", 2.f, "Delay before aiming.");
	Checkbox(&weapon->aim_automatic_penetration, "Automatic penetration", "");
	SliderInt(&weapon->aim_automatic_penetration_minimum_damage, "Automatic penetration min. damage", 1, 100, "%.0f%% hp");
	Combo(&weapon->aim_accuracy, "Accuracy boost", IM_ARRAYCOMBO(aim_accuracy_boost));
	SliderFloat(&weapon->aim_spread_hit_chance, "Spread hit chance", 1.f, 100.f, "%.0f%%", 1.f);
	PopCondition();
	EndChild();

	NextColumn();

	BeginChild("Triggerbot##Legitbot", ImVec2(GetChildWidth(), 230));
	PushId("Legitbot.Triggerbot");
	Checkbox(&weapon->trigger, "Enabled", "Global on/off triggerbot on weapon");
	PushCondition(weapon->trigger);
	Checkbox(&weapon->trigger_head, "Head", "");
	Checkbox(&weapon->trigger_chest, "Chest", "");
	Checkbox(&weapon->trigger_stomach, "Stomach", "");
	Checkbox(&weapon->trigger_arms, "Arms", "");
	Checkbox(&weapon->trigger_legs, "Legs", "");
	Checkbox(&weapon->trigger_automatic_penetration, "Automatic penetration", "");
	SliderInt(&weapon->trigger_automatic_penetration_minimum_damage, "Automatic penetration min. damage", 1, 100, "%.0f%% hp");
	Combo(&weapon->trigger_accuracy, "Accuracy boost", IM_ARRAYCOMBO(trigger_accuracy_boost));
	SliderFloat(&weapon->trigger_spread_hit_chance, "Spread hit chance", 1.f, 100.f, "%.0f%%", 1.f);
	PopCondition();
	EndChild();

	ImGui::Spacing();
	ImGui::Spacing();

	BeginChild("Other##Legitbot", ImVec2(GetChildWidth(), 200));
	PushId("Legitbot.Other");
	Checkbox(&cvar.legitbot.friendly_fire, "Friendly fire");
	Checkbox(&cvar.legitbot.aim_automatic_scope, "Legitbot automatic scope", "Automatic opening sniper scope.");
	Checkbox(&cvar.legitbot.aim_dependence_tickrate, "Legitbot dependence tickrate", "");
	Checkbox(&cvar.legitbot.smoke_check, "Smoke check", "");
	Checkbox(&cvar.legitbot.trigger_only_scoped, "Triggerbot only scoped", "");
	Checkbox(&cvar.legitbot.position_adjustment, "Position adjustment", "");
	EndChild();

	EndColumn();
	EndTab();
}