#include "..\menu_consts.h"
#include "menu_tabs.h"

#include "..\..\..\imgui\imgui_internal.h"
#include "..\..\..\cvars.h"

void CMenuTabs::TabGUI()
{
	static int current_selected_color;
	static int current_selected_theme;

	static bool bIsCopiedState;
	static bool bIsFirstColorPickerRef;

	static float flColorBuffer[4];

	float flChildWidth = GetChildWidth();
	const float flWidthSize = ImGui::GetWindowWidth() * 0.5f - SAMELINE_PADDING_OFFSET * 1.5f;
	const float flButtonWidth = (flChildWidth - 7.f - SAMELINE_PADDING_OFFSET * 2.f) * 0.5f;
	const float flButtonHeight = 22.f;
	const float flPickerSize = flWidthSize + 74.f;

	BeginTab();

	ImGui::BeginGroup();
	ImGui::SetNextItemWidth(flChildWidth);

	if (ImGui::ListBox("##GuiColorList", &current_selected_color, IM_ARRAYCOMBO(gui_color_list), 27) || !bIsFirstColorPickerRef)
	{
		const auto LastItemRect = ImGui::GetCurrentWindow()->DC.LastItemRect;
		GImGui->ColorPickerRef = ImColor(cvar.gui[current_selected_color]);
		bIsFirstColorPickerRef = true;
	}

	ImGui::EndGroup();

	ImGui::SameLine(flChildWidth + SAMELINE_PADDING_OFFSET * 0.5F);

	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
	ImGui::BeginChild("##PickerChild", ImVec2(flPickerSize, flPickerSize));
	ImGui::PopStyleColor();

	ImGui::ColorPicker4("##GuiColorPicker", cvar.gui[current_selected_color], ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_AlphaBar |
		ImGuiColorEditFlags_AlphaPreviewHalf, &GImGui->ColorPickerRef.x);

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	flChildWidth += SAMELINE_PADDING_OFFSET * 0.5F;

	BeginChild("##SettingsAddons", ImVec2(flChildWidth, 126));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4());

	if (ImGui::Button("Copy current color##GuiColor", ImVec2(flButtonWidth, flButtonHeight)))
	{
		for (int i = 0; i < IM_ARRAYSIZE(flColorBuffer); i++)
			flColorBuffer[i] = cvar.gui[current_selected_color][i];

		bIsCopiedState = true;
	}

	ImGui::SameLine();

	if (!bIsCopiedState)
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));

	if (ImGui::Button("Paste saved color##GuiColor", ImVec2(flButtonWidth, flButtonHeight)) && bIsCopiedState)
		for (size_t i = 0; i < ImGuiCol_COUNT; i++)
			cvar.gui[current_selected_color][i] = flColorBuffer[i];

	if (!bIsCopiedState)
		ImGui::PopStyleColor();

	if (ImGui::Button("Set to default theme##GuiColor", ImVec2(flButtonWidth, flButtonHeight)))
		DefaultColorTheme();

	ImGui::SameLine();

	if (ImGui::Button("Restore previous color##GuiColor", ImVec2(flButtonWidth, flButtonHeight)))
		for (size_t i = 0; i < 4; i++)
			cvar.gui[current_selected_color][i] = GImGui->ColorPickerRef[i];

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("Themes:");

	ImGui::SetNextItemWidth(flChildWidth - 63.f - SAMELINE_PADDING_OFFSET * 2.f);
	ImGui::Combo("##ColorThemes", &current_selected_theme, IM_ARRAYCOMBO(color_themes));

	ImGui::SameLine();

	if (ImGui::Button("Apply##ColorThemes", ImVec2(50, 16)))
	{
		switch (current_selected_theme)
		{
		case 0: DefaultColorTheme(); break;
		case 1: SimplifiedWhiteRed(); break;
		case 2: SimplifiedNightRed(); break;
		case 3: SimplifiedWhiteOrange(); break;
		case 4: SimplifiedNightOrange(); break;
		case 5: SimplifiedWhiteCyan(); break;
		case 6: SimplifiedNightCyan();
		}
	}

	ImGui::PopStyleColor();
	EndChild();

	flChildWidth -= SAMELINE_PADDING_OFFSET * 0.5f;

	ImGui::EndChild();
	EndTab();
}