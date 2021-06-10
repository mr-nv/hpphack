#include "main.h"

#include "render\render.h"
#include "visuals.h"

CVisuals g_Visuals;

static FontFlag GetFontFlag(int value)
{
	switch (value)
	{
	case 1: return FontFlag::SHADOW;
	case 2: return FontFlag::OUTLINE;
	}

	return FontFlag::NONE;
}

class CDrawPlayer
{
public:
	void Draw(int iEntIndex)
	{
		if (CalcScreen(iEntIndex))
		{
			if (cvar.visuals.esp_player_box)
				DrawBox(iEntIndex);

			if (cvar.visuals.esp_player_health)
				DrawHealth(iEntIndex);

			if (cvar.visuals.esp_player_armor)
				DrawArmor(iEntIndex);

			if (cvar.visuals.esp_player_name)
				DrawName(iEntIndex);

			if (cvar.visuals.esp_player_weapon)
				DrawWeapon(iEntIndex);

			if (cvar.visuals.esp_player_distance)
				DrawDistance(iEntIndex);

			if (cvar.visuals.esp_player_skeleton)
				DrawSkeleton(iEntIndex);
		}
		else
		{
			if (cvar.visuals.esp_player_out_of_pov)
				OutPovScreen(iEntIndex);
		}
	}

private:
	enum BoxTypesList
	{
		BoxType_Default = 1,
		BoxType_Corner,
		BoxType_Round
	};

	enum PositionsList
	{
		LEFT = 1,
		RIGHT,
		TOP,
		BOT
	};

	enum OutOfPovList
	{
		OutOfPov_Arrows = 1,
		OutOfPov_CirclesName,
		OutOfPov_CirclesWeapon,
		OutOfPov_CirclesDistance
	};

	enum ETeamNumDefinition
	{
		TERRORIST = 2,
		CT = 3
	};

	ImRect m_Rect;

	bool IsVisiblePlayer(int iEntIndex)
	{
		if (cvar.visuals.esp_player_visible_check)
		{
			static const std::vector<Hitboxes> hitboxes =
			{
				HITBOX_HEAD,
				HITBOX_RIGHT_HAND,
				HITBOX_LEFT_HAND,
				HITBOX_RIGHT_UPPER_ARM,
				HITBOX_LEFT_UPPER_ARM,
				HITBOX_RIGHT_FOOT,
				HITBOX_LEFT_FOOT,
				HITBOX_CHEST,
				HITBOX_STOMACH,
				HITBOX_PELVIS
			};

			for (auto& hitbox : hitboxes)
			{
				if (g_Utils.IsVisible(iEntIndex, g_Player[iEntIndex]->m_vecHitbox[hitbox]))
					return true;
			}
		}

		return false;
	}

	ImColor GetTeamColor(int iEntIndex)
	{
		switch (g_Player[iEntIndex]->m_iTeamNum)
		{
		case TERRORIST:
			return IsVisiblePlayer(iEntIndex)
				? cvar.visuals.esp_player_box_color_visible_t
				: cvar.visuals.esp_player_box_color_hide_t;
		case CT:
			return IsVisiblePlayer(iEntIndex)
				? cvar.visuals.esp_player_box_color_visible_ct
				: cvar.visuals.esp_player_box_color_hide_ct;
		}

		return ImColor(IM_COL32_WHITE);
	}

	ImRect GetRect(float(&flScreenTop)[2], float(&flScreenBot)[2])
	{
		ImVec2 min, max;
		max.y = floor(flScreenBot[1] - flScreenTop[1]);
		max.x = floor(max.y * 0.55f);
		min.y = floor(flScreenTop[1]);
		min.x = floor(flScreenBot[0] - max.x * 0.5f);
		return ImRect(min, min + max);
	}

	bool CalcScreen(int nPlayerIndex)
	{
		float flScreenTop[2], flScreenBot[2];

		Vector vecMaxs = g_Player[nPlayerIndex]->m_vecHitbox[HITBOX_HEAD];
		vecMaxs.z += 10.f;
		Vector vecMins = g_Player[nPlayerIndex]->m_vecOrigin;
		vecMins.z -= 5.f;

		const bool ret = g_Utils.WorldToScreen(vecMaxs, flScreenTop) &&
			g_Utils.WorldToScreen(vecMins, flScreenBot);

		m_Rect = GetRect(flScreenTop, flScreenBot);

		return ret;
	}

	float InterpolateFadeout(int iEntIndex, float f1)
	{
		return fInterp(g_Player[iEntIndex]->m_flHistory, g_pGlobalVars->curtime,
			g_Player[iEntIndex]->m_flHistory + cvar.visuals.esp_player_history_time, f1, 0.f);
	}

	void FadeoutDormant(int iEntIndex, ImColor & color)
	{
		if (!g_Player[iEntIndex]->m_IsDormant)
			return;

		if (cvar.visuals.esp_player_fadeout_dormant)
			color.Value.w = InterpolateFadeout(iEntIndex, color.Value.w);
		else
			color = ImColor(0.5f, 0.5f, 0.5f, color.Value.w);
	}

	void ReduceTransparency(int iEntIndex, ImColor & color)
	{
		if (g_Player[iEntIndex]->m_IsGunGameImmunity)
			color.Value.w *= 0.45f;
	}

	void DrawBox(int iEntIndex)
	{
		ImColor color = GetTeamColor(iEntIndex);
		ReduceTransparency(iEntIndex, color);
		FadeoutDormant(iEntIndex, color);

		switch (cvar.visuals.esp_player_box)
		{
		case BoxType_Default:
			g_Render.AddRect(m_Rect.Min, m_Rect.Max, color, cvar.visuals.esp_player_box_outline);
			break;
		case BoxType_Corner:
			g_Render.AddRectCorner(m_Rect.Min, m_Rect.Max, color, cvar.visuals.esp_player_box_outline);
			break;
		case BoxType_Round:
			g_Render.AddRect(m_Rect.Min, m_Rect.Max, color, cvar.visuals.esp_player_box_outline, 3.f);
		}
	}

	void DrawName(int iEntIndex)
	{
		player_info_t info;
		g_pIEngine->GetPlayerInfo(iEntIndex, &info);

		if (!strlen(info.szName))
			return;

		const ImFont* pFont = ImGui::GetIO().Fonts->Fonts[0];
		const ImVec2  text_size = pFont->CalcTextSizeA(cvar.visuals.esp_font_size, FLT_MAX, 0, info.szName);

		float x, y, spacing = 2.f;

		if (cvar.visuals.esp_player_name == cvar.visuals.esp_player_health)
			spacing += GetBarSpacing() + GetBarThickness();

		if (cvar.visuals.esp_player_name == cvar.visuals.esp_player_armor && g_Player[iEntIndex]->m_iArmorValue)
			spacing += GetBarSpacing() + GetBarThickness();

		switch (cvar.visuals.esp_player_name)
		{
		case LEFT:
			x = m_Rect.Min.x - spacing - text_size.x;
			y = fix_size(m_Rect.Min.y);
			break;
		case RIGHT:
			x = m_Rect.Max.x + spacing;
			y = fix_size(m_Rect.Min.y);
			break;
		case TOP:
			x = m_Rect.Min.x + width() * 0.5f;
			y = m_Rect.Min.y - spacing - cvar.visuals.esp_font_size;
			break;
		case BOT:
			x = m_Rect.Min.x + width() * 0.5f;
			y = fix_size(m_Rect.Max.y + spacing);
		}

		ImColor color = cvar.visuals.esp_font_color;
		ReduceTransparency(iEntIndex, color);
		FadeoutDormant(iEntIndex, color);

		g_Render.AddText(x, y, color, cvar.visuals.esp_font_size,
			GetFlags(cvar.visuals.esp_player_name), u8"%s", info.szName);
	}

	void DrawWeapon(int iEntIndex)
	{
		if (!g_Player[iEntIndex]->m_hActiveWeapon || !g_Player[iEntIndex]->m_CSWeaponData)
			return;

		std::string sWeaponInfo = GetWeaponNameByIndex(g_Player[iEntIndex]->m_iWeaponId);

		if (cvar.visuals.esp_player_weapon_ammo && g_Utils.IsGun(sWeaponInfo))
		{
			std::stringstream ss;
			ss << " (" << g_Player[iEntIndex]->m_iWeaponClip1 << ")";
			sWeaponInfo.append(ss.str());
		}

		const float nextline = (cvar.visuals.esp_player_weapon == cvar.visuals.esp_player_name) ?
			fix_size(cvar.visuals.esp_font_size) : 0.f;

		const ImFont * pFont = ImGui::GetIO().Fonts->Fonts[0];
		const ImVec2   text_size = pFont->CalcTextSizeA(cvar.visuals.esp_font_size, FLT_MAX, 0, sWeaponInfo.c_str());

		float x, y, spacing = 2.f;

		if (cvar.visuals.esp_player_weapon == cvar.visuals.esp_player_health)
			spacing += GetBarSpacing() + GetBarThickness();

		if (cvar.visuals.esp_player_weapon == cvar.visuals.esp_player_armor && g_Player[iEntIndex]->m_iArmorValue)
			spacing += GetBarSpacing() + GetBarThickness();

		switch (cvar.visuals.esp_player_weapon)
		{
		case LEFT:
			x = m_Rect.Min.x - spacing - text_size.x;
			y = fix_size(m_Rect.Min.y + nextline);
			break;
		case RIGHT:
			x = m_Rect.Max.x + spacing;
			y = fix_size(m_Rect.Min.y + nextline);
			break;
		case TOP:
			x = m_Rect.Min.x + width() * 0.5f;
			y = fix_size(m_Rect.Min.y - spacing - fix_size(cvar.visuals.esp_font_size) - nextline);
			break;
		case BOT:
			x = m_Rect.Min.x + width() * 0.5f;
			y = fix_size(m_Rect.Max.y + spacing + nextline);
		}

		ImColor color = cvar.visuals.esp_font_color;
		ReduceTransparency(iEntIndex, color);
		FadeoutDormant(iEntIndex, color);

		g_Render.AddText(x, y, color, cvar.visuals.esp_font_size,
			GetFlags(cvar.visuals.esp_player_weapon), sWeaponInfo.c_str());
	}

	void DrawDistance(int iEntIndex)
	{
		float flDistance = floor(g_Local->m_vecOrigin.DistTo(g_Player[iEntIndex]->m_vecOrigin));
		flDistance -= g_Player[iEntIndex]->m_vecBBMaxs.y + g_Local->m_vecBBMaxs.y;
		if (cvar.visuals.esp_player_distance_measure)
			ConvertDistanceToMeters(&flDistance);

		float x, y, spacing = 2.f;
		float nextline = -1.f;

		if (cvar.visuals.esp_player_distance == cvar.visuals.esp_player_health)
			spacing += GetBarSpacing() + GetBarThickness();

		if (cvar.visuals.esp_player_distance == cvar.visuals.esp_player_armor && g_Player[iEntIndex]->m_iArmorValue)
			spacing += GetBarSpacing() + GetBarThickness();

		if (cvar.visuals.esp_player_distance == cvar.visuals.esp_player_name)
			nextline += fix_size(cvar.visuals.esp_font_size);

		if (cvar.visuals.esp_player_distance == cvar.visuals.esp_player_weapon)
			nextline += fix_size(cvar.visuals.esp_font_size);

		char szDistance[16] = { '\0' };
		sprintf(szDistance, cvar.visuals.esp_player_distance_measure ? "%0.f m" : "%0.f u", flDistance);

		const ImFont * pFont = ImGui::GetIO().Fonts->Fonts[0];
		const ImVec2   text_size = pFont->CalcTextSizeA(cvar.visuals.esp_font_size, FLT_MAX, 0, szDistance);

		switch (cvar.visuals.esp_player_distance)
		{
		case LEFT:
			x = m_Rect.Min.x - spacing - text_size.x;
			y = m_Rect.Min.y + nextline;
			break;
		case RIGHT:
			x = m_Rect.Max.x + spacing;
			y = m_Rect.Min.y + nextline;
			break;
		case TOP:
			x = m_Rect.Min.x + width() * 0.5f;
			y = m_Rect.Min.y - spacing - cvar.visuals.esp_font_size - nextline;
			break;
		case BOT:
			x = m_Rect.Min.x + width() * 0.5f;
			y = m_Rect.Max.y + spacing + nextline;
		}

		ImColor color = cvar.visuals.esp_font_color;
		ReduceTransparency(iEntIndex, color);
		FadeoutDormant(iEntIndex, color);

		g_Render.AddText(x, y, color, cvar.visuals.esp_font_size,
			GetFlags(cvar.visuals.esp_player_distance), szDistance);
	}

	void DrawBar(int position, float value, const ImVec4 & color, float spacing)
	{
		const float  size = (position <= RIGHT ? height() : width()) / 100.f * value;
		const float  thickness = GetBarThickness();
		const ImVec4 color_background = ImVec4(0.25f, 0.25f, 0.25f, color.w);
		const ImVec4 color_outline = ImVec4(0.00f, 0.00f, 0.00f, color.w);

		if (position == LEFT)
		{
			const float x = m_Rect.Min.x - spacing - thickness;

			g_Render.AddRectFilled(ImVec2(x, m_Rect.Min.y), ImVec2(x + thickness, m_Rect.Max.y), color_background);
			g_Render.AddRectFilled(ImVec2(x, m_Rect.Max.y), ImVec2(x + thickness, m_Rect.Max.y - size), color);

			if (cvar.visuals.esp_player_box_outline)
				g_Render.AddRect(ImVec2(x - 1, m_Rect.Min.y - 1), ImVec2(x + thickness + 1, m_Rect.Max.y + 1), color_outline);
		}
		else if (position == RIGHT)
		{
			const float x = m_Rect.Max.x + spacing;

			g_Render.AddRectFilled(ImVec2(x, m_Rect.Min.y), ImVec2(x + thickness, m_Rect.Max.y), color_background);
			g_Render.AddRectFilled(ImVec2(x, m_Rect.Max.y), ImVec2(x + thickness, m_Rect.Max.y - size), color);

			if (cvar.visuals.esp_player_box_outline)
			{
				g_Render.AddRect(ImVec2(x - thickness * 0.5f, m_Rect.Min.y - 1),
					ImVec2(x + thickness * 0.5f + 2, m_Rect.Max.y + 1), color_outline);
			}
		}
		else if (position == TOP)
		{
			const float y = m_Rect.Min.y - spacing - thickness;

			g_Render.AddRectFilled(ImVec2(m_Rect.Min.x, y), ImVec2(m_Rect.Max.x, y + thickness), color_background);
			g_Render.AddRectFilled(ImVec2(m_Rect.Min.x, y), ImVec2(m_Rect.Min.x + size, y + thickness), color);

			if (cvar.visuals.esp_player_box_outline)
				g_Render.AddRect(ImVec2(m_Rect.Min.x - 1, y - 1), ImVec2(m_Rect.Max.x + 1, y + thickness + 1), color_outline);
		}
		else if (position == BOT)
		{
			const auto y = m_Rect.Max.y + spacing;

			g_Render.AddRectFilled(ImVec2(m_Rect.Min.x, y), ImVec2(m_Rect.Max.x, y + thickness), color_background);
			g_Render.AddRectFilled(ImVec2(m_Rect.Min.x, y), ImVec2(m_Rect.Min.x + size, y + thickness), color);

			if (cvar.visuals.esp_player_box_outline)
				g_Render.AddRect(ImVec2(m_Rect.Min.x - 1, y - 1), ImVec2(m_Rect.Max.x + 1, y + thickness + 1), color_outline);
		}
	}

	void DrawHealth(int iEntIndex)
	{
		const float health = static_cast<float>(g_Player[iEntIndex]->m_iHealth);
		ImColor color = ImColor(
			health <= 50.f ? 1.f : fInterp(50.f, health, 100.f, 1.f, 0.f),
			health >= 50.f ? 1.f : fInterp(0.f, health, 50.f, 0.f, 1.f),
			0.00f);
		ReduceTransparency(iEntIndex, color);
		FadeoutDormant(iEntIndex, color);
		DrawBar(cvar.visuals.esp_player_health, health, color, GetBarSpacing());
	}

	void DrawArmor(int iEntIndex)
	{
		const float armor = static_cast<float>(g_Player[iEntIndex]->m_iArmorValue);
		ImColor color = ImColor(0.75f, 0.75f, 1.00f);
		ReduceTransparency(iEntIndex, color);
		FadeoutDormant(iEntIndex, color);

		float spacing = GetBarSpacing();
		if (cvar.visuals.esp_player_armor == cvar.visuals.esp_player_health)
			spacing += spacing + GetBarThickness();

		DrawBar(cvar.visuals.esp_player_armor, armor, color, spacing);
	}

	void DrawSkeleton(int iEntIndex)
	{
		C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex(iEntIndex);
		studiohdr_t* pStudioModel = g_pIModelInfo->GetStudiomodel(pPlayer->GetModel());

		if (!pStudioModel)
			return;

		static matrix3x4_t pBoneToWorldOut[128];
		if (!pPlayer->SetupBones(pBoneToWorldOut, IM_ARRAYSIZE(pBoneToWorldOut), 156, 0.f))
			return;

		ImColor color = cvar.visuals.esp_player_skeleton_color;
		ReduceTransparency(iEntIndex, color);
		FadeoutDormant(iEntIndex, color);

		for (int i = 0; i < pStudioModel->numbones; i++)
		{
			mstudiobone_t* pBone = pStudioModel->GetBone(i);
			if (!pBone || ~pBone->flags & 256 || pBone->parent == -1)
				continue;

			float flBonePos[2][2];

			if (!g_Utils.WorldToScreen(
				Vector(
					pBoneToWorldOut[i][0][3],
					pBoneToWorldOut[i][1][3],
					pBoneToWorldOut[i][2][3]), flBonePos[0]))
				continue;

			if (!g_Utils.WorldToScreen(
				Vector(
					pBoneToWorldOut[pBone->parent][0][3],
					pBoneToWorldOut[pBone->parent][1][3],
					pBoneToWorldOut[pBone->parent][2][3]), flBonePos[1]))
				continue;

			g_Render.AddLine(ImVec2(flBonePos[0][0], flBonePos[0][1]), ImVec2(flBonePos[1][0], flBonePos[1][1]), color);
		}
	}

	template<typename T>
	void GetScreenSize(T(&ScreenSize)[2])
	{
		int iScreenSize[2];
		g_pIEngine->GetScreenSize(iScreenSize[0], iScreenSize[1]);
		for (int i = 0; i < IM_ARRAYSIZE(ScreenSize); i++)
			ScreenSize[i] = static_cast<T>(iScreenSize[i]);
	}

	template<typename T>
	void GetScreenCenter(T(&ScreenCenter)[2])
	{
		T ScreenSize[2];
		GetScreenSize(ScreenSize);
		for (int i = 0; i < IM_ARRAYSIZE(ScreenCenter); i++)
			ScreenCenter[i] = ScreenSize[i] / 2;
	}

	void FindPoint(float(&point)[2])
	{
		float flScreenCenter[2];
		GetScreenCenter<float>(flScreenCenter);

		float d = 0.f;
		for (int i = 0; i < IM_ARRAYSIZE(flScreenCenter); i++)
			d += pow((point[i] - flScreenCenter[i]), 2);

		const float r = flScreenCenter[1] / sqrt(d);
		for (int i = 0; i < IM_ARRAYSIZE(point); i++)
			point[i] = r * point[i] + (1.f - r) * flScreenCenter[i];
	}

	bool WorldToScreen(const Vector & vecOrigin, float(&flScreenPos)[2])
	{
		const bool ret = g_Utils.WorldToScreen(vecOrigin, flScreenPos);

		float flScreenSize[2];
		GetScreenSize<float>(flScreenSize);

		for (int i = 0; i < IM_ARRAYSIZE(flScreenPos); i++)
			flScreenPos[i] = flScreenSize[i] * 0.5f + 0.5f * flScreenPos[i] * flScreenSize[i];

		FindPoint(flScreenPos);
		return ret;
	}

	void OutPovScreen(int iEntIndex)
	{
		float flScreenPos[2];
		if (WorldToScreen(g_Player[iEntIndex]->m_vecHitbox[HITBOX_STOMACH], flScreenPos))
			return;

		float flScreenCenter[2];
		GetScreenCenter<float>(flScreenCenter);

		QAngle angles;
		Vector vecForward;
		for (int i = 0; i < IM_ARRAYSIZE(flScreenCenter); i++)
			vecForward[i] = flScreenPos[i] - flScreenCenter[i];
		g_Utils.VectorAngles(vecForward, angles);
		angles.Negate();
	
		float flScreenSize[2];
		GetScreenSize<float>(flScreenSize);

		const float radius = (flScreenCenter[1] - 30.f) * cvar.visuals.esp_player_out_of_pov_radius;
		const float yaw = DirectX::XMConvertToRadians(angles.y);
		const float new_point_x = flScreenCenter[0] + radius * cos(yaw);
		const float new_point_y = flScreenCenter[1] + radius * sin(yaw);

		ImColor color = GetTeamColor(iEntIndex);
		ReduceTransparency(iEntIndex, color);
		FadeoutDormant(iEntIndex, color);

		if (cvar.visuals.esp_player_out_of_pov == OutOfPov_Arrows)
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
			const ImFont* pFont = ImGui::GetIO().Fonts->Fonts[0];

			ImColor font_color = cvar.visuals.esp_font_color;
			ReduceTransparency(iEntIndex, color);
			FadeoutDormant(iEntIndex, font_color);

			if (cvar.visuals.esp_player_out_of_pov == OutOfPov_CirclesName)
			{
				player_info_t info;
				g_pIEngine->GetPlayerInfo(iEntIndex, &info);

				if (!strlen(info.szName))
					return;

				const ImVec2 text_size = pFont->CalcTextSizeA(cvar.visuals.esp_font_size, FLT_MAX, 0, info.szName);

				g_Render.AddText(new_point_x, new_point_y + fix_size(cvar.visuals.esp_font_size),
					font_color, cvar.visuals.esp_font_size, GetFlags(BOT), u8"%s", info.szName);
			}
			else if (cvar.visuals.esp_player_out_of_pov == OutOfPov_CirclesWeapon)
			{
				if (!g_Player[iEntIndex]->m_hActiveWeapon || !g_Player[iEntIndex]->m_CSWeaponData)
					return;

				const std::string sWeaponInfo = GetWeaponNameByIndex(g_Player[iEntIndex]->m_iWeaponId);
				const ImVec2 text_size = pFont->CalcTextSizeA(cvar.visuals.esp_font_size, FLT_MAX, 0, sWeaponInfo.c_str());

				g_Render.AddText(new_point_x, new_point_y + fix_size(cvar.visuals.esp_font_size),
					font_color, cvar.visuals.esp_font_size, GetFlags(BOT), sWeaponInfo.c_str());
			}
			else if (cvar.visuals.esp_player_out_of_pov == OutOfPov_CirclesDistance)
			{
				float flDistance = floor(g_Local->m_vecOrigin.DistTo(g_Player[iEntIndex]->m_vecOrigin));
				flDistance -= g_Player[iEntIndex]->m_vecBBMaxs.y + g_Local->m_vecBBMaxs.y;
				if (cvar.visuals.esp_player_distance_measure)
					ConvertDistanceToMeters(&flDistance);

				char szDistance[16] = { '\0' };
				sprintf(szDistance, cvar.visuals.esp_player_distance_measure ? "%0.f m" : "%0.f u", flDistance);

				const ImVec2 text_size = pFont->CalcTextSizeA(cvar.visuals.esp_font_size, FLT_MAX, 0, szDistance);

				g_Render.AddText(new_point_x, new_point_y + fix_size(cvar.visuals.esp_font_size),
					font_color, cvar.visuals.esp_font_size, GetFlags(BOT), szDistance);
			}

			g_Render.AddCircleFilled(ImVec2(new_point_x, new_point_y), 8.f, color);
		}
	}

	FontFlag GetFlags(int position)
	{
		FontFlag flags = GetFontFlag(cvar.visuals.esp_font_outline);
		return static_cast<FontFlag>(position > RIGHT ? (flags | FontFlag::CENTER_X) : flags);
	}

	void ConvertDistanceToMeters(float* flDistance)
	{
		*flDistance *= 0.025f;
	}

	float GetBarSpacing() { return cvar.visuals.esp_player_box_outline ? 3.f : 2.f; }
	float GetBarThickness() { return 2.f; }

	float width() { return m_Rect.Max.x - m_Rect.Min.x; }
	float height() { return m_Rect.Max.y - m_Rect.Min.y; }

	float fix_size(float size) { return size - 1; }
};

class CESP
{
public:
	void Draw()
	{
		if (cvar.visuals.esp_player_enabled)
		{
			const auto& selections = GetSelectionPlayers();

			if (!selections.empty())
			{
				for (auto& selection : selections)
				{
					static CDrawPlayer player;
					player.Draw(selection.nIndex);
				}
			}
		}
	}

private:
	class CSelection
	{
	public:
		float	flDistance;
		int		nIndex;
	};

	std::deque<CSelection>& SortSelection(const std::deque<CSelection>& selections)
	{
		static std::deque<CSelection> sorted;

		if (!sorted.empty())
			sorted.clear();

		for (size_t nStartIndex = 0; nStartIndex < selections.size(); nStartIndex++)
		{
			float flMaxDistance = 0.f;
			int   nSortIndex = 0;

			for (size_t nCurrentIndex = 0; nCurrentIndex < selections.size(); nCurrentIndex++)
			{
				bool bSkipIndex = false;

				if (!sorted.empty())
				{
					for (size_t i = 0; i < sorted.size(); i++)
					{
						if (sorted[i].nIndex == selections[nCurrentIndex].nIndex)
							bSkipIndex = true;
					}

					if (bSkipIndex)
						continue;
				}

				if (selections[nCurrentIndex].flDistance > flMaxDistance)
				{
					flMaxDistance = selections[nCurrentIndex].flDistance;
					nSortIndex = selections[nCurrentIndex].nIndex;
				}
			}

			CSelection selection;
			selection.flDistance = flMaxDistance;
			selection.nIndex = nSortIndex;
			sorted.push_back(selection);
		}

		return sorted;
	}

	void PushSelectionPlayer(std::deque<CSelection> & selections, int i)
	{
		if (g_Local->m_iEntIndex == i)
			return;

		C_BasePlayer * pBasePlayer = C_BasePlayer::GetPlayerByIndex(i);

		if (!pBasePlayer)
			return;

		if (g_Player[i]->m_IsDead)
			return;

		if (!cvar.visuals.esp_player_teammates && g_Player[i]->m_iTeamNum == g_Local->m_iTeamNum)
			return;

		if (g_Local->m_IsDead)
		{
			if (g_Local->m_iObserverMode != OBS_MODE_ROAMING &&
				g_Local->m_hObserverTarget == C_BasePlayer::GetPlayerByIndex(i))
				return;
		}

		if (g_Player[i]->m_IsDormant)
		{
			if (g_pGlobalVars->curtime - g_Player[i]->m_flHistory > cvar.visuals.esp_player_history_time)
				return;
		}

		CSelection selection;
		selection.flDistance = g_Local->m_vecOrigin.DistTo(g_Player[i]->m_vecOrigin);
		selection.nIndex = i;
		selections.push_back(selection);
	}

	std::deque<CSelection>& GetSelectionPlayers()
	{
		static std::deque<CSelection> selections;

		if (!selections.empty())
			selections.clear();

		for (int i = 1; i <= g_pIEngine->GetMaxClients(); i++)
			PushSelectionPlayer(selections, i);

		return SortSelection(selections);
	}
};

void CVisuals::Run()
{
	static CESP esp;
	esp.Draw();
}