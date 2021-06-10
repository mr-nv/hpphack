#include "..\menu_consts.h"
#include "..\keynum\menu_keynum_map.h"
#include "menu_drawing_elements.h"

#include "..\..\..\imgui\imgui_internal.h"

CMenuDrawingElements::CMenuDrawingElements() : m_str_id("default")
{
}

CMenuDrawingElements::~CMenuDrawingElements()
{
}

void CMenuDrawingElements::BeginTab()
{
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::SameLine(SAMELINE_PADDING_OFFSET);
	ImGui::BeginGroup();
}

void CMenuDrawingElements::BeginTab(bool* v, const char* label, int* key, bool have_on_line)
{
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::SameLine(SAMELINE_PADDING_OFFSET);
	ImGui::Checkbox(label, v);

	if (key)
		Key(key, label, false, true);

	if (!have_on_line)
		BeginTab();
}

void CMenuDrawingElements::EndTab()
{
	ImGui::EndGroup();
}

void CMenuDrawingElements::BeginColumn()
{
	ImGui::BeginGroup();
}

void CMenuDrawingElements::NextColumn()
{
	ImGui::EndGroup();

	const auto window_width = ImGui::GetWindowWidth();

	ImGui::SameLine((window_width - SAMELINE_PADDING_OFFSET) * 0.5f);
	ImGui::BeginGroup();
}

void CMenuDrawingElements::EndColumn()
{
	ImGui::EndGroup();
}

void CMenuDrawingElements::BeginChild(const char* label, ImVec2 size)
{
	const auto ColorSeparator = ImGui::GetStyleColorVec4(ImGuiCol_Separator);

	ImGui::PushStyleColor(ImGuiCol_Border, ColorSeparator);
	ImGui::BeginChildElement(label, size, true);
	ImGui::PopStyleColor();

	ImGui::Spacing();

	ImGui::SameLine(SAMELINE_PADDING_OFFSET);
	ImGui::BeginGroup();

	const auto window_width = ImGui::GetWindowWidth();

	ImGui::PushItemWidth(window_width * 0.5f - SAMELINE_PADDING_OFFSET * 2.f);

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
}

void CMenuDrawingElements::EndChild()
{
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::PopItemWidth();

	ImGui::EndGroup();
	ImGui::EndChild();
}

void CMenuDrawingElements::PushId(const char* str_id)
{
	m_str_id.clear();
	m_str_id.append("##");
	m_str_id.append(str_id == nullptr ? "default" : str_id);
}

void CMenuDrawingElements::PushCondition(bool condition)
{
	m_conditions.push_back(condition);
}

void CMenuDrawingElements::PopCondition(size_t count)
{
	do { m_conditions.pop_back(); } while (--count);
}

void CMenuDrawingElements::Text(const char* fmt, ...)
{
	bool bDisabledText = false;

	for (auto condition : m_conditions)
	{
		if (condition) continue;
		bDisabledText = !condition;
		break;
	}

	if (bDisabledText)
	{
		const auto ColorTextDisabled = ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
		ImGui::PushStyleColor(ImGuiCol_Text, ColorTextDisabled);
	}

	va_list args;
	va_start(args, fmt);
	ImGui::TextV(fmt, args);
	va_end(args);

	if (bDisabledText)
		ImGui::PopStyleColor();
}

void CMenuDrawingElements::TooltipMessage(const char* message)
{
	static const auto* const pFont = ImGui::GetIO().Fonts->Fonts[0];
	constexpr auto PADDING = 8.f;

	const auto text_size = pFont->CalcTextSizeA(pFont->FontSize, FLT_MAX, 0, message);
	const auto window_size = ImVec2(text_size.x + PADDING, text_size.y + PADDING);

	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4());
	ImGui::SetNextWindowSize(window_size);
	ImGui::BeginTooltip();

	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::SameLine(PADDING * 0.5f);
	ImGui::Text(message);

	ImGui::EndTooltip();
	ImGui::PopStyleColor();
}

std::string CMenuDrawingElements::Base(const char* label, const char* message)
{
	Text(label);

	if (message && strlen(message) && ImGui::IsItemHovered())
		TooltipMessage(message);

	const auto window_width = ImGui::GetWindowWidth();
	const auto local_pos_x = window_width * 0.5f;

	ImGui::SameLine(local_pos_x);

	return m_str_id + "." + label;
}

void CMenuDrawingElements::ClampColor(float* col, size_t size)
{
	for (size_t i = 0; i < size; i++)
		col[i] = ImClamp(col[i], 0.f, 1.f);
}

bool CMenuDrawingElements::Checkbox(bool* v, const char* label, const char* msg)
{
	const auto base = Base(label, msg);
	return ImGui::Checkbox(base.c_str(), v);
}

bool CMenuDrawingElements::Combo(int* v, const char* label, const char* items[], int size, const char* msg)
{
	const auto base = Base(label, msg);
	return ImGui::Combo(base.c_str(), v, items, size);
}

bool CMenuDrawingElements::SliderFloat(float* v, const char* label, float min, float max, const char* fmt, float power, const char* msg)
{
	const auto base = Base(label, msg);
	const auto ret = ImGui::SliderFloat(base.c_str(), v, min, max, fmt, power);
	*v = ImClamp(*v, min, max);
	return ret;
}

bool CMenuDrawingElements::SliderInt(int* v, const char* label, int min, int max, const char* fmt, const char* msg)
{
	const auto base = Base(label, msg);
	const auto ret = ImGui::SliderInt(base.c_str(), v, min, max, fmt);
	*v = ImClamp(*v, min, max);
	return ret;
}

static auto vector_getter = [](void* data, int id, const char** out_text)
{
	const auto& vector = *static_cast<std::vector<std::string>*>(data);
	*out_text = vector.at(id).c_str();
	return true;
};

bool CMenuDrawingElements::ListBoxArray(const char* label, int* v, std::vector<std::string> & values, int height_in_items)
{
	return ImGui::ListBox(label, v, vector_getter, &values, values.size(), height_in_items);
}

void CMenuDrawingElements::ColorButton4(float* col, const char* label, float sameline)
{
	ClampColor(col, 4);

	const auto window_width = ImGui::GetWindowWidth();
	ImGui::SameLine(sameline != -1.f ? sameline : window_width * 0.5f);

	if (ImGui::ColorButton(label, ImColor(col), ImGuiColorEditFlags_AlphaPreview, ImVec2(32, 0)))
	{
		const auto LastItemRect = ImGui::GetCurrentWindow()->DC.LastItemRect;

		GImGui->ColorPickerRef = ImColor(col);
		ImGui::OpenPopup(label);

		const auto rect_bottom_left = LastItemRect.GetBL();
		const auto window_pos = ImVec2(rect_bottom_left.x - 1.f, rect_bottom_left.y + GImGui->Style.ItemSpacing.y);

		ImGui::SetNextWindowPos(window_pos);
	}

	if (ImGui::BeginPopup(label))
	{
		static const auto flags = ImGuiColorEditFlags_PickerHueBar | 
			ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf;

		ImGui::ColorPicker4("##picker", col, flags, &GImGui->ColorPickerRef.x);
		ImGui::EndPopup();
	}
}

bool CMenuDrawingElements::Button(const char* label, ImVec2 size)
{
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4());

	const auto ret = ImGui::Button(label, size);

	ImGui::PopStyleColor();
	ImGui::SameLine();

	return ret;
}

void CMenuDrawingElements::Key(int* key, const char* name, bool have_owner, bool global_owner)
{
	static const auto* const pFont = ImGui::GetIO().Fonts->Fonts[0];

	const auto key_name = GetKeyName(*key);
	const auto text_size = pFont->CalcTextSizeA(pFont->FontSize, FLT_MAX, 0, key_name.c_str());
	const auto button_name = key_name + m_str_id + name;

	std::string popup_name = +name;
	popup_name.append("##Key");
	popup_name.append(name);

	const auto window_width = ImGui::GetWindowWidth();
	const auto button_offset = have_owner
		? (window_width * 0.5f - SAMELINE_PADDING_OFFSET) * 2.f - text_size.x - 9.f
		: window_width * 0.5f;

	ImGui::SameLine(global_owner ? 0.f : button_offset);
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4());

	if (ImGui::Button(button_name.c_str()))
	{
		ImGui::OpenPopup(popup_name.c_str());
		KeyAssignmentStart();
	}

	ImGui::PopStyleColor();

	if (BeginPopupModal(popup_name.c_str(), ImVec2(150, 15)))
	{
		PopupModalMessage("Press any key");

		if (!IsKeyAssignment())
		{
			*key = GetAssigmentKeynum();
			ImGui::CloseCurrentPopup();
		}

		EndPopupModal();
	}
}