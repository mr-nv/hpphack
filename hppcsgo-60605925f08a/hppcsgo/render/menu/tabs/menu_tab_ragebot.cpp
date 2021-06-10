#include "..\menu_consts.h"
#include "menu_tabs.h"

#include "..\..\..\cvars.h"

void CMenuTabs::TabRageBot()
{
	BeginTab(&cvar.ragebot.active, "Active##Ragebot", &cvar.ragebot.active_panic_key, true);

	if (cvar.ragebot.active)
		cvar.legitbot.active = false;

	WeaponCombos();

	auto* weapon = &cvar.ragebot.weapon[m_current_weapon_id];

	BeginTab();
	BeginColumn();

	BeginChild("Aimbot##Ragebot", ImVec2(GetChildWidth(), 330));
	PushId("Ragebot.Aimbot");
	Checkbox(&weapon->aim, "Enabled", cvar.settings.tooltip_language ? u8"Глобальное включение/выключение аимбота на оружии." : "Global on/off aimbot on weapon.");
	PushCondition(weapon->aim);
	SliderFloat(&weapon->aim_fov, "Maximum FOV", 0.f, 180.f, u8"%.1f°", 2.f, cvar.settings.tooltip_language ? u8"Максимальный угол обзора аимбота." : "Maximal field of view of the aimbot.");
	Checkbox(&weapon->aim_multi_point, "Multi-point");
	SliderFloat(&weapon->aim_multi_point_scale, "Multi-point scale", 0.1f, 100.f, "%.1f%%", 1.f);
	SliderInt(&weapon->aim_minimum_damage, "Minimum damage", 1, 100, "%.0f hp");
	SliderFloat(&weapon->aim_spread_hit_chance, "Spread hit chance", 1.f, 100.f, "%.0f%%", 1.f);
	PopCondition();
	EndChild();

	ImGui::Spacing();
	ImGui::Spacing();

	BeginChild("Fake lag##Ragebot", ImVec2(GetChildWidth(), 100));
	PushId("Ragebot.Fake lag");
	EndChild();

	NextColumn();

	BeginChild("Other##Ragebot", ImVec2(GetChildWidth(), 200));
	PushId("Ragebot.Other");
	Checkbox(&cvar.ragebot.aim_auto_fire, "Automatic fire");
	Checkbox(&cvar.ragebot.aim_friendly_fire, "Friendly fire");
	Checkbox(&cvar.ragebot.aim_automatic_scope, "Automatic scope");
	Checkbox(&cvar.ragebot.aim_silent, "Silent");
	Checkbox(&cvar.ragebot.aim_quick_stop, "Quick stop");
	EndChild();

	ImGui::Spacing();
	ImGui::Spacing();

	BeginChild("Anti-aimbot##Ragebot", ImVec2(GetChildWidth(), 230));
	PushId("Ragebot.Anti-aimbot");
	EndChild();

	EndColumn();
	EndTab();
}