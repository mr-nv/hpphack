#include "..\menu_consts.h"
#include "menu_tabs.h"

#include <Windows.h>
#include <string>

#include "..\..\..\cvars.h"
#include "..\..\..\globals.h"

void CMenuTabs::TabMisc()
{
	BeginTab();
	BeginColumn();

	BeginChild("Miscellaneous##Miscellaneous", ImVec2(GetChildWidth(), 221));
	PushId("Miscellaneous.Miscellaneous");
	Checkbox(&cvar.misc.automatic_pistol, "Automatic pistol");
	Checkbox(&cvar.misc.automatic_reload, "Automatic reload");
	Checkbox(&cvar.misc.automatic_revolver, "Automatic revolver");
	EndChild();

	ImGui::Spacing();
	ImGui::Spacing();

	BeginChild("Knifebot##Miscellaneous", ImVec2(GetChildWidth(), 235));
	PushId("Miscellaneous.Knifebot");
	EndChild();

	NextColumn();

	BeginChild("Other##Miscellaneous", ImVec2(GetChildWidth(), 468));
	PushId("Miscellaneous.Other");

	if (ImGui::Button("Unload cheat Dll", ImVec2(GetButtonWidth(), 22)))
		g_Globals.m_bUnloadLibrary = true;

	EndChild();

	EndColumn();
	EndTab();
}