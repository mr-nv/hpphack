#include "main.h"
#include "notifications.h"
#include "render\render.h"

static const auto WINDOW_SIZE = ImVec2(270, 110);
static const auto WINDOW_OFFSET = ImVec2(10, 50);
static const auto WINDOW_PADDING = 12.f;
static const auto DURATION = 0.2f; // 200 msec

CNotification g_Notification;

static const char* gen_random()
{
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	char s[50];
	for (int i = 0; i < 49; ++i)
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	s[49] = 0;

	return s;
}

void CNotification::Push(const char* _Title, const char* _Message)
{
	if (!cvar.settings.notifications_enabled)
		return;

	NotificationData data;
	data.count = m_Data.size();
	data.drawState = DrawState_Appearance;
	data.title = _Title;
	data.message = _Message;
	data.id = gen_random();
	data.time = g_pGlobalVars->curtime;

	m_Data.push_back(data);
}

void CNotification::Delete()
{
	m_Data.pop_front();

	if (m_Data.empty())
		return;

	for (auto& data : m_Data)
		data.count--;

	m_IsDescent = true;
	m_flDescentTime = g_pGlobalVars->curtime;
}

void CNotification::Draw()
{
	if (m_Data.empty())
		return;

	if (!cvar.settings.notifications_enabled)
		m_Data.clear();

	for (auto& data : m_Data)
	{
		switch (data.drawState)
		{
		case DrawState_Appearance:
			if (!IsAppearance(data.time))
				data.drawState = DrawState_Normal;
			break;
		case DrawState_Normal:
			if (!IsNormal(data.time))
				data.drawState = DrawState_Disappearance;
			break;
		case DrawState_Disappearance:
			if (!IsDisappearance(data.time))
				Delete();
		}

		RenderWindow(data);
	}
}

float CNotification::GetAppearanceReachTime(float time)
{
	return time + DURATION;
}

float CNotification::GetNormalReachTime(float time) 
{
	return GetAppearanceReachTime(time) + cvar.settings.notifications_time;
}

float CNotification::GetDisappearanceReachTime(float time)
{
	return GetNormalReachTime(time) + DURATION;
}

bool CNotification::IsAppearance(float time)
{
	return g_pGlobalVars->curtime < GetAppearanceReachTime(time);
}

bool CNotification::IsNormal(float time)
{
	return g_pGlobalVars->curtime < GetNormalReachTime(time);
}

bool CNotification::IsDisappearance(float time)
{
	return g_pGlobalVars->curtime < GetDisappearanceReachTime(time);
}

bool CNotification::IsCursorFocused(float window_pos_x, float window_pos_y)
{
	const auto cursor_pos = ImGui::GetIO().MousePos;
	return cursor_pos.x > window_pos_x && cursor_pos.x < g_Globals.m_flScreenWidth &&
		cursor_pos.y > window_pos_y && cursor_pos.y < window_pos_y + WINDOW_SIZE.y;
}

void CNotification::SwipeProcessing(NotificationData& data, float* window_pos_x)
{
	static float old_pos_x, drag_pos_x;
	static float different_pos_x;

	const auto& io = ImGui::GetIO();

	if (io.MouseDown[0])
	{
		data.drawState = DrawState_Normal;
		different_pos_x = io.MousePos.x - old_pos_x;

		if (old_pos_x - io.MousePos.x < 0.f)
			*window_pos_x = io.MousePos.x - drag_pos_x;
	}
	else
	{
		if (different_pos_x > 5.f)
		{
			data.drawState = DrawState_Disappearance;
			data.time = g_pGlobalVars->curtime - GetNormalReachTime(0.f);
			data.time -= GetInterpolateTime(data.time, DURATION, g_Globals.m_flScreenWidth - (WINDOW_SIZE.x + WINDOW_OFFSET.x), 
				io.MousePos.x - drag_pos_x, g_Globals.m_flScreenWidth) - data.time;
		}

		different_pos_x = 0.f;
		old_pos_x = io.MousePos.x;
		drag_pos_x = io.MousePos.x - *window_pos_x;
	}
}

float CNotification::GetInterpolateTime(float time, float duration, float start, float current, float end)
{
	return ((current - start) + time / duration * (end - start)) / (end - start) * duration;
}

float CNotification::Interpolate(float time, float curtime, float duration, float start, float end)
{
	return start + (curtime - time) / duration * (end - start);
}

void CNotification::GetScreenPosition(const NotificationData& data, float* pos_x, float* pos_y)
{
	static const float flWindowSizeWithOffsetX = WINDOW_SIZE.x + WINDOW_OFFSET.x;
	static const float flWindowSizeWithOffsetY = WINDOW_SIZE.y + WINDOW_OFFSET.y;
	static const float flWindowInterval = WINDOW_SIZE.y + 5.f;

	switch (data.drawState)
	{
	case DrawState_Appearance:
		*pos_x = g_Globals.m_flScreenWidth - Interpolate(data.time, g_pGlobalVars->curtime, DURATION, 0.f, flWindowSizeWithOffsetX);
		*pos_y = g_Globals.m_flScreenHeight - flWindowSizeWithOffsetY - flWindowInterval * static_cast<float>(data.count);
		break;
	case DrawState_Normal:
		*pos_x = g_Globals.m_flScreenWidth - flWindowSizeWithOffsetX;
		*pos_y = g_Globals.m_flScreenHeight - flWindowSizeWithOffsetY - flWindowInterval * static_cast<float>(data.count);
		break;
	case DrawState_Disappearance:
		*pos_x = g_Globals.m_flScreenWidth - Interpolate(GetNormalReachTime(data.time), g_pGlobalVars->curtime, DURATION, flWindowSizeWithOffsetX, 0.f);
		*pos_y = g_Globals.m_flScreenHeight - flWindowSizeWithOffsetY - flWindowInterval * static_cast<float>(data.count);
	}

	if (m_IsDescent)
	{
		if (g_pGlobalVars->curtime < m_flDescentTime + DURATION)
			*pos_y -= Interpolate(m_flDescentTime, g_pGlobalVars->curtime, DURATION, flWindowInterval, 0.f);
		else
			m_IsDescent = false;
	}
}

void CNotification::GetAlpha(const NotificationData& data, float default_alpha, float* out_alpha)
{
	switch (data.drawState)
	{
	case DrawState_Appearance:
		*out_alpha = Interpolate(data.time, g_pGlobalVars->curtime, DURATION, 0.00f, default_alpha);
		break;
	case DrawState_Normal:
		*out_alpha = default_alpha;
		break;
	case DrawState_Disappearance:
		*out_alpha = Interpolate(data.time + DURATION + cvar.settings.notifications_time, g_pGlobalVars->curtime, DURATION, default_alpha, 0.00f);
	}
}

ImVec4 CNotification::GetWindowBgColor(const NotificationData& data)
{
	auto windowbg_color = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
	GetAlpha(data, windowbg_color.w, &windowbg_color.w);
	return windowbg_color;
}

ImVec4 CNotification::GetTextColor(const NotificationData& data)
{
	auto text_color = ImGui::GetStyleColorVec4(ImGuiCol_Text);
	GetAlpha(data, text_color.w, &text_color.w);
	return text_color;
}

ImVec4 CNotification::GetTextDisabledColor(const NotificationData& data)
{
	auto textdisabled_color = ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
	GetAlpha(data, textdisabled_color.w, &textdisabled_color.w);
	return textdisabled_color;
}

void CNotification::RenderWindow(NotificationData& data)
{
	static float prevtime = g_pGlobalVars->curtime;

	float pos_x, pos_y;
	GetScreenPosition(data, &pos_x, &pos_y);

	if (!data.count && data.drawState == DrawState_Normal)
	{ 
		static bool bMouseDownMissed;
		static bool bMouseDownCatch;

		const auto& io = ImGui::GetIO();

		if (IsCursorFocused(pos_x, pos_y))
		{
			if (io.MouseDown[0] && !bMouseDownMissed && !bMouseDownCatch)
			{
				data.time += cvar.settings.notifications_time - (g_pGlobalVars->curtime - data.time);
				bMouseDownCatch = true;
			}
		}
		else
		{
			if (io.MouseDown[0] && !bMouseDownMissed && !bMouseDownCatch)
				bMouseDownMissed = true;
		}

		if (!io.MouseDown[0] && bMouseDownMissed)
			bMouseDownMissed = false;

		if (!io.MouseDown[0] && bMouseDownCatch)
			bMouseDownCatch = false;

		if (IsCursorFocused(pos_x, pos_y) || bMouseDownCatch)
		{
			SwipeProcessing(data, &pos_x);

			if (bMouseDownCatch)
				data.time += g_pGlobalVars->curtime - prevtime;
		}

		if (data.drawState == DrawState_Disappearance)
			GetScreenPosition(data, &pos_x, &pos_y);
	}

	prevtime = g_pGlobalVars->curtime;

	ImGuiStyle& style = ImGui::GetStyle();
	style.Alpha = 1.00f;

	ImGui::SetNextWindowPos(ImVec2(pos_x, pos_y));
	ImGui::SetNextWindowSize(WINDOW_SIZE);

	ImGui::PushStyleColor(ImGuiCol_WindowBg, GetWindowBgColor(data));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
	
	if (ImGui::Begin(data.id.c_str(), static_cast<bool*>(0), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing))
	{
		ImGui::SameLine(WINDOW_PADDING);
		ImGui::BeginGroup();

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::PushTextWrapPos(WINDOW_SIZE.x - WINDOW_PADDING * 2.f);

		ImGui::PushStyleColor(ImGuiCol_Text, GetTextColor(data));
		ImGui::TextWrapped(data.title.c_str());
		ImGui::PopStyleColor();

		ImGui::Spacing();

		ImGui::PushStyleColor(ImGuiCol_Text, GetTextDisabledColor(data));
		ImGui::TextWrapped(data.message.c_str());
		ImGui::PopStyleColor();

		ImGui::PopTextWrapPos();

		ImGui::EndGroup();
		ImGui::End();
	}

	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}