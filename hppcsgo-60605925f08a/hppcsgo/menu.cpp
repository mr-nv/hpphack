#include "main.h"
#include "menu_elements.h"

CMenuDrawing g_Menu;

static char g_szSettingsBuffer[64];

static int current_weapon[6];
static int current_weapon_group = 0;
static const char* tab_labels[] = { "RageBot", "LegitBot", "Visuals", "Kreedz", "Misc", "GUI", "Settings" };
static const char* weapon_groups[] = { "Pistol", "SubMachineGun", "Rifle", "Shotgun", "Sniper", "MachineGun" };
static const char* weapons_pistol[] = { "deagle", "elite", "fiveseven", "glock", "usp-s", "cz75a", "tec9", "hkp2000", "p250", "revolver" };
static const char* weapons_submachinegun[] = { "mac10", "p90", "mp5", "ump45", "bizon", "mp7", "mp9" };
static const char* weapons_rifle[] = { "ak47", "aug", "famas", "m4a4", "sg556", "galil", "m4a1-s" };
static const char* weapons_shotgun[] = { "mag7", "sawedoff", "xm1014","nova" };
static const char* weapons_sniper[] = { "awp", "g3sg1", "scar20", "ssg08" };
static const char* weapons_machinegun[] = { "m249", "negev" };
static const char* trigger_accuracy_boost[] = { "None", "Recoil", "Recoil & Spread" };
static const char* aim_accuracy_boost[] = { "None", "Recoil", "Recoil & Spread" };
static const char* esp_player_box[] = { "Off", "Default box", "Corner box", "Rounded box" };
static const char* esp_position[] = { "Off", "Left", "Right", "Top", "Bottom" };
static const char* esp_player_distance[] = { "In units", "In meters" };
static const char* esp_out_of_pow[] = { "Off", "Arrows", "Circles: Name", "Circles: Weapon", "Circles: Distance" };
static const char* font_outline[] = { "Off", "Shadow", "Outline" };

static void ReadFile(const char* _FileName, char* _Buffer)
{
	std::ifstream input(_FileName);
	std::size_t _ElementSize = 0;

	char ch;
	while ((ch = static_cast<char>(input.get())) != -1)
		_ElementSize++;

	input.close();

	FILE * _Stream = fopen(_FileName, "r");

	if (_Stream)
	{
		fread(_Buffer, _ElementSize, 1, _Stream);
		fclose(_Stream);
	}
}



void CMenuDrawing::ApplyColorsChanges()
{
	ImGuiStyle& style = ImGui::GetStyle();

	for (unsigned i = 0; i < ImGuiCol_COUNT; i++)
		style.Colors[i] = ImColor(cvar.gui[i]);
}

void CMenuDrawing::GetCurrentWeaponId()
{
	if (g_pIEngine->IsInGame() && m_CurrentWeaponId == WEAPON_INVALID)
	{
		std::string weapon_name = g_Weapon->m_sWeaponName;

		for (int i = 0; i < IM_ARRAYSIZE(weapons_pistol); i++)
		{
			if (weapon_name.compare(weapons_pistol[i]))
				continue;

			current_weapon_group = WEAPONGROUP_PISTOL;
			current_weapon[current_weapon_group] = i;
		}

		for (int i = 0; i < IM_ARRAYSIZE(weapons_submachinegun); i++)
		{
			if (weapon_name.compare(weapons_submachinegun[i]))
				continue;

			current_weapon_group = WEAPONGROUP_SUBMACHINEGUN;
			current_weapon[current_weapon_group] = i;
		}

		for (int i = 0; i < IM_ARRAYSIZE(weapons_rifle); i++)
		{
			if (weapon_name.compare(weapons_rifle[i]))
				continue;

			current_weapon_group = WEAPONGROUP_RIFLE;
			current_weapon[current_weapon_group] = i;
		}

		for (int i = 0; i < IM_ARRAYSIZE(weapons_shotgun); i++)
		{
			if (weapon_name.compare(weapons_shotgun[i]))
				continue;

			current_weapon_group = WEAPONGROUP_SHOTGUN;
			current_weapon[current_weapon_group] = i;
		}

		for (int i = 0; i < IM_ARRAYSIZE(weapons_sniper); i++)
		{
			if (weapon_name.compare(weapons_sniper[i]))
				continue;

			current_weapon_group = WEAPONGROUP_SNIPER_RIFLE;
			current_weapon[current_weapon_group] = i;
		}

		for (int i = 0; i < IM_ARRAYSIZE(weapons_machinegun); i++)
		{
			if (weapon_name.compare(weapons_machinegun[i]))
				continue;

			current_weapon_group = WEAPONGROUP_MACHINEGUN;
			current_weapon[current_weapon_group] = i;
		}
	}
}

void CMenuDrawing::Render()
{
	if (!IsOpened())
	{
		m_CurrentWeaponId = WEAPON_INVALID;
		return;
	}

	GetCurrentWeaponId();
	ApplyColorsChanges();

	const auto flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar;

	ImGui::SetNextWindowPosCenter(ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(850, 550));

	if (ImGui::Begin("##Menu", static_cast<bool*>(0), flags))
	{
		DrawWindowTitle();
		DrawFeaturesTabs();

		switch (m_CurrentTabId)
		{
		case TabId_RageBot:  DrawRageBot();  break;
		case TabId_LegitBot: DrawLegitBot(); break;
		case TabId_Visuals:  DrawVisuals();	 break;
		case TabId_Kreedz:	 DrawKreedz();	 break;
		case TabId_Misc:     DrawMisc();	 break;
		case TabId_Gui:      DrawGUI();	     break;
		case TabId_Settings: DrawSettings();
		}

		ImGui::End();
	}
}

void CMenuDrawing::DrawWindowTitle()
{
	static const char* menu_title = "Hpp Hack for Counter-Strike: Global Offensive";

	const float	  window_width = ImGui::GetWindowWidth();
	const ImFont* pFont = ImGui::GetIO().Fonts->Fonts[0];
	const ImVec2  text_size = pFont->CalcTextSizeA(pFont->FontSize, window_width, 0, menu_title);

	ImGui::Spacing(2);
	ImGui::SameLine((window_width - text_size.x) * 0.5f);
	ImGui::Text(menu_title);
	ImGui::Separator();
}

void CMenuDrawing::DrawFeaturesTabs()
{
	ImGuiStyle& style = ImGui::GetStyle();
	const auto ItemSpacing = style.ItemSpacing;
	const auto FrameRounding = style.FrameRounding;
	const auto ColorButton = style.Colors[ImGuiCol_Button];
	const auto ColorButtonActive = style.Colors[ImGuiCol_ButtonActive];
	const auto ColorButtonHovered = style.Colors[ImGuiCol_ButtonHovered];
	const auto ColorBorder = style.Colors[ImGuiCol_Border];
	const auto ColorScrollbarBg = style.Colors[ImGuiCol_ScrollbarBg];

	style.ItemSpacing = ImVec2(0, 1);
	style.FrameRounding = 0.f;
	style.Colors[ImGuiCol_ButtonHovered] = ColorButtonHovered;
	style.Colors[ImGuiCol_ButtonActive] = ColorButtonActive;
	style.Colors[ImGuiCol_Border] = ImColor();

	for (unsigned i = 0; i < TabId_MAX_COUNT; i++)
	{
		style.Colors[ImGuiCol_Button] = i != m_CurrentTabId ? ColorScrollbarBg : ColorButton;

		if (i > 0) ImGui::SameLine();

		const float button_width = ImGui::GetWindowWidth() / static_cast<float>(TabId_MAX_COUNT);
		if (ImGui::Button(tab_labels[i], ImVec2(button_width, 18)))
			m_CurrentTabId = static_cast<TabId>(i);
	}

	ImGui::Spacing(2);
	ImGui::Separator();

	style.Colors[ImGuiCol_Button] = ColorButton;
	style.Colors[ImGuiCol_ButtonActive] = ColorButtonActive;
	style.Colors[ImGuiCol_ButtonHovered] = ColorButtonHovered;
	style.Colors[ImGuiCol_Border] = ColorBorder;
	style.ItemSpacing = ItemSpacing;
	style.FrameRounding = FrameRounding;
}

void CMenuDrawing::DrawRageBot()
{
	ImGui::Element::Begin(&cvar.ragebot.active, "Active##ragebot", 0, true);

	if (cvar.ragebot.active)
		cvar.legitbot.active = false;

	ImGui::SameLine(422);
	ImGui::PushItemWidth(150);
	ImGui::Text("Group");
	ImGui::SameLine();
	ImGui::Combo("##RageBot.Group", &current_weapon_group, IM_ARRAYCOMBO(weapon_groups));
	ImGui::SameLine();
	ImGui::Text("\tWeapon");
	ImGui::SameLine();

	switch (current_weapon_group)
	{
	case WEAPONGROUP_PISTOL:
		ImGui::Combo("##RageBot.Weapon", &current_weapon[current_weapon_group], IM_ARRAYCOMBO(weapons_pistol));
		switch (current_weapon[current_weapon_group])
		{
		case 0: m_CurrentWeaponId = WEAPON_DEAGLE;        break;
		case 1: m_CurrentWeaponId = WEAPON_ELITE;         break;
		case 2: m_CurrentWeaponId = WEAPON_FIVESEVEN;     break;
		case 3: m_CurrentWeaponId = WEAPON_GLOCK;         break;
		case 4: m_CurrentWeaponId = WEAPON_USP_SILENCER;  break;
		case 5: m_CurrentWeaponId = WEAPON_CZ75A;         break;
		case 6: m_CurrentWeaponId = WEAPON_TEC9;          break;
		case 7: m_CurrentWeaponId = WEAPON_HKP2000;       break;
		case 8: m_CurrentWeaponId = WEAPON_P250;          break;
		case 9: m_CurrentWeaponId = WEAPON_REVOLVER;
		}
		break;
	case WEAPONGROUP_SUBMACHINEGUN:
		ImGui::Combo("##RageBot.Weapon", &current_weapon[current_weapon_group], IM_ARRAYCOMBO(weapons_submachinegun));
		switch (current_weapon[current_weapon_group])
		{
		case 0: m_CurrentWeaponId = WEAPON_MAC10;         break;
		case 1: m_CurrentWeaponId = WEAPON_P90;           break;
		case 2: m_CurrentWeaponId = WEAPON_MP5;           break;
		case 3: m_CurrentWeaponId = WEAPON_UMP45;         break;
		case 4: m_CurrentWeaponId = WEAPON_BIZON;         break;
		case 5: m_CurrentWeaponId = WEAPON_MP7;           break;
		case 6: m_CurrentWeaponId = WEAPON_MP9;
		}
		break;
	case WEAPONGROUP_RIFLE:
		ImGui::Combo("##RageBot.Weapon", &current_weapon[current_weapon_group], IM_ARRAYCOMBO(weapons_rifle));
		switch (current_weapon[current_weapon_group])
		{
		case 0: m_CurrentWeaponId = WEAPON_AK47;          break;
		case 1: m_CurrentWeaponId = WEAPON_AUG;           break;
		case 2: m_CurrentWeaponId = WEAPON_FAMAS;         break;
		case 3: m_CurrentWeaponId = WEAPON_M4A1;          break;
		case 4: m_CurrentWeaponId = WEAPON_SG556;         break;
		case 5: m_CurrentWeaponId = WEAPON_GALILAR;       break;
		case 6: m_CurrentWeaponId = WEAPON_M4A1_SILENCER;
		}
		break;
	case WEAPONGROUP_SHOTGUN:
		ImGui::Combo("##RageBot.Weapon", &current_weapon[current_weapon_group], IM_ARRAYCOMBO(weapons_shotgun));
		switch (current_weapon[current_weapon_group])
		{
		case 0: m_CurrentWeaponId = WEAPON_MAG7;          break;
		case 1: m_CurrentWeaponId = WEAPON_SAWEDOFF;      break;
		case 2: m_CurrentWeaponId = WEAPON_XM1014;        break;
		case 3: m_CurrentWeaponId = WEAPON_NOVA;
		}
		break;
	case WEAPONGROUP_SNIPER_RIFLE:
		ImGui::Combo("##RageBot.Weapon", &current_weapon[current_weapon_group], IM_ARRAYCOMBO(weapons_sniper));
		switch (current_weapon[current_weapon_group])
		{
		case 0: m_CurrentWeaponId = WEAPON_AWP;           break;
		case 1: m_CurrentWeaponId = WEAPON_G3SG1;         break;
		case 2: m_CurrentWeaponId = WEAPON_SCAR20;        break;
		case 3: m_CurrentWeaponId = WEAPON_SSG08;
		}
		break;
	case WEAPONGROUP_MACHINEGUN:
		ImGui::Combo("##RageBot.Weapon", &current_weapon[current_weapon_group], IM_ARRAYCOMBO(weapons_machinegun));
		switch (current_weapon[current_weapon_group])
		{
		case 0: m_CurrentWeaponId = WEAPON_M249;          break;
		case 1: m_CurrentWeaponId = WEAPON_NEGEV;
		}
	}

	auto* weapon = &cvar.ragebot.weapon[m_CurrentWeaponId];

	ImGui::Element::Begin();
	ImGui::Element::BeginColumn();
	ImGui::Element::BeginChild("Aimbot##Ragebot", ImVec2(ImGui::GetChildWidth(), 335));
	ImGui::Element::PushId("Ragebot.Aimbot");
	ImGui::Element::Checkbox(&weapon->aim, "Enabled", "Global on/off aimbot on weapon");
	ImGui::Element::PushCondition(weapon->aim);
	ImGui::Element::SliderFloat(&weapon->aim_fov, "Maximum FOV", 0.f, 180.f, u8"%.1f°", 2.f, "Maximal field of view of the aimbot.");
	ImGui::Element::Checkbox(&weapon->aim_multi_point, "Multi-point");
	ImGui::Element::SliderFloat(&weapon->aim_multi_point_scale, "Multi-point scale", 0.1f, 100.f, "%.1f%%", 1.f);
	ImGui::Element::SliderInt(&weapon->aim_minimum_damage, "Minimum damage", 1, 100, "%.0f%% hp");
	ImGui::Element::SliderFloat(&weapon->aim_spread_hit_chance, "Spread hit chance", 1.f, 100.f, "%.0f%%", 1.f);
	ImGui::Element::PopCondition();
	ImGui::Element::EndChild();

	ImGui::Spacing(2);

	ImGui::Element::BeginChild("Fake lag##Ragebot", ImVec2(ImGui::GetChildWidth(), 100));
	ImGui::Element::PushId("Ragebot.Fake lag");

	ImGui::Element::EndChild();

	ImGui::Element::NextColumn();
	ImGui::Element::BeginChild("Other##Ragebot", ImVec2(ImGui::GetChildWidth(), 200));
	ImGui::Element::PushId("Ragebot.Other");
	ImGui::Element::Checkbox(&cvar.ragebot.aim_auto_fire, "Automatic fire");
	ImGui::Element::Checkbox(&cvar.ragebot.aim_friendly_fire, "Friendly fire");
	ImGui::Element::Checkbox(&cvar.ragebot.aim_automatic_scope, "Automatic scope");
	ImGui::Element::Checkbox(&cvar.ragebot.aim_silent, "Silent");
	ImGui::Element::EndChild();

	ImGui::Spacing(2);

	ImGui::Element::BeginChild("Anti-aimbot##Ragebot", ImVec2(ImGui::GetChildWidth(), 235));
	ImGui::Element::PushId("Ragebot.Anti-aimbot");
	
	ImGui::Element::EndChild();
	ImGui::Element::EndColumn();
	ImGui::Element::End();
}

void CMenuDrawing::DrawLegitBot()
{
	ImGui::Element::Begin(&cvar.legitbot.active, "Active##Legitbot", 0, true);

	if (cvar.legitbot.active)
		cvar.ragebot.active = false;

	ImGui::SameLine(422);
	ImGui::PushItemWidth(150);
	ImGui::Text("Group");
	ImGui::SameLine();
	ImGui::Combo("##LegitBot.Group", &current_weapon_group, IM_ARRAYCOMBO(weapon_groups));
	ImGui::SameLine();
	ImGui::Text("\tWeapon");
	ImGui::SameLine();

	switch (current_weapon_group)
	{
	case WEAPONGROUP_PISTOL:
		ImGui::Combo("##RageBot.Weapon", &current_weapon[current_weapon_group], IM_ARRAYCOMBO(weapons_pistol));
		switch (current_weapon[current_weapon_group])
		{
		case 0: m_CurrentWeaponId = WEAPON_DEAGLE;        break;
		case 1: m_CurrentWeaponId = WEAPON_ELITE;         break;
		case 2: m_CurrentWeaponId = WEAPON_FIVESEVEN;     break;
		case 3: m_CurrentWeaponId = WEAPON_GLOCK;         break;
		case 4: m_CurrentWeaponId = WEAPON_USP_SILENCER;  break;
		case 5: m_CurrentWeaponId = WEAPON_CZ75A;         break;
		case 6: m_CurrentWeaponId = WEAPON_TEC9;          break;
		case 7: m_CurrentWeaponId = WEAPON_HKP2000;       break;
		case 8: m_CurrentWeaponId = WEAPON_P250;          break;
		case 9: m_CurrentWeaponId = WEAPON_REVOLVER;
		}
		break;
	case WEAPONGROUP_SUBMACHINEGUN:
		ImGui::Combo("##RageBot.Weapon", &current_weapon[current_weapon_group], IM_ARRAYCOMBO(weapons_submachinegun));
		switch (current_weapon[current_weapon_group])
		{
		case 0: m_CurrentWeaponId = WEAPON_MAC10;         break;
		case 1: m_CurrentWeaponId = WEAPON_P90;           break;
		case 2: m_CurrentWeaponId = WEAPON_MP5;           break;
		case 3: m_CurrentWeaponId = WEAPON_UMP45;         break;
		case 4: m_CurrentWeaponId = WEAPON_BIZON;         break;
		case 5: m_CurrentWeaponId = WEAPON_MP7;           break;
		case 6: m_CurrentWeaponId = WEAPON_MP9;
		}
		break;
	case WEAPONGROUP_RIFLE:
		ImGui::Combo("##RageBot.Weapon", &current_weapon[current_weapon_group], IM_ARRAYCOMBO(weapons_rifle));
		switch (current_weapon[current_weapon_group])
		{
		case 0: m_CurrentWeaponId = WEAPON_AK47;          break;
		case 1: m_CurrentWeaponId = WEAPON_AUG;           break;
		case 2: m_CurrentWeaponId = WEAPON_FAMAS;         break;
		case 3: m_CurrentWeaponId = WEAPON_M4A1;          break;
		case 4: m_CurrentWeaponId = WEAPON_SG556;         break;
		case 5: m_CurrentWeaponId = WEAPON_GALILAR;       break;
		case 6: m_CurrentWeaponId = WEAPON_M4A1_SILENCER;
		}
		break;
	case WEAPONGROUP_SHOTGUN:
		ImGui::Combo("##RageBot.Weapon", &current_weapon[current_weapon_group], IM_ARRAYCOMBO(weapons_shotgun));
		switch (current_weapon[current_weapon_group])
		{
		case 0: m_CurrentWeaponId = WEAPON_MAG7;          break;
		case 1: m_CurrentWeaponId = WEAPON_SAWEDOFF;      break;
		case 2: m_CurrentWeaponId = WEAPON_XM1014;        break;
		case 3: m_CurrentWeaponId = WEAPON_NOVA;
		}
		break;
	case WEAPONGROUP_SNIPER_RIFLE:
		ImGui::Combo("##RageBot.Weapon", &current_weapon[current_weapon_group], IM_ARRAYCOMBO(weapons_sniper));
		switch (current_weapon[current_weapon_group])
		{
		case 0: m_CurrentWeaponId = WEAPON_AWP;           break;
		case 1: m_CurrentWeaponId = WEAPON_G3SG1;         break;
		case 2: m_CurrentWeaponId = WEAPON_SCAR20;        break;
		case 3: m_CurrentWeaponId = WEAPON_SSG08;
		}
		break;
	case WEAPONGROUP_MACHINEGUN:
		ImGui::Combo("##RageBot.Weapon", &current_weapon[current_weapon_group], IM_ARRAYCOMBO(weapons_machinegun));
		switch (current_weapon[current_weapon_group])
		{
		case 0: m_CurrentWeaponId = WEAPON_M249;          break;
		case 1: m_CurrentWeaponId = WEAPON_NEGEV;
		}
	}

	auto* weapon = &cvar.legitbot.weapon[m_CurrentWeaponId];

	ImGui::Element::Begin();
	ImGui::Element::BeginColumn();
	ImGui::Element::BeginChild("Aimbot##Legitbot", ImVec2(ImGui::GetChildWidth(), 447));
	ImGui::Element::PushId("Legitbot.Aimbot");
	ImGui::Element::Checkbox(&weapon->aim, "Enabled", "Global on/off aimbot on weapon");
	ImGui::Element::PushCondition(weapon->aim);
	ImGui::Element::Checkbox(&weapon->aim_head, "Head", "");
	ImGui::Element::Checkbox(&weapon->aim_chest, "Chest", "");
	ImGui::Element::Checkbox(&weapon->aim_stomach, "Stomach", "");
	ImGui::Element::Checkbox(&weapon->aim_arms, "Arms", "");
	ImGui::Element::Checkbox(&weapon->aim_legs, "Legs", "");
	ImGui::Element::SliderFloat(&weapon->aim_maximum_lock_on_time, "Maximum lock-on time", 0.f, 10000.f,
		weapon->aim_maximum_lock_on_time ? "%.0f ms" : "infinity", 1.f, "The aimbot time when the target is in the field of view of aimbot.");
	ImGui::Element::SliderInt(&weapon->aim_reaction_time, "Reaction time", 0, 1000, "%.0f ms", "Delay before aiming to the next player after killing the previous one.");
	ImGui::Element::SliderFloat(&weapon->aim_fov, "Maximum FOV", 0.f, 180.f, u8"%.1f°", 2.f, "Maximal field of view of the aimbot.");
	ImGui::Element::SliderFloat(&weapon->aim_smooth_auto, "Smooth (auto aim)", 0.f, 300.f, "%.2f", 2.f, "Smoothing auto aiming aimbot.");
	ImGui::Element::SliderFloat(&weapon->aim_smooth_scale_fov, "Smooth scale - FOV", 0.f, 100.f, "%.0f%%", 1.f); // ÷òî åòî òàêîå
	ImGui::Element::SliderInt(&weapon->aim_recoil_pitch, "Recoil compensation pitch", 0, 100, "%.0f%%", "The percentage of efficiency of recoil compensation (pitch angle).");
	ImGui::Element::SliderInt(&weapon->aim_recoil_yaw, "Recoil compensation yaw", 0, 100, "%.0f%%", "The percentage of efficiency of recoil compensation (yaw angle).");
	ImGui::Element::SliderFloat(&weapon->aim_recoil_smooth, "Recoil compensation smooth", 0.f, 300.f, "%.2f", 2.f);
	ImGui::Element::SliderFloat(&weapon->aim_recoil_fov, "Recoil compensation maximum FOV", 0.f, 180.f, u8"%.1f°", 2.f, "Maximal field of view of the aimbot when working recoil compensation.");
	ImGui::Element::SliderInt(&weapon->aim_recoil_start, "Recoil compensation start", 0, 15, weapon->aim_recoil_start ? "%.0f bullet" : "if punchangle2D > 0");
	ImGui::Element::SliderFloat(&weapon->aim_delay_before_firing, "Delay before firing", 0.f, 1000.f, "%.0f ms", 2.f, "Delay before firing.");
	ImGui::Element::SliderFloat(&weapon->aim_delay_before_aiming, "Delay before aiming", 0.f, 1000.f, "%.0f ms", 2.f, "Delay before aiming.");
	ImGui::Element::Checkbox(&weapon->aim_automatic_penetration, "Automatic penetration", "");
	ImGui::Element::SliderInt(&weapon->aim_automatic_penetration_minimum_damage, "Automatic penetration min. damage", 1, 100, "%.0f%% hp");
	ImGui::Element::Combo(&weapon->aim_accuracy, "Accuracy boost", IM_ARRAYCOMBO(aim_accuracy_boost));
	ImGui::Element::SliderFloat(&weapon->aim_spread_hit_chance, "Spread hit chance", 1.f, 100.f, "%.0f%%", 1.f);
	ImGui::Element::PopCondition();
	ImGui::Element::EndChild();
	ImGui::Element::NextColumn();
	ImGui::Element::BeginChild("Triggerbot##Legitbot", ImVec2(ImGui::GetChildWidth(), 235));
	ImGui::Element::PushId("Legitbot.Triggerbot");
	ImGui::Element::Checkbox(&weapon->trigger, "Enabled", "Global on/off triggerbot on weapon");
	ImGui::Element::PushCondition(weapon->trigger);
	ImGui::Element::Checkbox(&weapon->trigger_head, "Head", "");
	ImGui::Element::Checkbox(&weapon->trigger_chest, "Chest", "");
	ImGui::Element::Checkbox(&weapon->trigger_stomach, "Stomach", "");
	ImGui::Element::Checkbox(&weapon->trigger_arms, "Arms", "");
	ImGui::Element::Checkbox(&weapon->trigger_legs, "Legs", "");
	ImGui::Element::Checkbox(&weapon->trigger_automatic_penetration, "Automatic penetration", "");
	ImGui::Element::SliderInt(&weapon->trigger_automatic_penetration_minimum_damage, "Automatic penetration min. damage", 1, 100, "%.0f%% hp");
	ImGui::Element::Combo(&weapon->trigger_accuracy, "Accuracy boost", IM_ARRAYCOMBO(trigger_accuracy_boost));
	ImGui::Element::SliderFloat(&weapon->trigger_spread_hit_chance, "Spread hit chance", 1.f, 100.f, "%.0f%%", 1.f);
	ImGui::Element::PopCondition();
	ImGui::Element::EndChild();
	ImGui::Spacing(2);
	ImGui::Element::BeginChild("Other##Legitbot", ImVec2(ImGui::GetChildWidth(), 200));
	ImGui::Element::PushId("Legitbot.Other");
	ImGui::Element::Checkbox(&cvar.legitbot.friendly_fire, "Friendly fire");
	ImGui::Element::Checkbox(&cvar.legitbot.aim_automatic_scope, "Legitbot automatic scope", "Automatic opening sniper scope.");
	ImGui::Element::Checkbox(&cvar.legitbot.aim_dependence_tickrate, "Legitbot dependence tickrate", "");
	ImGui::Element::Checkbox(&cvar.legitbot.aim_accuracy_all_hitboxes, "Legitbot accuracy all hitboxes");
	ImGui::Element::Checkbox(&cvar.legitbot.smoke_check, "Smoke check", "");
	ImGui::Element::Checkbox(&cvar.legitbot.trigger_only_scoped, "Triggerbot only scoped", "");
	ImGui::Element::SliderFloat(&cvar.legitbot.trigger_hitbox_scale, "Triggerbot hitbox scale", 1.0F, 100.0F, "%.f%%", 1.0F); // ÷òî åòî òàêîå
	ImGui::Element::Checkbox(&cvar.legitbot.position_adjustment, "Position adjustment", "");
	ImGui::Element::EndChild();
	ImGui::Element::EndColumn();
	ImGui::Element::End();
}

void CMenuDrawing::DrawVisuals()
{
	ImGui::Element::Begin(&cvar.visuals.active, "Active##Visuals");
	ImGui::Element::BeginColumn();
	ImGui::Element::BeginChild("Player##Visuals", ImVec2(ImGui::GetChildWidth(), 258));
	ImGui::Element::PushId("Visuals.Player");
	ImGui::Element::Checkbox(&cvar.visuals.esp_player_enabled, "Enabled", "Global player ESP on/off");
	ImGui::Element::PushCondition(cvar.visuals.esp_player_enabled);
	ImGui::Element::Checkbox(&cvar.visuals.esp_player_teammates, "Teammates", "Display esp on teammates.");
	ImGui::Element::Checkbox(&cvar.visuals.esp_player_visible_check, "Visible check", "Check player visibility.");
	ImGui::Element::Combo(&cvar.visuals.esp_player_box, "Bounding box", IM_ARRAYCOMBO(esp_player_box), "Draw boxes on players.");
	ImGui::Element::PushCondition(cvar.visuals.esp_player_box);
	ImGui::Element::Text("Bounding box colors hide");
	ImGui::Element::ColorButton4(cvar.visuals.esp_player_box_color_hide_t, "##TerroristColorHide"); ImGui::SameLine();
	ImGui::Element::Text("T");
	ImGui::Element::ColorButton4(cvar.visuals.esp_player_box_color_hide_ct, "##CounterTerroristColorHide", 0.f); ImGui::SameLine();
	ImGui::Element::Text("CT");
	ImGui::Element::Text("Bounding box colors visible");
	ImGui::Element::ColorButton4(cvar.visuals.esp_player_box_color_visible_t, "##TerroristColorVisible"); ImGui::SameLine();
	ImGui::Element::Text("T");
	ImGui::Element::ColorButton4(cvar.visuals.esp_player_box_color_visible_ct, "##CounterTerroristColorVisible", 0.f); ImGui::SameLine();
	ImGui::Element::Text("CT");
	ImGui::Element::Checkbox(&cvar.visuals.esp_player_box_outline, "Bounding box outline", "Draw the outline on the boxes.");
	ImGui::Element::PopCondition();
	ImGui::Element::Combo(&cvar.visuals.esp_player_name, "Name", IM_ARRAYCOMBO(esp_position), "Display player nicknames.");
	ImGui::Element::Combo(&cvar.visuals.esp_player_weapon, "Weapon", IM_ARRAYCOMBO(esp_position), "Display player weapons.");
	ImGui::Element::PushCondition(cvar.visuals.esp_player_weapon);
	ImGui::Element::Checkbox(&cvar.visuals.esp_player_weapon_ammo, "Weapon ammo", "Display player weapon ammo");
	ImGui::Element::PopCondition();
	ImGui::Element::Combo(&cvar.visuals.esp_player_distance, "Distance", IM_ARRAYCOMBO(esp_position), "Show distance to players.");
	ImGui::Element::PushCondition(cvar.visuals.esp_player_distance);
	ImGui::Element::Combo(&cvar.visuals.esp_player_distance_measure, "Distance measurement", IM_ARRAYCOMBO(esp_player_distance), "The measurement of the distance to the players.");
	ImGui::Element::PopCondition();
	ImGui::Element::Combo(&cvar.visuals.esp_player_health, "Health bar", IM_ARRAYCOMBO(esp_position), "Draw a player health bar.");
	ImGui::Element::Combo(&cvar.visuals.esp_player_armor, "Armor bar", IM_ARRAYCOMBO(esp_position), "Draw a player armor bar.");
	ImGui::Element::Checkbox(&cvar.visuals.esp_player_skeleton, "Skeleton", "Draw the skeleton on the players.");
	ImGui::Element::PushCondition(cvar.visuals.esp_player_skeleton);
	ImGui::Element::ColorButton4(cvar.visuals.esp_player_skeleton_color, "##SkeletonColor", 0.f);
	ImGui::Element::PopCondition();
	ImGui::Element::Combo(&cvar.visuals.esp_player_out_of_pov, "Out of POV", IM_ARRAYCOMBO(esp_out_of_pow), "Draw location positions for players out of sight.");
	ImGui::Element::PushCondition(cvar.visuals.esp_player_out_of_pov);
	ImGui::Element::SliderFloat(&cvar.visuals.esp_player_out_of_pov_radius, "Out of POV radius", 0.1f, 1.f, "%.2f", 1.f);
	ImGui::Element::PopCondition();
	ImGui::Element::SliderFloat(&cvar.visuals.esp_player_history_time, "History time", 0.f, 2.5f, "%.1f sec", 1.f, "Time to display visuals after a player's state is false.");
	ImGui::Element::Checkbox(&cvar.visuals.esp_player_fadeout_dormant, "Fade out dormant", "Smooth fading of visuals after a player’s state is false.");
	ImGui::Element::SliderFloat(&cvar.visuals.esp_font_size, "Font size", 0.1f, 18.f, "%.1f px", 1.f);
	ImGui::Element::Combo(&cvar.visuals.esp_font_outline, "Font outline", IM_ARRAYCOMBO(font_outline), "Draw the outline on the font.");
	ImGui::Element::Text("Font color");
	ImGui::Element::ColorButton4(cvar.visuals.esp_font_color, "##FontColor");
	ImGui::Element::PopCondition();
	ImGui::Element::EndChild();
	ImGui::Spacing(2);
	ImGui::Element::BeginChild("Colored models##Visuals", ImVec2(ImGui::GetChildWidth(), 177));
	ImGui::Element::PushId("Visuals.Colored models");
	ImGui::Element::EndChild();
	ImGui::Element::NextColumn();
	ImGui::Element::BeginChild("Other##Visuals", ImVec2(ImGui::GetChildWidth(), 217));
	ImGui::Element::SliderInt(&cvar.visuals.max_render_calls, "Max render calls per second", 1, 300, "%.0f"); // desc
	ImGui::Element::EndChild();
	ImGui::Spacing(2);
	ImGui::Element::BeginChild("Effects##Visuals", ImVec2(ImGui::GetChildWidth(), 218));
	ImGui::Element::PushId("Visuals.Effects");
	ImGui::Element::EndChild();
	ImGui::Element::EndColumn();
	ImGui::Element::End();
}

void CMenuDrawing::DrawKreedz()
{
	ImGui::Element::Begin(&cvar.kreedz.active, "Active##kreedz");
	ImGui::Element::End();
}

void CMenuDrawing::DrawMisc()
{
	ImGui::Element::Begin();
	ImGui::Element::BeginColumn();
	ImGui::Element::BeginChild("Miscellaneous##Miscellaneous", ImVec2(ImGui::GetChildWidth(), 221));
	ImGui::Element::PushId("Miscellaneous.Miscellaneous");
	ImGui::Element::EndChild();
	ImGui::Spacing(2);
	ImGui::Element::BeginChild("Knifebot##Miscellaneous", ImVec2(ImGui::GetChildWidth(), 240));
	ImGui::Element::PushId("Miscellaneous.Knifebot");
	ImGui::Element::EndChild();
	ImGui::Element::NextColumn();
	ImGui::Element::BeginChild("Other##Miscellaneous", ImVec2(ImGui::GetChildWidth(), 473));
	ImGui::Element::PushId("Miscellaneous.Other");

	if (ImGui::Button("Unload cheat Dll", ImVec2(ImGui::GetWindowWidth() - ImGui::GetPadding() * 2.f, 22)))
		UnloadCheatDLL();

	ImGui::Element::EndChild();
	ImGui::Element::EndColumn();
	ImGui::Element::End();
}

void CMenuDrawing::DrawGUI()
{
	ImGui::Element::Begin();
	ImGui::Element::End();
}

void CMenuDrawing::DrawSettings()
{
	static const char* FilesList[] = { "ragebot.ini", "legitbot.ini", "visuals.ini", "kreedz.ini", "misc.ini", "gui.ini" };
	static const float flPaddingOffset = ImGui::GetPadding();

	const auto ColorWindowBg = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);

	auto& sSettingsList = m_SettingsList.GetList();
	auto& iSelectedSettings = m_SettingsList.GetSelectedSettings();
	auto& iSelectedFile = m_SettingsList.GetSelectedFile();

	m_SettingsList.Refresh();

	ImGui::Spacing(4);
	ImGui::SameLine(flPaddingOffset);

	ImGui::PushStyleColor(ImGuiCol_ChildBg, ColorWindowBg);
	ImGui::BeginChild("##SettingsElements", ImVec2(440, 460));
	ImGui::PopStyleColor();

	ImGui::BeginGroup();
	ImGui::Text("Settings:");
	ImGui::PushItemWidth(216);
	ImGui::Element::ListBoxArray("##SettingsList", &iSelectedSettings, sSettingsList, 13);
	ImGui::PopItemWidth();
	ImGui::EndGroup();

	ImGui::SameLine();

	ImGui::BeginGroup();
	ImGui::Text("Files:");
	ImGui::PushItemWidth(216);
	ImGui::ListBox("##FilesList", &iSelectedFile, IM_ARRAYCOMBO(FilesList));
	ImGui::PopItemWidth();
	ImGui::Spacing();
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4());
	const bool bButtonLoad = ImGui::Button("Load##Settings", ImVec2(104, 22)) || (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(76) & 0x1); // CTRL + L
	ImGui::SameLine();
	const bool bButtonSave = ImGui::Button("Save##Settings", ImVec2(104, 22)) || (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(83) & 0x1); // CTRL + S
	ImGui::Spacing();
	const bool bButtonRestore = ImGui::Button("Restore##Settings", ImVec2(104, 22));
	ImGui::SameLine();
	const bool bButtonSaveAs = ImGui::Button("Save as##Settings", ImVec2(104, 22));
	ImGui::Spacing();
	const bool bButtonRemove = ImGui::Button("Remove##Settings", ImVec2(104, 22)) || GetAsyncKeyState(VK_DELETE) & 0x1; // DELETE
	ImGui::SameLine();
	const bool bButtonRename = ImGui::Button("Rename##Settings", ImVec2(104, 22)) || (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(82) & 0x1); // CTRL + R
	ImGui::Spacing();
	const bool bButtonOpenFolder = ImGui::Button("Open folder##Settings", ImVec2(104, 21));
	ImGui::SameLine();
	const bool bButtonOpenFile = ImGui::Button("Open file##Settings", ImVec2(104, 21));
	ImGui::PopStyleColor();
	ImGui::EndGroup();

	if (bButtonLoad)
		g_Settings.Load(m_SettingsList.GetFocusSettings().c_str());

	if (bButtonSave)
		g_Settings.Save(m_SettingsList.GetFocusSettings().c_str());

	if (bButtonRestore)
		ImGui::OpenPopup("Restore##Modal");

	if (bButtonSaveAs)
		ImGui::OpenPopup("Save as##Modal");

	if (bButtonRemove && m_SettingsList.GetFocusSettings().compare("Default"))
		ImGui::OpenPopup("Remove##Modal");

	if (bButtonRename && m_SettingsList.GetFocusSettings().compare("Default"))
	{
		strcpy(g_szSettingsBuffer, m_SettingsList.GetFocusSettings().c_str());
		ImGui::OpenPopup("Rename##Modal");
	}

	if (bButtonOpenFolder)
	{
		std::string sPath = g_Globals.m_sSettingsPath;
		sPath.append(m_SettingsList.GetFocusSettings());
		ShellExecuteA(0, "open", sPath.c_str(), 0, 0, SW_RESTORE);
	}

	if (bButtonOpenFile)
	{
		std::string sPath = g_Globals.m_sSettingsPath;
		sPath.append(m_SettingsList.GetFocusSettings());
		sPath.append("\\");
		sPath.append(FilesList[iSelectedFile]);
		ShellExecuteA(0, "open", sPath.c_str(), 0, 0, SW_RESTORE);
	}

	ImGui::Spacing(2);
	ImGui::Element::BeginChild("##SettingsAddons", ImVec2(0, 198));
	ImGui::Element::EndChild();

	if (ImGui::Element::BeginModal("Save as##Modal", ImVec2(250, 78)))
		ModalSettingsSaveAs();

	if (ImGui::Element::BeginModal("Rename##Modal", ImVec2(250, 78)))
		ModalSettingsRename();

	if (ImGui::Element::BeginModal("Remove##Modal", ImVec2(250, 62)))
		ModalSettingsRemove();

	if (ImGui::Element::BeginModal("Restore##Modal", ImVec2(250, 62)))
		ModalSettingsRestore();

	ImGui::EndChild();

	static char szFileContents[104448];
	RtlSecureZeroMemory(szFileContents, sizeof(szFileContents));

	std::string sPath = g_Globals.m_sSettingsPath;
	sPath.append(m_SettingsList.GetFocusSettings());
	sPath.append("\\");
	sPath.append(FilesList[iSelectedFile]);

	if (FileExists(sPath.c_str()))
		ReadFile(sPath.c_str(), szFileContents);

	ImGui::SameLine();

	ImGui::BeginGroup();
	ImGui::Spacing(4);
	ImGui::PushFont(g_FontProggyClean);
	ImGui::InputTextMultiline("##FileContent", szFileContents, sizeof(szFileContents), ImVec2(378, 439), ImGuiInputTextFlags_AllowTabInput);
	ImGui::PopFont();
	ImGui::EndGroup();

	ImGui::Spacing();
	ImGui::SameLine(ImGui::GetPadding());

	ImGui::PushItemWidth(714);
	ImGui::InputText("##SettingsPath", const_cast<char*>(g_Globals.m_sSettingsPath.c_str()),
		g_Globals.m_sSettingsPath.size() + 1, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll);
	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4());

	if (ImGui::Button("Open path##Settings", ImVec2(104, 15)))
		ShellExecuteA(0, "open", g_Globals.m_sSettingsPath.c_str(), 0, 0, SW_RESTORE);

	ImGui::PopStyleColor();
}

void CMenuDrawing::ModalSettingsSaveAs()
{
	ImGui::Element::BeginModal();

	ImGui::Text("Settings name:");
	ImGui::PushItemWidth(ImGui::GetWindowWidth() - ImGui::GetPadding() * 2.f);
	ImGui::InputText("##SettingsName", g_szSettingsBuffer, sizeof(g_szSettingsBuffer));
	ImGui::PopItemWidth();

	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4());
	ImGui::Spacing();
	const bool bButtonSave = ImGui::Button("Save", ImVec2(109, 20)) || GetAsyncKeyState(VK_RETURN) & 0x1;
	ImGui::SameLine();
	const bool bButtonCancel = ImGui::Button("Cancel", ImVec2(109, 20)) || GetAsyncKeyState(VK_ESCAPE) & 0x1;
	ImGui::PopStyleColor();

	if (bButtonSave && strlen(g_szSettingsBuffer))
	{
		std::string sPath = g_Globals.m_sSettingsPath;
		sPath.append(g_szSettingsBuffer);

		CreateDirectoryA(sPath.c_str(), 0);

		m_SettingsList.Refresh();
		m_SettingsList.SetFocusSettings(g_szSettingsBuffer);
		g_Settings.Save(m_SettingsList.GetFocusSettings().c_str());

		RtlSecureZeroMemory(g_szSettingsBuffer, sizeof(g_szSettingsBuffer));
		ImGui::CloseCurrentPopup();
	}

	if (bButtonCancel)
	{
		RtlSecureZeroMemory(g_szSettingsBuffer, sizeof(g_szSettingsBuffer));
		ImGui::CloseCurrentPopup();
	}

	ImGui::Element::EndModal();
}

void CMenuDrawing::ModalSettingsRename()
{
	ImGui::Element::BeginModal();

	ImGui::Text("Settings name:");
	ImGui::PushItemWidth(ImGui::GetWindowWidth() - ImGui::GetPadding() * 2.f);
	ImGui::InputText("##SettingsName", g_szSettingsBuffer, sizeof(g_szSettingsBuffer));
	ImGui::PopItemWidth();

	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4());
	ImGui::Spacing();
	const bool bButtonRename = ImGui::Button("Rename", ImVec2(109, 20)) || GetAsyncKeyState(VK_RETURN) & 0x1;
	ImGui::SameLine();
	const bool bButtonCancel = ImGui::Button("Cancel", ImVec2(109, 20)) || GetAsyncKeyState(VK_ESCAPE) & 0x1;
	ImGui::PopStyleColor();

	if (bButtonRename && strlen(g_szSettingsBuffer))
	{
		if (m_SettingsList.GetFocusSettings().compare(g_szSettingsBuffer))
		{
			std::string sExistingFileName = g_Globals.m_sSettingsPath;
			sExistingFileName.append(m_SettingsList.GetFocusSettings());

			std::string sNewFileName = g_Globals.m_sSettingsPath;
			sNewFileName.append(g_szSettingsBuffer);

			MoveFileA(sExistingFileName.c_str(), sNewFileName.c_str());

			m_SettingsList.Refresh();
			m_SettingsList.SetFocusSettings(g_szSettingsBuffer);
		}

		RtlSecureZeroMemory(g_szSettingsBuffer, sizeof(g_szSettingsBuffer));
		ImGui::CloseCurrentPopup();
	}

	if (bButtonCancel)
	{
		RtlSecureZeroMemory(g_szSettingsBuffer, sizeof(g_szSettingsBuffer));
		ImGui::CloseCurrentPopup();
	}

	ImGui::Element::EndModal();
}

void CMenuDrawing::ModalSettingsRemove()
{
	ImGui::Element::BeginModal();

	const char* message = "Are you sure you want to delete?";
	const float item_width = ImGui::GetWindowWidth() - ImGui::GetPadding() * 2.f;

	const ImFont* pFont = ImGui::GetIO().Fonts->Fonts[0];
	const ImVec2  text_size = pFont->CalcTextSizeA(pFont->FontSize, item_width, 0, message);

	ImGui::SameLine((item_width - text_size.x) * 0.5f);
	ImGui::Text(message);

	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4());
	ImGui::Spacing();
	const bool bButtonDelete = ImGui::Button("Delete", ImVec2(109, 20)) || GetAsyncKeyState(VK_RETURN) & 0x1;
	ImGui::SameLine();
	const bool bButtonCancel = ImGui::Button("Cancel", ImVec2(109, 20)) || GetAsyncKeyState(VK_ESCAPE) & 0x1;
	ImGui::PopStyleColor();

	if (bButtonDelete)
	{
		std::string sPath = g_Globals.m_sSettingsPath;
		sPath.append(m_SettingsList.GetFocusSettings());

		g_Settings.Remove(sPath.c_str());
		m_SettingsList.Refresh();

		auto& iSelectedSettings = m_SettingsList.GetSelectedSettings();
		iSelectedSettings = min(static_cast<size_t>(iSelectedSettings), m_SettingsList.GetList().size() - 1);

		ImGui::CloseCurrentPopup();
	}

	if (bButtonCancel)
		ImGui::CloseCurrentPopup();

	ImGui::Element::EndModal();
}

void CMenuDrawing::ModalSettingsRestore()
{
	ImGui::Element::BeginModal();

	const char* message = "Choose the type of restore";
	const float item_width = ImGui::GetWindowWidth() - ImGui::GetPadding() * 2.f;

	const ImFont* pFont = ImGui::GetIO().Fonts->Fonts[0];
	const ImVec2  text_size = pFont->CalcTextSizeA(pFont->FontSize, item_width, 0, message);

	ImGui::SameLine((item_width - text_size.x) * 0.5f);
	ImGui::Text(message);

	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4());
	ImGui::Spacing();
	const bool bButtonSetDefault = ImGui::Button("Set as default", ImVec2(109, 20)) || GetAsyncKeyState(VK_RETURN) & 0x1;
	ImGui::SameLine();
	const bool bButtonSaveSettings = ImGui::Button("Save settings", ImVec2(109, 20));
	ImGui::PopStyleColor();
	const bool bButtonCancel = GetAsyncKeyState(VK_ESCAPE) & 0x1;

	if (bButtonSetDefault)
	{
		g_Settings.Restore(m_SettingsList.GetFocusSettings().c_str(), true);
		ImGui::CloseCurrentPopup();
	}

	if (bButtonSaveSettings)
	{
		g_Settings.Restore(m_SettingsList.GetFocusSettings().c_str());
		ImGui::CloseCurrentPopup();
	}

	if (bButtonCancel)
		ImGui::CloseCurrentPopup();

	ImGui::Element::EndModal();
}

void CMenuDrawing::SetCursor()
{
	static bool bShowCursorState = false;

	if (!ImGui::GetCurrentContext())
		return;

	ImGuiIO& io = ImGui::GetIO();
	io.MouseDrawCursor = IsOpened();

	if (io.MouseDrawCursor)
	{
		g_pIVGuiSurface->UnlockCursor();
		g_pIInputSystem->ResetInputState();

		if (!bShowCursorState)
		{
			ShowCursor(false);
			bShowCursorState = true;
		}
	}
	else
	{
		if (bShowCursorState)
		{
			ShowCursor(true);
			bShowCursorState = false;
		}
	}
}