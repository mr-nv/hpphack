#include "menu_fade_elements.h"

#include <Windows.h>

#include "..\..\..\ValveSDK\misc\GlobalVars.hpp"

extern CGlobalVarsBase* g_pGlobalVars;

CMenuFadeElements::CMenuFadeElements() :
	m_IsFadeElements(false), m_DurationTime(0.1f), m_SavedTime(0.f)
{
	RtlSecureZeroMemory(m_ColorsAlpha, sizeof(m_ColorsAlpha));
}

CMenuFadeElements::~CMenuFadeElements()
{
}

void CMenuFadeElements::FadeElementsInit()
{
	m_IsFadeElements = true;
	m_SavedTime = g_pGlobalVars->curtime;

	static const auto& style = ImGui::GetStyle();

	for (size_t i = 0; i < ImGuiCol_COUNT; i++)
	{
		if (i == ImGuiCol_WindowBg)
			continue;

		if (i == ImGuiCol_ChildBg)
			continue;

		m_ColorsAlpha[i] = style.Colors[i].w;
	}
}

void CMenuFadeElements::FadeElementsRun()
{
	if (!m_IsFadeElements)
		return;

	if (g_pGlobalVars->curtime < m_SavedTime + m_DurationTime)
	{
		static auto& style = ImGui::GetStyle();

		for (size_t i = 0; i < ImGuiCol_COUNT; i++)
		{
			if (i == ImGuiCol_WindowBg)
				continue;

			if (i == ImGuiCol_ChildBg)
				continue;

			style.Colors[i].w = Interpolate(m_ColorsAlpha[i]);
		}
	}
	else
	{
		m_IsFadeElements = false;
		m_SavedTime = 0.f;

		static auto& style = ImGui::GetStyle();

		for (size_t i = 0; i < ImGuiCol_COUNT; i++)
		{
			if (i == ImGuiCol_WindowBg)
				continue;

			if (i == ImGuiCol_ChildBg)
				continue;

			style.Colors[i].w = m_ColorsAlpha[i];
		}

		RtlSecureZeroMemory(m_ColorsAlpha, sizeof(m_ColorsAlpha));
	}
}

float CMenuFadeElements::Interpolate(float alpha)
{
	return (g_pGlobalVars->curtime - m_SavedTime) / m_DurationTime * (alpha - 0.f);
}