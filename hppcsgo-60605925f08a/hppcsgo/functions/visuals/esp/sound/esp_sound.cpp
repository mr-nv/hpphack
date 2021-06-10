#include "esp_sound.h"

#include "..\..\..\..\main.h"
#include "..\..\..\..\render\render.h"

CDrawSound::CDrawSound()
{
}

CDrawSound::~CDrawSound()
{
}

void CDrawSound::DrawSound()
{
	auto& sounds = GetSoundData();

	for (auto& sound : sounds)
	{
		if (sound.time + cvar.visuals.esp_sound_fadeout_time < g_pGlobalVars->curtime)
		{
			sounds.pop_front();
			continue;
		}

		float flScreenPos[2];

		if (!g_Utils.WorldToScreen(sound.origin, flScreenPos))
			continue;	

		switch (cvar.visuals.esp_sound_draw_type)
		{
		case SoundType_Boxes:
			Box(sound, flScreenPos);
			break;
		case SoundType_Circles:
			Circle(sound);
		}
	}
}

float CDrawSound::InterpolateFadeout(float time, float start, float end)
{
	return start + ((g_pGlobalVars->curtime - time) / 
		cvar.visuals.esp_sound_fadeout_time) * (end - start);
}

ImColor CDrawSound::GetTeamColor(int index)
{
	switch (g_Player[index]->m_iTeamNum)
	{
	case TEAM_TERRORIST:
		return cvar.visuals.esp_sound_color_t;
	case TEAM_COUNTER_TERRORIST:
		return cvar.visuals.esp_sound_color_ct;
	}

	return ImColor();
}

ImColor CDrawSound::GetProcessedColor(const SoundData& data)
{
	auto color = GetTeamColor(data.index);
	color.Value.w = InterpolateFadeout(data.time, 1.00f, 0.00f);
	return color;
}

void CDrawSound::Circle(const SoundData& data)
{
	static auto step = IM_PI * 0.075f;
	auto prev_point = Vector();

	for (float lat = 0.f; lat <= IM_PI * 3.f; lat += step)
	{
		const auto sin1 = sin(lat);
		const auto cos1 = cos(lat);
		const auto sin3 = sin(0);
		const auto cos3 = cos(0);

		const auto radius = InterpolateFadeout(data.time, 0.f, cvar.visuals.esp_sound_size);
		const auto point1 = Vector(sin1 * cos3, cos1, sin1 * sin3) * radius;
		const auto point3 = data.origin + point1;
		const auto color = GetProcessedColor(data);

		float flPoint[2], flPrevious[2];

		if (g_Utils.WorldToScreen(point3, flPoint) && g_Utils.WorldToScreen(prev_point, flPrevious) && lat > 0.f)
			g_Render.AddLine(ImVec2(flPrevious[0], flPrevious[1]), ImVec2(flPoint[0], flPoint[1]), color);

		prev_point = point3;
	}
}

void CDrawSound::Box(const SoundData& data, float* flScreenPos)
{
	const auto size = InterpolateFadeout(data.time, cvar.visuals.esp_sound_size, 0.f);
	const auto a = ImVec2(flScreenPos[0], flScreenPos[1]) - (size * 0.5f);
	const auto b = a + size;
	const auto color = GetProcessedColor(data);

	g_Render.AddRect(a, b, color, cvar.visuals.esp_player_box_outline);
}