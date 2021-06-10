#include "menu_fade_background.h"

#include <Windows.h>
#include <string>

#include "..\..\..\globals.h"
#include "..\..\..\imgui\imgui.h"
#include "..\..\..\ValveSDK\misc\GlobalVars.hpp"
#include "..\..\..\ValveSDK\interfaces\IVEngineClient.hpp"
#include "..\..\render.h"

extern IVEngineClient* g_pIEngine;
extern CGlobalVarsBase* g_pGlobalVars;

CMenuFadeBackground::CMenuFadeBackground() :
	m_IsOpened(false), m_IsFadeBackground(false), m_DurationTime(0.1f), m_SavedTime(0.f)
{
}

CMenuFadeBackground::~CMenuFadeBackground()
{
}

void CMenuFadeBackground::FadeBackgroundInit(bool is_opened)
{
	if (m_IsOpened == is_opened)
		return;

	m_SavedTime = g_pGlobalVars->curtime;
	m_IsOpened = is_opened;
	m_IsFadeBackground = true;
}

void CMenuFadeBackground::FadeBackgroundRun()
{
	const auto max_alpha = g_pIEngine->IsInGame() ? 0.25f : 0.50f;

	if (m_IsFadeBackground && g_pGlobalVars->curtime < m_SavedTime + m_DurationTime)
	{
		const auto alpha = Interpolate(m_IsOpened ? 0.f : max_alpha, m_IsOpened ? max_alpha : 0.f);

		g_Render.AddRectFilled(ImVec2(), ImVec2(g_Globals.m_flScreenWidth, 
			g_Globals.m_flScreenHeight), ImVec4(0.00f, 0.00f, 0.00f, alpha));
	}
	else
	{
		if (m_IsOpened)
		{
			g_Render.AddRectFilled(ImVec2(), ImVec2(g_Globals.m_flScreenWidth,
				g_Globals.m_flScreenHeight), ImVec4(0.00f, 0.00f, 0.00f, max_alpha));
		}

		m_IsFadeBackground = false;
	}
}

float CMenuFadeBackground::Interpolate(float start, float end)
{
	return start + (g_pGlobalVars->curtime - m_SavedTime) / m_DurationTime * (end - start);
}