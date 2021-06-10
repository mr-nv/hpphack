#include "..\menu_consts.h"
#include "menu_tabs.h"

#include "..\..\..\weapon_map.h"
#include "..\..\..\main.h"

CMenuTabs::CMenuTabs() :
	m_current_weapon_group(0), m_current_weapon_id(0)
{
	RtlSecureZeroMemory(m_current_weapon, sizeof(m_current_weapon));
}

CMenuTabs::~CMenuTabs()
{
}

int CMenuTabs::WeaponList(const char* str_id)
{
	switch (m_current_weapon_group)
	{
	case WEAPONGROUP_PISTOL:
		ImGui::Combo(str_id, &m_current_weapon[m_current_weapon_group], IM_ARRAYCOMBO(weapons_pistol), INT_MAX);
		return GetWeaponIndex(weapons_pistol[m_current_weapon[m_current_weapon_group]]);
	case WEAPONGROUP_SUBMACHINEGUN:
		ImGui::Combo(str_id, &m_current_weapon[m_current_weapon_group], IM_ARRAYCOMBO(weapons_submachinegun), INT_MAX);
		return GetWeaponIndex(weapons_submachinegun[m_current_weapon[m_current_weapon_group]]);
	case WEAPONGROUP_RIFLE:
		ImGui::Combo(str_id, &m_current_weapon[m_current_weapon_group], IM_ARRAYCOMBO(weapons_rifle), INT_MAX);
		return GetWeaponIndex(weapons_rifle[m_current_weapon[m_current_weapon_group]]);
	case WEAPONGROUP_SHOTGUN:
		ImGui::Combo(str_id, &m_current_weapon[m_current_weapon_group], IM_ARRAYCOMBO(weapons_shotgun), INT_MAX);
		return GetWeaponIndex(weapons_shotgun[m_current_weapon[m_current_weapon_group]]);
	case WEAPONGROUP_SNIPER_RIFLE:
		ImGui::Combo(str_id, &m_current_weapon[m_current_weapon_group], IM_ARRAYCOMBO(weapons_sniper), INT_MAX);
		return GetWeaponIndex(weapons_sniper[m_current_weapon[m_current_weapon_group]]);
	case WEAPONGROUP_MACHINEGUN:
		ImGui::Combo(str_id, &m_current_weapon[m_current_weapon_group], IM_ARRAYCOMBO(weapons_machinegun), INT_MAX);
		return GetWeaponIndex(weapons_machinegun[m_current_weapon[m_current_weapon_group]]);
	}

	return m_current_weapon_id;
}

void CMenuTabs::WeaponCombos()
{
	ImGui::PushItemWidth(140);

	ImGui::SameLine(442);
	ImGui::Text("Group");

	ImGui::SameLine();
	ImGui::Combo("##CurrentWeaponGroup", &m_current_weapon_group, IM_ARRAYCOMBO(weapon_groups));

	ImGui::SameLine();
	ImGui::Text("\tWeapon");

	ImGui::SameLine();
	m_current_weapon_id = WeaponList("##CurrentWeapon");
	ImGui::PopItemWidth();
}

float CMenuTabs::GetChildWidth()
{
	const auto window_width = ImGui::GetWindowWidth();
	return (window_width - SAMELINE_PADDING_OFFSET * 3) * 0.5f;
}

float CMenuTabs::GetButtonWidth()
{
	const auto window_width = ImGui::GetWindowWidth();
	return window_width - SAMELINE_PADDING_OFFSET * 2.f;
}

void CMenuTabs::ResetCurrentWeaponId()
{
	m_current_weapon_id = WEAPON_INVALID;
}

void CMenuTabs::GetCurrentWeaponId()
{
	if (!g_Weapon->m_IsValid)
		return;

	if (!g_Weapon.IsGun())
		return;

	if (m_current_weapon_id != WEAPON_INVALID)
		return;

	m_current_weapon_group = g_Weapon->m_iCurrentType - 1;

	int size = 0;
	const char** weapon_array = 0;

	switch (m_current_weapon_group)
	{
	case WEAPONGROUP_PISTOL:
		size = IM_ARRAYSIZE(weapons_pistol);
		weapon_array = weapons_pistol;
		break;
	case WEAPONGROUP_SUBMACHINEGUN:
		size = IM_ARRAYSIZE(weapons_submachinegun);
		weapon_array = weapons_submachinegun;
		break;
	case WEAPONGROUP_RIFLE:
		size = IM_ARRAYSIZE(weapons_rifle);
		weapon_array = weapons_rifle;
		break;
	case WEAPONGROUP_SHOTGUN:
		size = IM_ARRAYSIZE(weapons_shotgun);
		weapon_array = weapons_shotgun;
		break;
	case WEAPONGROUP_SNIPER_RIFLE:
		size = IM_ARRAYSIZE(weapons_sniper);
		weapon_array = weapons_sniper;
		break;
	case WEAPONGROUP_MACHINEGUN:
		size = IM_ARRAYSIZE(weapons_machinegun);
		weapon_array = weapons_machinegun;
	}

	for (int i = 0; i < size; i++)
	{
		if (!g_Weapon->m_sWeaponName.compare(weapon_array[i]))
		{
			m_current_weapon[m_current_weapon_group] = i;
			break;
		}
	}
}