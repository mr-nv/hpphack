#include "..\menu_consts.h"
#include "menu_tabs.h"

#include "..\..\..\cvars.h"

void CMenuTabs::TabKreedz()
{
	BeginTab(&cvar.kreedz.active, "Active##kreedz");
	EndTab();
}