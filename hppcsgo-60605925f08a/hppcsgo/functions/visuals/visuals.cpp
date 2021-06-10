#include "visuals.h"

#include "..\..\imgui\imgui.h"
#include "..\..\cvars.h"

CVisuals::CVisuals()
{
}

CVisuals::~CVisuals()
{
}

void CVisuals::Draw()
{
	if (cvar.visuals.active)
		DrawESP();
}

CVisuals g_Visuals;