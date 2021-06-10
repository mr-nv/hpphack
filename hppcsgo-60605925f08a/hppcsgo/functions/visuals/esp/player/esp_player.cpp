#include "esp_player.h"

#include "..\..\..\..\main.h"
#include "..\..\..\..\weapon_map.h"

CDrawPlayer::CDrawPlayer()
{
}

CDrawPlayer::~CDrawPlayer()
{
}

void CDrawPlayer::DrawPlayer()
{
	const auto& selection_players = GetSelectionPlayers();

	if (!selection_players.empty())
	{
		for (auto& player : selection_players)
		{
			if (CalcScreen(player.index))
			{
				if (cvar.visuals.esp_player_box)
					BoundBox(player.index);

				if (cvar.visuals.esp_player_health)
					HealthBar(player.index);

				if (cvar.visuals.esp_player_armor)
					ArmorBar(player.index);

				if (cvar.visuals.esp_player_name)
					NickName(player.index);

				if (cvar.visuals.esp_player_weapon)
					WeaponName(player.index);

				if (cvar.visuals.esp_player_distance)
					Distance(player.index);

				if (cvar.visuals.esp_player_skeleton)
					Skeleton(player.index);
			}
			else
			{
				if (cvar.visuals.esp_player_out_of_pov)
					OutPovScreen(player.index);
			}
		}
	}
}

float CDrawPlayer::InterpolateFadeout(int index, float alpha)
{
	return alpha - ((g_pGlobalVars->curtime - g_Player[index]->m_flHistory) /
		cvar.visuals.esp_player_history_time) * alpha;
}

void CDrawPlayer::ReduceTransparency(int index, ImColor& color)
{
	if (g_Player[index]->m_IsGunGameImmunity)
		color.Value.w *= 0.25f;
}

void CDrawPlayer::FadeoutDormant(int index, ImColor& color)
{
	if (g_Player[index]->m_IsDormant)
	{
		if (cvar.visuals.esp_player_fadeout_dormant)
			color.Value.w = InterpolateFadeout(index, color.Value.w);
		else
			color = ImColor(0.5f, 0.5f, 0.5f, color.Value.w);
	}
}

ImColor CDrawPlayer::GetTeamColor(int index)
{
	switch (g_Player[index]->m_iTeamNum)
	{
	case TEAM_TERRORIST:
		return IsVisible(index)
			? cvar.visuals.esp_player_box_color_visible_t
			: cvar.visuals.esp_player_box_color_hide_t;
	case TEAM_COUNTER_TERRORIST:
		return IsVisible(index)
			? cvar.visuals.esp_player_box_color_visible_ct
			: cvar.visuals.esp_player_box_color_hide_ct;
	}

	return ImColor();
}

ImRect CDrawPlayer::GetRect(float* flScreenTop, float* flScreenBot)
{
	ImVec2 min, max;
	max.y = max(ceil(flScreenBot[1] - flScreenTop[1]), 18.f);
	max.x = ceil(max.y * 0.55f);
	min.y = ceil(flScreenTop[1]);
	min.x = ceil(flScreenBot[0] - max.x * 0.5f);
	return ImRect(min, min + max);
}

ImVec2 CDrawPlayer::GetTextSize(const char* text)
{
	static const auto* const pFont = ImGui::GetIO().Fonts->Fonts[0];
	return pFont->CalcTextSizeA(cvar.visuals.esp_font_size, FLT_MAX, 0, text);
}

FontFlag CDrawPlayer::GetFlags(int position)
{
	static auto LGetFontFlag = [&](int value)
	{
		switch (value)
		{
		case 1: return SHADOW;
		case 2: return OUTLINE;
		}

		return NONE;
	};

	const auto flags = LGetFontFlag(cvar.visuals.esp_font_outline);
	return static_cast<FontFlag>(position > Pos_Right ? (flags | CENTER_X) : flags);
}

float CDrawPlayer::GetHeight()
{
	return m_Rect.Max.y - m_Rect.Min.y;
}

float CDrawPlayer::GetWidth()
{
	return m_Rect.Max.x - m_Rect.Min.x;
}

float CDrawPlayer::GetAlignOffset()
{
	return floor(static_cast<float>(cvar.visuals.esp_player_box_thickness) * 0.5f);
}

float CDrawPlayer::GetSpacingBar()
{
	return cvar.visuals.esp_player_box_outline ? 3.f : 1.f;
}

float CDrawPlayer::GetSpacingText(int index, int pos)
{
	auto spacing = cvar.visuals.esp_player_box_outline ? 3.f : 2.f;

	if (pos == cvar.visuals.esp_player_health)
	{
		spacing += cvar.visuals.esp_player_box_thickness;
		spacing += GetSpacingBar();
	}

	if (pos == cvar.visuals.esp_player_armor)
	{
		auto armor = static_cast<float>(g_Player[index]->m_iArmorValue);
		armor = std::clamp<float>(armor, 0, 100);

		if (armor)
		{
			spacing += cvar.visuals.esp_player_box_thickness;
			spacing += GetSpacingBar();
		}
	}

	return spacing;
}

float CDrawPlayer::GetNextlineText(const ImVec2& text_size, int& pos)
{
	auto nextline = 0.f;

	if (&pos == &cvar.visuals.esp_player_name)
		return nextline;

	if (pos == cvar.visuals.esp_player_name)
		nextline += text_size.y;

	if (&pos == &cvar.visuals.esp_player_weapon)
		return nextline;

	if (pos == cvar.visuals.esp_player_weapon)
		nextline += text_size.y;

	if (&pos == &cvar.visuals.esp_player_distance)
		return nextline;

	if (pos == cvar.visuals.esp_player_distance)
		nextline += text_size.y;

	return nextline;
}

ImVec2 CDrawPlayer::GetPositionText(float spacing, float nextline, const ImVec2& text_size, int pos)
{
	const auto align = GetAlignOffset();

	m_Rect.Min -= align;
	m_Rect.Max += align;

	float x, y;

	switch (pos)
	{
	case Pos_Left:
		x = m_Rect.Min.x - spacing - text_size.x;
		y = m_Rect.Min.y + nextline;
		break;
	case Pos_Right:
		x = m_Rect.Max.x + spacing;
		y = m_Rect.Min.y + nextline;
		break;
	case Pos_Top:
		x = m_Rect.Min.x + GetWidth() * 0.5f;
		y = m_Rect.Min.y - spacing - text_size.y - nextline;
		break;
	case Pos_Bot:
		x = m_Rect.Min.x + GetWidth() * 0.5f;
		y = m_Rect.Max.y + spacing + nextline;
	}

	m_Rect.Min += align;
	m_Rect.Max -= align;

	return ImVec2(x, y);
}

ImColor CDrawPlayer::GetProcessedColor(int index, ImColor color)
{
	ReduceTransparency(index, color);
	FadeoutDormant(index, color);
	return color;
}

bool CDrawPlayer::IsVisible(int index)
{
	if (cvar.visuals.esp_player_visible_check)
	{
		static const std::vector<Hitboxes> hitboxes =
		{
			HITBOX_HEAD,
			HITBOX_LOWER_CHEST,
			HITBOX_RIGHT_FOOT,
			HITBOX_LEFT_FOOT,
			HITBOX_RIGHT_HAND,
			HITBOX_LEFT_HAND,
		};

		for (auto& hitbox : hitboxes)
		{
			if (g_Utils.IsVisible(index, g_Player[index]->m_vecHitbox[hitbox]))
				return true;
		}
	}

	return false;
}

bool CDrawPlayer::CalcScreen(int index)
{
	float flScreenTop[2], flScreenBot[2];

	auto top = g_Player[index]->m_IsDormant 
		? g_Player[index]->m_vecOrigin 
		: g_Player[index]->m_vecHitbox[HITBOX_HEAD];

	top.z += g_Player[index]->m_IsDormant
		? 72.f
		: 10.f;

	auto bot = g_Player[index]->m_vecOrigin;
	bot.z -= 5.f;

	const auto ret = g_Utils.WorldToScreen(top, flScreenTop) &&
		g_Utils.WorldToScreen(bot, flScreenBot);

	m_Rect = GetRect(flScreenTop, flScreenBot);

	return ret;
}

void CDrawPlayer::BoundBox(int index)
{
	const auto color_team = GetTeamColor(index);
	const auto color = GetProcessedColor(index, color_team);

	switch (cvar.visuals.esp_player_box)
	{
	case BoxType_Default:
		g_Render.AddRect(m_Rect.Min, m_Rect.Max, color, cvar.visuals.esp_player_box_outline,
			0.f, ImDrawCornerFlags_All, cvar.visuals.esp_player_box_thickness);
		return;
	case BoxType_Corner:
		g_Render.AddRectCorner(m_Rect.Min, m_Rect.Max, color, cvar.visuals.esp_player_box_outline,
			cvar.visuals.esp_player_box_thickness);
		return;
	case BoxType_Round:
		g_Render.AddRect(m_Rect.Min, m_Rect.Max, color, cvar.visuals.esp_player_box_outline, 
			3.f, ImDrawCornerFlags_All, cvar.visuals.esp_player_box_thickness);
	}
}

void CDrawPlayer::DrawBar(int pos, float value, const ImVec4& color, float spacing)
{
	const auto align = GetAlignOffset();

	m_Rect.Min -= align;
	m_Rect.Max += align;

	const auto size = (pos <= Pos_Right ? GetHeight() : GetWidth()) / 100.f * value;
	const auto thickness = cvar.visuals.esp_player_box_thickness;
	
	const auto color_background = ImVec4(0.25f, 0.25, 0.25f, color.w);
	const auto color_outline = ImVec4(0.00f, 0.00f, 0.00f, color.w);

	float v;

	switch (pos)
	{
	case Pos_Left:	v = m_Rect.Min.x - spacing - thickness; break;
	case Pos_Right:	v = m_Rect.Max.x + spacing; break;
	case Pos_Top:	v = m_Rect.Min.y - spacing - thickness; break;
	case Pos_Bot:	v = m_Rect.Max.y + spacing;
	}

	switch (pos)
	{
	case Pos_Left:
	case Pos_Right:
		g_Render.AddRectFilled(ImVec2(v, m_Rect.Min.y), ImVec2(v + thickness, m_Rect.Max.y), color_background);
		g_Render.AddRectFilled(ImVec2(v, m_Rect.Max.y), ImVec2(v + thickness, m_Rect.Max.y - size), color);

		if (cvar.visuals.esp_player_box_outline)
			g_Render.AddRect(ImVec2(v - 1, m_Rect.Min.y - 1), ImVec2(v + thickness + 1, m_Rect.Max.y + 1), color_outline);
		break;
	case Pos_Top:
	case Pos_Bot:
		g_Render.AddRectFilled(ImVec2(m_Rect.Min.x, v), ImVec2(m_Rect.Max.x, v + thickness), color_background);
		g_Render.AddRectFilled(ImVec2(m_Rect.Min.x, v), ImVec2(m_Rect.Min.x + size, v + thickness), color);

		if (cvar.visuals.esp_player_box_outline)
			g_Render.AddRect(ImVec2(m_Rect.Min.x - 1, v - 1), ImVec2(m_Rect.Max.x + 1, v + thickness + 1), color_outline);
	}

	m_Rect.Min += align;
	m_Rect.Max -= align;
}

void CDrawPlayer::HealthBar(int index)
{
	auto health = static_cast<float>(g_Player[index]->m_iHealth);
	health = std::clamp<float>(health, 0, 100);
	auto color_health = ImColor(
		health <= 50.f ? 1.00f : fInterp(50.f, health, 100.f, 1.00f, 0.00f),
		health >= 50.f ? 1.00f : fInterp(0.f, health, 50.f, 0.00f, 1.00f),
		0.00f);
	const auto color = GetProcessedColor(index, color_health);
	DrawBar(cvar.visuals.esp_player_health, health, color, GetSpacingBar());
}

void CDrawPlayer::ArmorBar(int index)
{
	auto armor = static_cast<float>(g_Player[index]->m_iArmorValue);
	armor = std::clamp<float>(armor, 0, 100);

	if (!armor)
		return;

	const auto color = GetProcessedColor(index, ImColor(0.75f, 0.75f, 0.75f));
	auto spacing = GetSpacingBar();

	if (cvar.visuals.esp_player_armor == cvar.visuals.esp_player_health)
	{
		spacing += cvar.visuals.esp_player_box_thickness;
		spacing += GetSpacingBar();
	}

	DrawBar(cvar.visuals.esp_player_armor, armor, color, spacing);
}

void CDrawPlayer::NickName(int index)
{
	player_info_t info;
	g_pIEngine->GetPlayerInfo(index, &info);

	if (info.szName[0] == '\0')
		return;

	if (!strlen(info.szName))
		return;

	const auto text_size = GetTextSize(info.szName);
	const auto nextline = GetNextlineText(text_size, cvar.visuals.esp_player_name);
	const auto spacing = GetSpacingText(index, cvar.visuals.esp_player_name);
	const auto pos = GetPositionText(spacing, nextline, text_size, cvar.visuals.esp_player_name);
	const auto color = GetProcessedColor(index, cvar.visuals.esp_font_color);
	const auto flags = GetFlags(cvar.visuals.esp_player_name);

	g_Render.AddText(pos.x, pos.y, color, text_size.y, flags, u8"%s", info.szName);
}

void CDrawPlayer::WeaponName(int index)
{
	if (!g_Player[index]->m_hActiveWeapon)
		return;

	if (!g_Player[index]->m_CSWeaponData)
		return;

	const auto weapon_id = g_Player[index]->m_iWeaponId;
	std::string weapon_info = GetWeaponName(weapon_id);

	if (cvar.visuals.esp_player_weapon_ammo && g_Utils.IsGun(weapon_info))
	{
		std::stringstream ss;
		ss << " [" << g_Player[index]->m_iWeaponClip1 << "]";
		weapon_info.append(ss.str());
	}

	const auto text_size = GetTextSize(weapon_info.c_str());
	const auto nextline = GetNextlineText(text_size, cvar.visuals.esp_player_weapon);
	const auto spacing = GetSpacingText(index, cvar.visuals.esp_player_weapon);
	const auto pos = GetPositionText(spacing, nextline, text_size, cvar.visuals.esp_player_weapon);
	const auto color = GetProcessedColor(index, cvar.visuals.esp_font_color);
	const auto flags = GetFlags(cvar.visuals.esp_player_weapon);

	g_Render.AddText(pos.x, pos.y, color, text_size.y, flags, weapon_info.c_str());
}

void CDrawPlayer::Distance(int index)
{
	auto distance = floor(g_Local->m_vecOrigin.DistTo(g_Player[index]->m_vecOrigin));
	distance -= g_Local->m_vecBBMaxs.y + g_Player[index]->m_vecBBMaxs.y;

	if (cvar.visuals.esp_player_distance_measure)
		distance *= 0.025f;

	char buf[32];
	sprintf_s(buf, cvar.visuals.esp_player_distance_measure ? "%0.f m" : "%0.f u", distance);

	const auto text_size = GetTextSize(buf);
	const auto nextline = GetNextlineText(text_size, cvar.visuals.esp_player_distance);
	const auto spacing = GetSpacingText(index, cvar.visuals.esp_player_distance);
	const auto pos = GetPositionText(spacing, nextline, text_size, cvar.visuals.esp_player_distance);
	const auto color = GetProcessedColor(index, cvar.visuals.esp_font_color);
	const auto flags = GetFlags(cvar.visuals.esp_player_distance);

	g_Render.AddText(pos.x, pos.y, color, text_size.y, flags, buf);
}

void CDrawPlayer::Skeleton(int index)
{
	static auto LSkeletonLine = [&](int index, int hitbox_start, int hitbox_end)
	{
		float flScreenHitboxStart[2], flScreenHitboxEnd[2];

		if (g_Utils.WorldToScreen(g_Player[index]->m_vecHitbox[hitbox_start], flScreenHitboxStart) &&
			g_Utils.WorldToScreen(g_Player[index]->m_vecHitbox[hitbox_end], flScreenHitboxEnd))
		{
			const auto color = GetProcessedColor(index, cvar.visuals.esp_player_skeleton_color);

			g_Render.AddLine(ImVec2(flScreenHitboxStart[0], flScreenHitboxStart[1]), 
				ImVec2(flScreenHitboxEnd[0], flScreenHitboxEnd[1]), color, cvar.visuals.esp_player_box_thickness);
		}
	};

	LSkeletonLine(index, 0, 1);
	LSkeletonLine(index, 1, 6);
	LSkeletonLine(index, 6, 5);
	LSkeletonLine(index, 5, 4);
	LSkeletonLine(index, 4, 3);

	LSkeletonLine(index, 6, 15);
	LSkeletonLine(index, 15, 16);
	LSkeletonLine(index, 16, 13);
	LSkeletonLine(index, 6, 17);
	LSkeletonLine(index, 17, 18);
	LSkeletonLine(index, 18, 14);

	LSkeletonLine(index, 3, 8);
	LSkeletonLine(index, 8, 10);
	LSkeletonLine(index, 10, 12);
	LSkeletonLine(index, 3, 7);
	LSkeletonLine(index, 7, 9);
	LSkeletonLine(index, 9, 11);
}

void CDrawPlayer::OutPovScreen(int index)
{
	static auto LFindPoint = [](float* point)
	{
		const float flScreenCenter[2] =
		{
			g_Globals.m_flScreenWidth * 0.5f,
			g_Globals.m_flScreenHeight * 0.5f
		};

		const auto d = pow((point[0] - flScreenCenter[0] * 0.5f), 2) + pow((point[1] - flScreenCenter[1] * 0.5f), 2);
		const auto r = flScreenCenter[1] / sqrt(d);

		point[0] = r * point[0] + (1.f - r) * flScreenCenter[0];
		point[1] = r * point[1] + (1.f - r) * flScreenCenter[1];
	};

	static auto LWorldToScreen = [](const Vector& vecOrigin, float* flScreenPos)
	{
		const auto ret = g_Utils.WorldToScreen(vecOrigin, flScreenPos);

		flScreenPos[0] = g_Globals.m_flScreenWidth * 0.5f + flScreenPos[0] * g_Globals.m_flScreenWidth * 0.5f;
		flScreenPos[1] = g_Globals.m_flScreenHeight * 0.5f + flScreenPos[1] * g_Globals.m_flScreenHeight * 0.5f;
		LFindPoint(flScreenPos);

		return ret;
	};
	
	float flScreenPos[2];

	if (LWorldToScreen(g_Player[index]->m_vecHitbox[HITBOX_STOMACH], flScreenPos))
		return;

	const float flScreenCenter[2] =
	{
		g_Globals.m_flScreenWidth * 0.5f,
		g_Globals.m_flScreenHeight * 0.5f
	};

	const auto forward = Vector(
		flScreenPos[0] - flScreenCenter[0],
		flScreenPos[1] - flScreenCenter[1],
		0.f);

	auto angles = QAngle();

	g_Utils.VectorAngles(forward, angles);
	angles.Negate();

	const float radius = (flScreenCenter[1] - 30.f) * cvar.visuals.esp_player_out_of_pov_radius;
	const float yaw = DirectX::XMConvertToRadians(angles.y);
	const float new_point_x = flScreenCenter[0] + radius * cos(yaw);
	const float new_point_y = flScreenCenter[1] + radius * sin(yaw);

	const auto color_team = GetTeamColor(index);
	const auto color = GetProcessedColor(index, color_team);

	if (cvar.visuals.esp_player_out_of_pov == Arrows)
	{
		ImVec2 points[3];
		points[0] = ImVec2(new_point_x - 10, new_point_y - 10);
		points[1] = ImVec2(new_point_x + 12, new_point_y);
		points[2] = ImVec2(new_point_x - 10, new_point_y + 10);

		ImVec2 points_center;
		for (size_t i = 0; i < ImVec2::MAX_SIZE; i++)
			points_center[i] = (points[0][i] + points[1][i] + points[2][i]) / 3.f;

		for (auto& point : points)
		{
			point -= points_center;

			const float temp_x = point.x;
			const float temp_y = point.y;

			const float theta = DirectX::XMConvertToRadians(angles.y);
			const float c = cos(theta);
			const float s = sin(theta);

			point.x = temp_x * c - temp_y * s;
			point.y = temp_x * s + temp_y * c;

			point += points_center;
		}

		g_Render.AddTriangleFilled(points[0], points[1], points[2], color);
	}
	else
	{
		const auto color_font = GetProcessedColor(index, cvar.visuals.esp_font_color);

		if (cvar.visuals.esp_player_out_of_pov == CirclesName)
		{
			player_info_t info;
			g_pIEngine->GetPlayerInfo(index, &info);

			if (info.szName[0] == '\0')
				return;

			if (!strlen(info.szName))
				return;

			const auto text_size = GetTextSize(info.szName);

			g_Render.AddText(new_point_x, new_point_y + text_size.y, 
				color_font, text_size.y, GetFlags(Pos_Bot), u8"%s", info.szName);
		}
		else if (cvar.visuals.esp_player_out_of_pov == CirclesWeapon)
		{
			if (!g_Player[index]->m_hActiveWeapon || !g_Player[index]->m_CSWeaponData)
				return;

			const auto weapon_id = g_Player[index]->m_iWeaponId;
			std::string weapon_info = GetWeaponName(weapon_id);
			const auto text_size = GetTextSize(weapon_info.c_str());

			g_Render.AddText(new_point_x, new_point_y + text_size.y,
				color_font, text_size.y, GetFlags(Pos_Bot), weapon_info.c_str());
		}
		else if (cvar.visuals.esp_player_out_of_pov == CirclesDistance)
		{
			auto distance = floor(g_Local->m_vecOrigin.DistTo(g_Player[index]->m_vecOrigin));
			distance -= g_Local->m_vecBBMaxs.y + g_Player[index]->m_vecBBMaxs.y;

			if (cvar.visuals.esp_player_distance_measure)
				distance *= 0.025f;

			char buf[32];
			sprintf_s(buf, cvar.visuals.esp_player_distance_measure ? "%0.f m" : "%0.f u", distance);

			const auto text_size = GetTextSize(buf);

			g_Render.AddText(new_point_x, new_point_y + text_size.y, 
				color_font, text_size.y, GetFlags(Pos_Bot), buf);
		}

		g_Render.AddCircleFilled(ImVec2(new_point_x, new_point_y), 8.f, color);
	}
}