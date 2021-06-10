#include "..\menu_consts.h"
#include "menu_drawing.h"

#include <Windows.h>
#include <string>
#include <chrono>
#include <ctime>

#include "..\..\..\imgui\imgui.h"
#include "..\..\..\globals.h"

CMenuDrawing::CMenuDrawing() : m_current_tab_id(RAGEBOT)
{
}

CMenuDrawing::~CMenuDrawing()
{
}

unsigned CMenuDrawing::GetCurrentTabId()
{
	return m_current_tab_id;
}

void CMenuDrawing::DrawWindowTitle()
{
	static const auto* const pFont = ImGui::GetIO().Fonts->Fonts[0];

	static const char* menu_title = "Hpp Hack for Counter-Strike: Global Offensive";
	static const auto window_width = ImGui::GetWindowWidth();
	
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::SameLine(6.f);
	ImGui::Text("Online: %s", g_Globals.m_sOnlineUsers.c_str());

	auto text_size = pFont->CalcTextSizeA(pFont->FontSize, window_width, 0, menu_title);

	ImGui::SameLine((window_width - text_size.x) * 0.5f);
	ImGui::Text(menu_title);

	const auto current_time = std::chrono::system_clock::now();
	const auto current_time_t = std::chrono::system_clock::to_time_t(current_time);
	const char* pcsztime = std::ctime(&current_time_t);

	text_size = pFont->CalcTextSizeA(pFont->FontSize, FLT_MAX, 0, pcsztime);

	ImGui::SameLine(window_width - text_size.x - 6.f);
	ImGui::Text("%s", pcsztime);

	ImGui::Separator();
}

bool CMenuDrawing::DrawFeaturesTabs()
{
	static auto& style = ImGui::GetStyle();

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

	bool IsChanged = false;

	for (unsigned i = 0, size = MAX_COUNT; i < size; i++)
	{
		style.Colors[ImGuiCol_Button] = i != m_current_tab_id ? ColorScrollbarBg : ColorButton;

		static const auto window_width = ImGui::GetWindowWidth();

		if (ImGui::Button(tab_names[i], ImVec2(window_width / size, 18)))
		{
			const auto changed_tad_id = i;

			if (m_current_tab_id != changed_tad_id)
				IsChanged = true;

			m_current_tab_id = changed_tad_id;
		}

		ImGui::SameLine();
	}

	style.Colors[ImGuiCol_Button] = ColorButton;
	style.Colors[ImGuiCol_ButtonActive] = ColorButtonActive;
	style.Colors[ImGuiCol_ButtonHovered] = ColorButtonHovered;
	style.Colors[ImGuiCol_Border] = ColorBorder;
	style.ItemSpacing = ItemSpacing;
	style.FrameRounding = FrameRounding;

	ImGui::Spacing();
	ImGui::Separator();

	return IsChanged;
}