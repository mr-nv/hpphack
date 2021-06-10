#include "menu_fade_globals.h"

#include "..\..\..\imgui\imgui.h"
#include "..\..\..\ValveSDK\misc\GlobalVars.hpp"

extern CGlobalVarsBase* g_pGlobalVars;

CMenuFadeGlobals::CMenuFadeGlobals() :
	m_IsOpened(false), m_IsFadeGlobals(false), m_DurationTime(0.1f), m_SavedTime(0.f)
{
}

CMenuFadeGlobals::~CMenuFadeGlobals()
{
}

void CMenuFadeGlobals::FadeGlobalsInit(bool is_opened)
{
	if (m_IsOpened == is_opened)
		return;

	m_SavedTime = g_pGlobalVars->curtime;
	m_IsOpened = is_opened;
	m_IsFadeGlobals = true;
}

bool CMenuFadeGlobals::FadeGlobalsOpen()
{
	if (!m_IsFadeGlobals)
		return false;

	if (!m_IsOpened)
		return false;

	static auto& style = ImGui::GetStyle();

	if (g_pGlobalVars->curtime < m_SavedTime + m_DurationTime)
	{
		style.Alpha = Interpolate(0.f, 1.f);
	}
	else
	{
		style.Alpha = 1.f;
		FadeGlobalsReset();
	}

	return false;
}

bool CMenuFadeGlobals::FadeGlobalsClose()
{
	if (!m_IsFadeGlobals)
		return false;

	if (m_IsOpened)
		return false;

	ImGuiStyle& style = ImGui::GetStyle();

	if (g_pGlobalVars->curtime < m_SavedTime + m_DurationTime)
	{
		style.Alpha = Interpolate(1.f, 0.f);
	}
	else
	{
		style.Alpha = 1.f;

		FadeGlobalsReset();
	}

	return m_IsFadeGlobals;
}

float CMenuFadeGlobals::Interpolate(float start, float end)
{
	return start + (g_pGlobalVars->curtime - m_SavedTime) / m_DurationTime * (end - start);
}

void CMenuFadeGlobals::FadeGlobalsReset()
{
	m_SavedTime = 0.f;
	m_IsFadeGlobals = false;
}