#include "..\menu_consts.h"
#include "menu_tabs.h"

#include <Windows.h>
#include <string>
#include <io.h>
#include <fstream>

#include "..\..\..\imgui\impl\imgui_impl_dx9.h"
#include "..\..\..\imgui\imgui_internal.h"
#include "..\..\..\cvars.h"
#include "..\..\..\settings.h"
#include "..\..\..\globals.h"
#include "..\..\..\ValveSDK\misc\GlobalVars.hpp"

extern CGlobalVarsBase* g_pGlobalVars;

void CMenuTabs::TabSettings()
{
	static char sz_input_buf[64];
	static char sz_file_contents[104448];

	static float refresh_settings_reachtime;
	static float update_settings_file_reachtime;

	auto& settings_list = GetSettingsList();
	auto& selected_settings = GetSelectedSettings();
	auto& selected_file = GetSelectedFile();

	if (g_pGlobalVars->curtime > refresh_settings_reachtime)
	{
		RefreshSettingsList();

		if (settings_list.empty())
			g_Settings.Restore("Default");

		refresh_settings_reachtime = g_pGlobalVars->curtime + 0.100f;
	}

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::SameLine(SAMELINE_PADDING_OFFSET);

	const auto ColorWindowBg = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);

	ImGui::PushStyleColor(ImGuiCol_ChildBg, ColorWindowBg);
	ImGui::BeginChild("##SettingsElements", ImVec2(440, 455));
	ImGui::PopStyleColor();

	ImGui::BeginGroup();
	ImGui::Text("Settings:");
	ImGui::SetNextItemWidth(216);
	ListBoxArray("##SettingsList", &selected_settings, settings_list, 13);
	ImGui::EndGroup();

	ImGui::SameLine();

	ImGui::BeginGroup();
	ImGui::Text("Files:");
	ImGui::SetNextItemWidth(216);
	ImGui::ListBox("##FilesList", &selected_file, IM_ARRAYCOMBO(settings_files_list));
	ImGui::Spacing();

	static auto button_size_settings = ImVec2(104, 22);

	if (Button("Load##Settings", button_size_settings) || IsHotkey("CTRL + L"))
		g_Settings.Load(GetFocusSettings().c_str());

	if (Button("Save##Settings", button_size_settings) || IsHotkey("CTRL + S"))
		g_Settings.Save(GetFocusSettings().c_str());

	ImGui::Spacing();
	ImGui::Spacing();

	if (Button("Restore##Settings", button_size_settings) || IsHotkey("CTRL + ALT + R"))
		ImGui::OpenPopup("Restore##Modal");

	if (Button("Save as##Settings", button_size_settings) || IsHotkey("CTRL + ALT + S"))
		ImGui::OpenPopup("Save as##Modal");

	ImGui::Spacing();
	ImGui::Spacing();

	if (Button("Remove##Settings", button_size_settings) || IsHotkey("DELETE"))
	{
		const auto& settings_name = GetFocusSettings();

		if (settings_name.compare("Default"))
			ImGui::OpenPopup("Remove##Modal");
	}

	if (Button("Rename##Settings", button_size_settings) || IsHotkey("CTRL + R"))
	{
		const auto& settings_name = GetFocusSettings();

		if (settings_name.compare("Default"))
		{
			settings_name.copy(sz_input_buf, sizeof(sz_input_buf));
			ImGui::OpenPopup("Rename##Modal");
		}
	}

	ImGui::Spacing();
	ImGui::Spacing();

	button_size_settings -= ImVec2(0, 1);

	if (Button("Open folder##Settings", button_size_settings) || IsHotkey("CTRL + ALT + O"))
	{
		const auto path_name = g_Globals.m_sSettingsPath + GetFocusSettings();
		ShellExecuteA(0, "open", path_name.c_str(), 0, 0, SW_RESTORE);
	}

	if (Button("Open file##Settings", button_size_settings) || IsHotkey("CTRL + ALT + F"))
	{
		auto path_name = g_Globals.m_sSettingsPath;
		path_name.append(GetFocusSettings());
		path_name.append("\\");
		path_name.append(settings_files_list[selected_file]);

		ShellExecuteA(0, "open", path_name.c_str(), 0, 0, SW_RESTORE);
	}

	button_size_settings += ImVec2(0, 1);

	ImGui::EndGroup();

	ImGui::Spacing();
	ImGui::Spacing();

	BeginChild("##SettingsAddons", ImVec2(0, 199));
	ImGui::Checkbox("Notifications", &cvar.settings.notifications_enabled);
	ImGui::SliderFloat("Notifications time", &cvar.settings.notifications_time, 5.f, 15.f, "%.2f sec");

	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("Tooltip language:");
	ImGui::RadioButton("English", &cvar.settings.tooltip_language, 0);
	ImGui::RadioButton(u8"Русский", &cvar.settings.tooltip_language, 1);
	EndChild();

	static auto popup_modal_size_with_input = ImVec2(250, 78);
	static auto popup_modal_size_without_input = ImVec2(250, 62);

	if (BeginPopupModal("Save as##Modal", popup_modal_size_with_input))
	{
		DialogSettingsSaveAs(sz_input_buf, sizeof(sz_input_buf));
		EndPopupModal();
	}

	if (BeginPopupModal("Rename##Modal", popup_modal_size_with_input))
	{
		DialogSettingsRename(sz_input_buf, sizeof(sz_input_buf));
		EndPopupModal();
	}

	if (BeginPopupModal("Remove##Modal", popup_modal_size_without_input))
	{
		DialogSettingsRemove();
		EndPopupModal();
	}

	if (BeginPopupModal("Restore##Modal", popup_modal_size_without_input))
	{
		DialogSettingsRestore();
		EndPopupModal();
	}

	ImGui::EndChild();

	if (g_pGlobalVars->curtime > update_settings_file_reachtime)
	{
		auto path_name = g_Globals.m_sSettingsPath;
		path_name.append(GetFocusSettings());
		path_name.append("\\");
		path_name.append(settings_files_list[selected_file]);

		RtlSecureZeroMemory(sz_file_contents, sizeof(sz_file_contents));

		if (_access(path_name.c_str(), 0) != -1)
		{
			std::ifstream input(path_name.c_str());
			std::size_t _ElementSize = 0;

			char ch;
			while ((ch = static_cast<char>(input.get())) != -1)
				_ElementSize++;

			input.close();

			FILE * _Stream = fopen(path_name.c_str(), "r");

			if (_Stream)
			{
				fread(sz_file_contents, _ElementSize, 1, _Stream);
				fclose(_Stream);
			}
		}

		update_settings_file_reachtime = g_pGlobalVars->curtime + 0.100f;
	}

	ImGui::SameLine();
	ImGui::BeginGroup();

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::PushFont(g_FontProggyClean);

	if (ImGui::InputTextMultiline("##FileContent", sz_file_contents, sizeof(sz_file_contents), ImVec2(378, 434), ImGuiInputTextFlags_AllowTabInput))
	{
		auto path_name = g_Globals.m_sSettingsPath;
		path_name.append(GetFocusSettings());
		path_name.append("\\");
		path_name.append(settings_files_list[selected_file]);

		if (_access(path_name.c_str(), 0) != -1)
		{
			std::ofstream ofs;
			ofs.open(path_name.c_str(), std::ofstream::out | std::ofstream::trunc);
			ofs << sz_file_contents;
			ofs.close();
		}
	}

	ImGui::PopFont();

	ImGui::EndGroup();

	ImGui::Spacing();
	ImGui::SameLine(SAMELINE_PADDING_OFFSET);

	ImGui::SetNextItemWidth(714);
	ImGui::InputText("##SettingsPath", g_Globals.m_sSettingsPath.data(), g_Globals.m_sSettingsPath.size(),
		ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll);

	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4());

	if (ImGui::Button("Open path##Settings", ImVec2(104, 14)))
		ShellExecuteA(0, "open", g_Globals.m_sSettingsPath.c_str(), 0, 0, SW_RESTORE);

	ImGui::PopStyleColor();
}