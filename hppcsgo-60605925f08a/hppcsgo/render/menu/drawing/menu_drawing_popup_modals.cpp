#include "..\menu_consts.h"
#include "menu_drawing_popup_modals.h"

#include <string>

CMenuDrawingPopupModals::CMenuDrawingPopupModals() :
	m_popup_modal_button_size(109, 20)
{
}

CMenuDrawingPopupModals::~CMenuDrawingPopupModals()
{
}

bool CMenuDrawingPopupModals::BeginPopupModal(const char* name, ImVec2 size)
{
	ImGui::SetNextWindowSize(size);

	static const auto flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
	const auto ret = ImGui::BeginPopupModal(name, nullptr, flags);

	if (ret)
	{
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::SameLine(SAMELINE_PADDING_OFFSET);
		ImGui::BeginGroup();
	}

	return ret;
}

void CMenuDrawingPopupModals::PopupModalMessage(const char* message)
{
	static const auto* const pFont = ImGui::GetIO().Fonts->Fonts[0];

	const auto item_width = GetItemWidth();
	const auto text_size = pFont->CalcTextSizeA(pFont->FontSize, item_width, 0, message);
	const auto text_pos_x = (item_width - text_size.x) * 0.5f;

	ImGui::SameLine(text_pos_x);
	ImGui::Text(message);

	ImGui::Spacing();
}

void CMenuDrawingPopupModals::PopupModalInput(const char* name, char* buf, size_t buf_size)
{
	const auto item_width = GetItemWidth();

	ImGui::Text(name);

	std::string str_id("##");
	str_id.append(name);

	ImGui::SetNextItemWidth(item_width);
	ImGui::InputText(str_id.c_str(), buf, buf_size);

	ImGui::Spacing();
}

bool CMenuDrawingPopupModals::PopupModalButton(const char* name)
{
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4());

	const auto ret = ImGui::Button(name, m_popup_modal_button_size);

	ImGui::PopStyleColor();
	ImGui::SameLine();

	return ret;
}

void CMenuDrawingPopupModals::EndPopupModal()
{
	ImGui::EndGroup();
	ImGui::EndPopup();
}

float CMenuDrawingPopupModals::GetItemWidth()
{
	const auto widow_width = ImGui::GetWindowWidth();
	return widow_width - SAMELINE_PADDING_OFFSET * 2.f;
}
