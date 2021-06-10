#include "menu_consts.h"
#include "menu.h"

#include "..\..\main.h"

CMenu::CMenu()
{
}

CMenu::~CMenu()
{
}

void CMenu::Draw()
{
	FadeGlobalsInit(IsOpened());

	if (!FadeGlobalsClose() && !IsOpened())
	{
		ResetCurrentWeaponId();
		return;
	}

	FadeGlobalsOpen();
	GetCurrentWeaponId();
	ApplyColorsChanges();

	static const auto flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar;

	ImGui::SetNextWindowPosCenter(ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(850, 550));

	if (ImGui::Begin("##Menu", static_cast<bool*>(0), flags))
	{
		DrawWindowTitle();

		if (DrawFeaturesTabs())
			FadeElementsInit();

		FadeElementsRun();

		const auto current_tab_id = GetCurrentTabId();

		switch (current_tab_id)
		{
		case RAGEBOT:
			TabRageBot();
			break;
		case LEGITBOT:
			TabLegitBot();
			break;
		case VISUALS:
			TabVisuals();
			break;
		case KREEDZ:
			TabKreedz();
			break;
		case MISC:
			TabMisc();
			break;
		case GUI:
			TabGUI();
			break;
		case SETTINGS:
			TabSettings();
		}

		ImGui::End();
	}
}

void CMenu::DrawCursor()
{
	static bool bSetCursorState;
	static auto& io = ImGui::GetIO();

	io.MouseDrawCursor = IsOpened();

	if (io.MouseDrawCursor)
	{
		g_pIVGuiSurface->UnlockCursor();
		g_pIInputSystem->ResetInputState();

		SetCursor(0);

		if (!bSetCursorState)
			bSetCursorState = true;
	}
	else
	{
		if (bSetCursorState)
		{
			SetCursor(LoadCursorA(0, IDC_ARROW));
			bSetCursorState = false;
		}
	}
}

void CMenu::ApplyColorsChanges()
{
	static auto& style = ImGui::GetStyle();

	for (unsigned i = 0; i < ImGuiCol_COUNT; i++)
		style.Colors[i] = ImColor(cvar.gui[i]);
}

CMenu g_Menu;