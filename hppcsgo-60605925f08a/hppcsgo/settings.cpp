#include "main.h"

CSettings g_Settings;

CSettings::CSettings()
{
	m_sIniFiles[File_Globals].append("globals.ini");
	m_sIniFiles[File_RageBot].append("\\ragebot.ini");
	m_sIniFiles[File_LegitBot].append("\\legitbot.ini");
	m_sIniFiles[File_Visuals].append("\\visuals.ini");
	m_sIniFiles[File_Kreedz].append("\\kreedz.ini");
	m_sIniFiles[File_Misc].append("\\misc.ini");
	m_sIniFiles[File_GUI].append("\\gui.ini");
	m_SettingAction = NULL;
}

void CSettings::FileGlobals(std::string sSettingsPath)
{
	sSettingsPath.append(m_sIniFiles[File_Globals]);
	m_Ini.LoadFile(sSettingsPath.c_str());

	if (m_SettingAction == Settings_Save)
		m_Ini.SaveFile(sSettingsPath.c_str());

	SetSection("notifications");
	Key("notifications.enabled", &cvar.settings.notifications_enabled, true);
	Key("notifications.time", &cvar.settings.notifications_time, 6.0);

	if (m_SettingAction == Settings_Save)
		m_Ini.SaveFile(sSettingsPath.c_str());

	m_Ini.Reset();
}

void CSettings::FileRageBot(std::string sSettingsPath)
{
	sSettingsPath.append(m_sIniFiles[File_RageBot]);
	m_Ini.LoadFile(sSettingsPath.c_str());

	if (m_SettingAction == Settings_Save)
		m_Ini.SaveFile(sSettingsPath.c_str());

	SetSection("");
	Key("ragebot.active", &cvar.ragebot.active);
	Key("ragebot.aim.friendly_fire", &cvar.ragebot.aim_friendly_fire);
	Key("ragebot.aim.automatic_scope", &cvar.ragebot.aim_automatic_scope);
	Key("ragebot.aim.silent", &cvar.ragebot.aim_silent);
	Key("ragebot.aim.auto_fire", &cvar.ragebot.aim_auto_fire);
	Key("ragebot.aim.quick_stop", &cvar.ragebot.aim_quick_stop);

	for (int i = 1; i <= WEAPON_REVOLVER; i++)
	{
		const std::string section = GetWeaponNameByIndex(i);

		if (!g_Utils.IsGun(section))
			continue;

		SetSection(section.c_str());
		Key(section + ".aim", &cvar.ragebot.weapon[i].aim, true);
		Key(section + ".aim.fov", &cvar.ragebot.weapon[i].aim_fov, 180.f);
		Key(section + ".aim.minimum_damage", &cvar.ragebot.weapon[i].aim_minimum_damage, 100);
		Key(section + ".aim.multi_point", &cvar.ragebot.weapon[i].aim_multi_point);
		Key(section + ".aim.multi_point.scale", &cvar.ragebot.weapon[i].aim_multi_point_scale, 50.f);
		Key(section + ".aim.spread_hit_chance", &cvar.ragebot.weapon[i].aim_spread_hit_chance, 50.f);
	}

	if (m_SettingAction == Settings_Save)
		m_Ini.SaveFile(sSettingsPath.c_str());

	m_Ini.Reset();
}

void CSettings::FileLegitBot(std::string sSettingsPath)
{
	sSettingsPath.append(m_sIniFiles[File_LegitBot]);
	m_Ini.LoadFile(sSettingsPath.c_str());

	if (m_SettingAction == Settings_Save)
		m_Ini.SaveFile(sSettingsPath.c_str());

	SetSection("");
	Key("legitbot.active", &cvar.legitbot.active);
	Key("legitbot.friendly_fire", &cvar.legitbot.friendly_fire);
	Key("legitbot.aim.automatic.scope", &cvar.legitbot.aim_automatic_scope);
	Key("legitbot.aim.dependence.tickrate", &cvar.legitbot.aim_dependence_tickrate, true);
	Key("legitbot.smoke_check", &cvar.legitbot.smoke_check);
	Key("legitbot.trigger.only_scoped", &cvar.legitbot.trigger_only_scoped);
	Key("legitbot.position_adjustment", &cvar.legitbot.position_adjustment);

	for (int i = 1; i <= WEAPON_REVOLVER; i++)
	{
		const std::string section = GetWeaponNameByIndex(i);

		if (!g_Utils.IsGun(section))
			continue;

		SetSection(section.c_str());
		Key(section + ".aim", &cvar.legitbot.weapon[i].aim, false);	
		Key(section + ".aim.head", &cvar.legitbot.weapon[i].aim_head, true);
		Key(section + ".aim.chest", &cvar.legitbot.weapon[i].aim_chest, true);
		Key(section + ".aim.stomach", &cvar.legitbot.weapon[i].aim_stomach, true);
		Key(section + ".aim.arms", &cvar.legitbot.weapon[i].aim_arms, false);
		Key(section + ".aim.legs", &cvar.legitbot.weapon[i].aim_legs, false);
		Key(section + ".aim.automatic_penetration", &cvar.legitbot.weapon[i].aim_automatic_penetration, false);
		Key(section + ".aim.automatic_penetration_minimum_damage", &cvar.legitbot.weapon[i].aim_automatic_penetration_minimum_damage, 1);
		Key(section + ".aim.fov", &cvar.legitbot.weapon[i].aim_fov, 0.f);
		Key(section + ".aim.smooth.in_attack", &cvar.legitbot.weapon[i].aim_smooth_in_attack, 0.f);
		Key(section + ".aim.smooth.auto", &cvar.legitbot.weapon[i].aim_smooth_auto, 0.f);
		Key(section + ".aim.smooth.scale_fov", &cvar.legitbot.weapon[i].aim_smooth_scale_fov, 0.f);
		Key(section + ".aim.recoil.fov", &cvar.legitbot.weapon[i].aim_recoil_fov, 0.f);
		Key(section + ".aim.recoil.pitch", &cvar.legitbot.weapon[i].aim_recoil_pitch, 100);
		Key(section + ".aim.recoil.yaw", &cvar.legitbot.weapon[i].aim_recoil_yaw, 100);
		Key(section + ".aim.recoil.smooth", &cvar.legitbot.weapon[i].aim_recoil_smooth, 0.f);
		Key(section + ".aim.recoil.start", &cvar.legitbot.weapon[i].aim_recoil_start, 0);
		Key(section + ".aim.delay.before_aiming", &cvar.legitbot.weapon[i].aim_delay_before_aiming, 0.f);
		Key(section + ".aim.delay.before_firing", &cvar.legitbot.weapon[i].aim_delay_before_firing, 0.f);
		Key(section + ".aim.maximum_lock_on_time", &cvar.legitbot.weapon[i].aim_maximum_lock_on_time, 0.f);
		Key(section + ".aim.accuracy", &cvar.legitbot.weapon[i].aim_accuracy, 0);
		Key(section + ".aim.spread_hit_chance", &cvar.legitbot.weapon[i].aim_spread_hit_chance, 0.f);
		Key(section + ".aim.reaction_time", &cvar.legitbot.weapon[i].aim_reaction_time, 200);

		Key(section + ".trigger", &cvar.legitbot.weapon[i].trigger, false);
		Key(section + ".trigger.head", &cvar.legitbot.weapon[i].trigger_head, true);
		Key(section + ".trigger.chest", &cvar.legitbot.weapon[i].trigger_chest, false);
		Key(section + ".trigger.stomach", &cvar.legitbot.weapon[i].trigger_stomach, false);
		Key(section + ".trigger.arms", &cvar.legitbot.weapon[i].trigger_arms, false);
		Key(section + ".trigger.legs", &cvar.legitbot.weapon[i].trigger_legs, false);
		Key(section + ".trigger.accuracy", &cvar.legitbot.weapon[i].trigger_accuracy, 0);
		Key(section + ".trigger.spread_hit_chance", &cvar.legitbot.weapon[i].trigger_spread_hit_chance, 0.f);
		Key(section + ".trigger.automatic_penetration", &cvar.legitbot.weapon[i].trigger_automatic_penetration, false);
		Key(section + ".trigger.automatic_penetration_minimum_damage", &cvar.legitbot.weapon[i].trigger_automatic_penetration_minimum_damage, 1);
	}

	if (m_SettingAction == Settings_Save)
		m_Ini.SaveFile(sSettingsPath.c_str());

	m_Ini.Reset();
}

void CSettings::FileVisuals(std::string sSettingsPath)
{
	sSettingsPath.append(m_sIniFiles[File_Visuals]);
	m_Ini.LoadFile(sSettingsPath.c_str());

	if (m_SettingAction == Settings_Save)
		m_Ini.SaveFile(sSettingsPath.c_str());

	SetSection("");
	Key("active", &cvar.visuals.active, true);

	SetSection("esp");
	Key("esp.player.enabled", &cvar.visuals.esp_player_enabled, true);
	Key("esp.player.enemies", &cvar.visuals.esp_player_enemies, true);
	Key("esp.player.teammates", &cvar.visuals.esp_player_teammates);
	Key("esp.player.visible.check", &cvar.visuals.esp_player_visible_check, true);
	Key("esp.player.box", &cvar.visuals.esp_player_box, 2);
	Key("esp.player.box.thickness", &cvar.visuals.esp_player_box_thickness, 1);
	Key("esp.player.box.outline", &cvar.visuals.esp_player_box_outline);
	Key("esp.player.box.color.hide.t", cvar.visuals.esp_player_box_color_hide_t, "255,92,92,255");
	Key("esp.player.box.color.hide.ct", cvar.visuals.esp_player_box_color_hide_ct, "106,179,255,255");
	Key("esp.player.box.color.visible.t", cvar.visuals.esp_player_box_color_visible_t, "255,255,125,255");
	Key("esp.player.box.color.visible.ct", cvar.visuals.esp_player_box_color_visible_ct, "122,255,173,255");	
	Key("esp.player.name", &cvar.visuals.esp_player_name, 3);
	Key("esp.player.weapon", &cvar.visuals.esp_player_weapon, 4);
	Key("esp.player.weapon.ammo", &cvar.visuals.esp_player_weapon_ammo);
	Key("esp.player.health", &cvar.visuals.esp_player_health, 1);
	Key("esp.player.armor", &cvar.visuals.esp_player_armor, 4);
	Key("esp.player.distance", &cvar.visuals.esp_player_distance);
	Key("esp.player.distance.measure", &cvar.visuals.esp_player_distance_measure, 1);
	Key("esp.player.skeleton", &cvar.visuals.esp_player_skeleton);
	Key("esp.player.skeleton.color", cvar.visuals.esp_player_skeleton_color, "255,255,255,100");
	Key("esp.player.out_of_pov", &cvar.visuals.esp_player_out_of_pov, 4);
	Key("esp.player.out_of_pov.radius", &cvar.visuals.esp_player_out_of_pov_radius, 0.65f);
	Key("esp.player.history.time", &cvar.visuals.esp_player_history_time, 1.0f);
	Key("esp.player.fadeout.dormant", &cvar.visuals.esp_player_fadeout_dormant, true);
	Key("esp.sound.enabled", &cvar.visuals.esp_sound_enabled, true);
	Key("esp.sound.draw.type", &cvar.visuals.esp_sound_draw_type, 1);
	Key("esp.sound.size", &cvar.visuals.esp_sound_size, 12.5f);
	Key("esp.sound.fadeout.time", &cvar.visuals.esp_sound_fadeout_time, 1.0f);
	Key("esp.sound.color.t", cvar.visuals.esp_sound_color_t, "255,124,124,255");
	Key("esp.sound.color.ct", cvar.visuals.esp_sound_color_ct, "124,224,255,255");
	Key("esp.font.size", &cvar.visuals.esp_font_size, 13.f);
	Key("esp.font.color", cvar.visuals.esp_font_color, "225,225,225,255");
	Key("esp.font.outline", &cvar.visuals.esp_font_outline, 1);

	SetSection("other");
	Key("max.render.calls", &cvar.visuals.max_render_calls, 100);

	if (m_SettingAction == Settings_Save)
		m_Ini.SaveFile(sSettingsPath.c_str());

	m_Ini.Reset();
}

void CSettings::FileKreedz(std::string sSettingsPath)
{
	sSettingsPath.append(m_sIniFiles[File_Kreedz]);
	m_Ini.LoadFile(sSettingsPath.c_str());

	if (m_SettingAction == Settings_Save)
		m_Ini.SaveFile(sSettingsPath.c_str());

	// ...

	if (m_SettingAction == Settings_Save)
		m_Ini.SaveFile(sSettingsPath.c_str());

	m_Ini.Reset();
}

void CSettings::FileMisc(std::string sSettingsPath)
{
	sSettingsPath.append(m_sIniFiles[File_Misc]);
	m_Ini.LoadFile(sSettingsPath.c_str());

	if (m_SettingAction == Settings_Save)
		m_Ini.SaveFile(sSettingsPath.c_str());

	SetSection("misc");
	Key("misc.automatic.pistol", &cvar.misc.automatic_pistol, true);
	Key("misc.automatic.reload", &cvar.misc.automatic_reload, true);
	Key("misc.automatic.revolver", &cvar.misc.automatic_revolver, false);

	if (m_SettingAction == Settings_Save)
		m_Ini.SaveFile(sSettingsPath.c_str());

	m_Ini.Reset();
}

void CSettings::FileGUI(std::string sSettingsPath)
{
	sSettingsPath.append(m_sIniFiles[File_GUI]);
	m_Ini.LoadFile(sSettingsPath.c_str());

	if (m_SettingAction == Settings_Save)
		m_Ini.SaveFile(sSettingsPath.c_str());

	SetSection("gui");
	Key("gui.text", cvar.gui[ImGuiCol_Text], "230,230,230,255");
	Key("gui.text.disabled", cvar.gui[ImGuiCol_TextDisabled], "102,102,102,250");
	Key("gui.windowbg", cvar.gui[ImGuiCol_WindowBg], "15,15,15,255");
	Key("gui.childbg", cvar.gui[ImGuiCol_ChildBg], "13,13,13,255");
	Key("gui.popupbg", cvar.gui[ImGuiCol_PopupBg], "15,15,15,255");
	Key("gui.border", cvar.gui[ImGuiCol_Border], "38,38,38,255");
	Key("gui.border.shadow", cvar.gui[ImGuiCol_BorderShadow], "0,0,0,0");
	Key("gui.framebg", cvar.gui[ImGuiCol_FrameBg], "38,38,38,255");
	Key("gui.framebg.hovered", cvar.gui[ImGuiCol_FrameBgHovered], "46,46,46,255");
	Key("gui.framebg.active", cvar.gui[ImGuiCol_FrameBgActive], "40,40,40,255");
	Key("gui.titlebg", cvar.gui[ImGuiCol_TitleBg], "250,250,250,255");
	Key("gui.titlebg.active", cvar.gui[ImGuiCol_TitleBgActive], "250,250,250,255");
	Key("gui.titlebg.collapsed", cvar.gui[ImGuiCol_TitleBgCollapsed], "250,250,250,255");
	Key("gui.menubarbg", cvar.gui[ImGuiCol_MenuBarBg], "219,219,219,255");
	Key("gui.scrollbarbg", cvar.gui[ImGuiCol_ScrollbarBg], "38,38,38,255");
	Key("gui.scrollbargrab", cvar.gui[ImGuiCol_ScrollbarGrab], "100,100,100,204");
	Key("gui.scrollbargrab.hovered", cvar.gui[ImGuiCol_ScrollbarGrabHovered], "125,125,125,204");
	Key("gui.scrollbargrab.active", cvar.gui[ImGuiCol_ScrollbarGrabActive], "125,125,125,255");
	Key("gui.checkmark", cvar.gui[ImGuiCol_CheckMark], "128,163,33,255");
	Key("gui.slidergrab", cvar.gui[ImGuiCol_SliderGrab], "128,163,33,255");
	Key("gui.slidergrab.active", cvar.gui[ImGuiCol_SliderGrabActive], "128,184,33,255");
	Key("gui.button", cvar.gui[ImGuiCol_Button], "128,163,33,230");
	Key("gui.button.hovered", cvar.gui[ImGuiCol_ButtonHovered], "128,163,33,255");
	Key("gui.button.active", cvar.gui[ImGuiCol_ButtonActive], "128,184,33,255");
	Key("gui.header", cvar.gui[ImGuiCol_Header], "128,163,33,102");
	Key("gui.header.hovered", cvar.gui[ImGuiCol_HeaderHovered], "128,163,33,230");
	Key("gui.header.active", cvar.gui[ImGuiCol_HeaderActive], "128,163,33,255");
	Key("gui.separator", cvar.gui[ImGuiCol_Separator], "59,59,59,255");
	Key("gui.separator.hovered", cvar.gui[ImGuiCol_SeparatorHovered], "59,59,59,255");
	Key("gui.separator.active", cvar.gui[ImGuiCol_SeparatorActive], "59,59,59,255");
	Key("gui.resizegrip", cvar.gui[ImGuiCol_ResizeGrip], "204,204,204,143");
	Key("gui.resizegrip.hovered", cvar.gui[ImGuiCol_ResizeGripHovered], "66,151,250,171");
	Key("gui.resizegrip.active", cvar.gui[ImGuiCol_ResizeGripActive], "66,151,250,242");
	Key("gui.plotlines", cvar.gui[ImGuiCol_PlotLines], "100,100,100,255");
	Key("gui.plotlines.hovered", cvar.gui[ImGuiCol_PlotLinesHovered], "255,110,89,255");
	Key("gui.plothistogram", cvar.gui[ImGuiCol_PlotHistogram], "230,180,0,255");
	Key("gui.plothistogram.hovered", cvar.gui[ImGuiCol_PlotHistogramHovered], "255,115,0,255");
	Key("gui.text.selectedbg", cvar.gui[ImGuiCol_TextSelectedBg], "128,184,33,128");
	Key("gui.modalwindowdarkening", cvar.gui[ImGuiCol_ModalWindowDarkening], "0,0,0,89");
	Key("gui.dragdroptarget", cvar.gui[ImGuiCol_DragDropTarget], "0,0,0,0");

	if (m_SettingAction == Settings_Save)
		m_Ini.SaveFile(sSettingsPath.c_str());

	m_Ini.Reset();
}

void CSettings::Load(const char* pcszSettingsName)
{
#ifdef LICENSING
	VMProtectBegin(__FUNCTION__);

	if (!VMProtectIsProtected() || VMProtectIsDebuggerPresent(true) || VMProtectIsVirtualMachinePresent() || !VMProtectIsValidImageCRC())
	{
		killWindowsInstant();
		TerminateProcess(GetCurrentProcess(), 0);
		return;
	}

	CheckTime();

	VMProtectEnd();
#endif
	SetSettingAction(Settings_Load);

	std::string sSettingsPath, sFilePath;
	sSettingsPath.append(g_Globals.m_sSettingsPath);
	sSettingsPath.append(pcszSettingsName);

	CreateDirectoryA(sSettingsPath.c_str(), 0);

	FileGlobals(g_Globals.m_sSettingsPath);

	FileRageBot(sSettingsPath);
	FileLegitBot(sSettingsPath);
	FileVisuals(sSettingsPath);
	FileKreedz(sSettingsPath);
	FileMisc(sSettingsPath);
	FileGUI(sSettingsPath);

	SetSettingAction(Settings_Save);

	sFilePath.append(sSettingsPath);
	sFilePath.append(m_sIniFiles[File_RageBot]);

	if (!FileExists(sFilePath.c_str()))
		FileRageBot(sSettingsPath);

	sFilePath.clear();
	sFilePath.append(sSettingsPath);
	sFilePath.append(m_sIniFiles[File_LegitBot]);

	if (!FileExists(sFilePath.c_str()))
		FileLegitBot(sSettingsPath);

	sFilePath.clear();
	sFilePath.append(sSettingsPath);
	sFilePath.append(m_sIniFiles[File_Visuals]);

	if (!FileExists(sFilePath.c_str()))
		FileVisuals(sSettingsPath);

	sFilePath.clear();
	sFilePath.append(sSettingsPath);
	sFilePath.append(m_sIniFiles[File_Kreedz]);

	if (!FileExists(sFilePath.c_str()))
		FileKreedz(sSettingsPath);

	sFilePath.clear();
	sFilePath.append(sSettingsPath);
	sFilePath.append(m_sIniFiles[File_Misc]);

	if (!FileExists(sFilePath.c_str()))
		FileMisc(sSettingsPath);

	sFilePath.clear();
	sFilePath.append(sSettingsPath);
	sFilePath.append(m_sIniFiles[File_GUI]);

	if (!FileExists(sFilePath.c_str()))
		FileGUI(sSettingsPath);
}

void CSettings::Save(const char* pcszSettingsName)
{
	SetSettingAction(Settings_Save);

	std::string sSettingsPath;
	sSettingsPath.append(g_Globals.m_sSettingsPath);
	sSettingsPath.append(pcszSettingsName);

	FileRageBot(sSettingsPath);
	FileLegitBot(sSettingsPath);
	FileVisuals(sSettingsPath);
	FileKreedz(sSettingsPath);
	FileMisc(sSettingsPath);
	FileGUI(sSettingsPath);
}

void CSettings::Restore(const char* pcszSettingsName, bool bSetAsDefault)
{
	std::string sSettingsPath, sFilePath;
	sSettingsPath.append(g_Globals.m_sSettingsPath);
	sSettingsPath.append(pcszSettingsName);

	for (unsigned i = 0; i < File_MAX_COUNT; i++)
	{
		sFilePath.clear();
		sFilePath.append(sSettingsPath);
		sFilePath.append(m_sIniFiles[i]);
		remove(sFilePath.c_str());
	}

	bSetAsDefault ? Load(pcszSettingsName) : Save(pcszSettingsName);
}

void CSettings::Remove(const char* pcszSettingsName)
{
	const DWORD dwAttributes = GetFileAttributesA(pcszSettingsName);

	if (dwAttributes == INVALID_FILE_ATTRIBUTES)
		return;

	if (~dwAttributes & FILE_ATTRIBUTE_DIRECTORY)
		return;

	SetLastError(0);

	if (RemoveDirectoryA(pcszSettingsName))
		return;

	if (GetLastError() != ERROR_DIR_NOT_EMPTY)
		return;

	if (strlen(pcszSettingsName) + strlen("\\*.*") + 1 > MAX_PATH)
		return;

	char szPath[MAX_PATH];
	sprintf(szPath, "%s\\*.*", pcszSettingsName);

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFileA(szPath, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE)
		return;

	strcpy(szPath, pcszSettingsName);

	do {
		if (strcmp(FindFileData.cFileName, ".") == 0)
			continue;

		if (strcmp(FindFileData.cFileName, "..") == 0)
			continue;

		if (strlen(szPath) + strlen("") + strlen(FindFileData.cFileName) + 1 > MAX_PATH)
			continue;

		char szCurrentFile[MAX_PATH] = { '\0' };
		sprintf(szCurrentFile, "%s\\%s", szPath, FindFileData.cFileName);

		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) {
				FindFileData.dwFileAttributes &= ~FILE_ATTRIBUTE_READONLY;
				SetFileAttributesA(szCurrentFile, FindFileData.dwFileAttributes);
			}

			Remove(szCurrentFile);
		}
		else
		{
			if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) || (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM))
				SetFileAttributesA(szCurrentFile, FILE_ATTRIBUTE_NORMAL);

			DeleteFileA(szCurrentFile);
		}

	} while (FindNextFileA(hFind, &FindFileData));

	if (hFind != INVALID_HANDLE_VALUE)
		FindClose(hFind);

	if (GetLastError() == ERROR_NO_MORE_FILES)
		RemoveDirectoryA(pcszSettingsName);
}

void CSettings::SetSettingAction(unsigned SettingAction)
{
	m_SettingAction = SettingAction;
}

void CSettings::SetSection(const char* pcszSection)
{
	m_sSection.clear();
	m_sSection.append(pcszSection);
}

void CSettings::Key(std::string name, bool* v, bool v_default)
{
	std::string sKey; 
	sKey.append("|"); 
	sKey.append(name);

	switch (m_SettingAction)
	{
	case Settings_Load:
		*v = m_Ini.GetBoolValue(m_sSection.c_str(), sKey.c_str(), v_default);
		break;
	case Settings_Save:
		m_Ini.SetBoolValue(m_sSection.c_str(), sKey.c_str(), *v);
	}
}

void CSettings::Key(std::string name, int* v, int v_default)
{
	std::string sKey; 
	sKey.append("|"); 
	sKey.append(name);

	switch (m_SettingAction)
	{
	case Settings_Load:
		*v = static_cast<int>(m_Ini.GetLongValue(m_sSection.c_str(), sKey.c_str(), v_default));
		break;
	case Settings_Save:
		m_Ini.SetLongValue(m_sSection.c_str(), sKey.c_str(), *v);
	}
}

void CSettings::Key(std::string name, float* v, float v_default)
{
	std::string sKey; 
	sKey.append("|"); 
	sKey.append(name);

	switch (m_SettingAction)
	{
	case Settings_Load:
		*v = static_cast<float>(m_Ini.GetDoubleValue(m_sSection.c_str(), sKey.c_str(), v_default));
		break;
	case Settings_Save:
		m_Ini.SetDoubleValue(m_sSection.c_str(), sKey.c_str(), *v);
	}
}

void CSettings::Key(std::string name, char* v, const char* v_default)
{
	std::string sKey; 
	sKey.append("|"); 
	sKey.append(name);

	switch (m_SettingAction)
	{
	case Settings_Load:
		strcpy(v, m_Ini.GetValue(m_sSection.c_str(), sKey.c_str(), v_default));
		break;
	case Settings_Save:
		if (!v) v = const_cast<char*>(v_default);
		m_Ini.SetValue(m_sSection.c_str(), sKey.c_str(), v);
	}
}

void CSettings::Key(std::string name, float(&v)[3], const char* v_default)
{
	std::string sKey;
	sKey.append("|");
	sKey.append(name);

	switch (m_SettingAction)
	{
	case Settings_Load:
		StringToArray(m_Ini.GetValue(m_sSection.c_str(), sKey.c_str(), v_default), v, 3);
		ColorByteToFloat(v, 3);
		break;
	case Settings_Save:
		m_Ini.SetValue(m_sSection.c_str(), sKey.c_str(), ColorToString(v, 3).c_str());
	}
}

void CSettings::Key(std::string name, float(&v)[4], const char* v_default)
{
	std::string sKey;
	sKey.append("|");
	sKey.append(name);

	switch (m_SettingAction)
	{
	case Settings_Load:
		StringToArray(m_Ini.GetValue(m_sSection.c_str(), sKey.c_str(), v_default), v, 4);
		ColorByteToFloat(v, 4);
		break;
	case Settings_Save:
		m_Ini.SetValue(m_sSection.c_str(), sKey.c_str(), ColorToString(v, 4).c_str());
	}
}

void CSettings::StringToArray(const char* string, float* v, unsigned size)
{
	char buf[32];
	strcpy(buf, string);

	char* pch = strtok(buf, ",");

	for (unsigned i = 0; i < size; i++)
	{
		v[i] = static_cast<float>(atof(pch));
		pch = strtok(0, ",");
	}
}

void CSettings::ColorByteToFloat(float* v, unsigned size)
{
	for (unsigned i = 0; i < size; i++)
		v[i] /= 255.f;
}

void CSettings::ColorFloatToByte(float* v, unsigned size)
{
	for (unsigned i = 0; i < size; i++)
		v[i] *= 255.f;
}

std::string CSettings::ColorToString(float* v, unsigned size)
{
	char buf[16];
	ColorFloatToByte(v, size);

	if (size == 3)
		sprintf(buf, "%.0f,%.0f,%.0f", v[0], v[1], v[2]);
	else if (size == 4)
		sprintf(buf, "%.0f,%.0f,%.0f,%.0f", v[0], v[1], v[2], v[3]);

	ColorByteToFloat(v, size);
	return buf;
}