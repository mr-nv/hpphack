#include "esp.h"

#include "..\..\..\imgui\imgui.h"
#include "..\..\..\cvars.h"

CESP::CESP()
{
}

CESP::~CESP()
{
}

void CESP::DrawESP()
{
	if (cvar.visuals.esp_sound_enabled)
		DrawSound();

	if (cvar.visuals.esp_player_enabled)
		DrawPlayer();
}