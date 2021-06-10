#include "render_drawlist.h"

#include "..\imgui\imgui_internal.h"

CDrawList::CDrawList() : m_pDrawList(nullptr)
{
}

CDrawList::~CDrawList()
{
}

void CDrawList::AddText(float x, float y, const ImColor& col, float size, FontFlag flags, const char* fmt, ...)
{
	if (!fmt || !strlen(fmt))
		return;

	char buf[256] = { '\0' };
	va_list vlist = nullptr;
	va_start(vlist, fmt);
	vsprintf_s(buf, fmt, vlist);
	va_end(vlist);

	static const auto& io = ImGui::GetIO();
	static const auto* const pFont = io.Fonts->Fonts[0];

	m_pDrawList->PushTextureID(io.Fonts->TexID);

	if (size == 0.f)
		size = pFont->FontSize;

	const auto text_size = pFont->CalcTextSizeA(size, FLT_MAX, 0, buf);
	const auto col_black = ImColor(0.00f, 0.00f, 0.00f, col.Value.w);
	const auto col_u32_black = ImGui::ColorConvertFloat4ToU32(col_black);

	if (flags & CENTER_X)
		x -= text_size.x * 0.5f;

	if (flags & CENTER_Y)
		y -= text_size.y * 0.5f;

	if (flags & SHADOW)
	{
		m_pDrawList->AddText(pFont, size, ImVec2(x + 1, y + 1), col_u32_black, buf);
	}
	else if (flags & OUTLINE)
	{
		m_pDrawList->AddText(pFont, size, ImVec2(x, y - 1), col_u32_black, buf);
		m_pDrawList->AddText(pFont, size, ImVec2(x, y + 1), col_u32_black, buf);
		m_pDrawList->AddText(pFont, size, ImVec2(x + 1, y), col_u32_black, buf);
		m_pDrawList->AddText(pFont, size, ImVec2(x - 1, y), col_u32_black, buf);

		m_pDrawList->AddText(pFont, size, ImVec2(x - 1, y - 1), col_u32_black, buf);
		m_pDrawList->AddText(pFont, size, ImVec2(x + 1, y - 1), col_u32_black, buf);
		m_pDrawList->AddText(pFont, size, ImVec2(x - 1, y + 1), col_u32_black, buf);
		m_pDrawList->AddText(pFont, size, ImVec2(x + 1, y + 1), col_u32_black, buf);
	}

	if (flags & UNDERLINE)
		AddLine(ImVec2(x, y + text_size.y), ImVec2(x + text_size.x, y + text_size.y), col_black);

	const auto col_u32 = ImGui::ColorConvertFloat4ToU32(col);

	m_pDrawList->AddText(pFont, size, ImVec2(x, y), col_u32, buf);
	m_pDrawList->PopTextureID();
}

void CDrawList::AddText(float x, float y, const ImColor& col, const char* fmt, ...)
{
	if (!fmt || !strlen(fmt))
		return;

	char buf[256] = { '\0' };
	va_list vlist = nullptr;
	va_start(vlist, fmt);
	vsprintf_s(buf, fmt, vlist);
	va_end(vlist);

	AddText(x, y, col, 0, NONE, buf);
}

void CDrawList::AddTextWrapped(float x, float y, const ImColor& col, float size, float wrap_width, FontFlag flags, const char* fmt, ...)
{
	if (!fmt || !strlen(fmt))
		return;

	char buf[256] = { '\0' };
	va_list vlist = nullptr;
	va_start(vlist, fmt);
	vsprintf_s(buf, fmt, vlist);
	va_end(vlist);

	static const auto& io = ImGui::GetIO();
	static const auto* const pFont = io.Fonts->Fonts[0];

	m_pDrawList->PushTextureID(io.Fonts->TexID);

	if (size == 0.f)
		size = pFont->FontSize;

	const auto text_size = pFont->CalcTextSizeA(size, FLT_MAX, NULL, buf);
	const auto col_black = ImColor(0.00f, 0.00f, 0.00f, col.Value.w);
	const auto col_u32_black = ImGui::ColorConvertFloat4ToU32(col_black);

	if (flags & CENTER_X)
		x -= text_size.x * 0.5f;

	if (flags & CENTER_Y)
		y -= text_size.y * 0.5f;

	if (flags & SHADOW)
	{
		m_pDrawList->AddText(pFont, size, ImVec2(x + 1, y + 1), col_u32_black, buf, 0, wrap_width);
	}
	else if (flags & OUTLINE)
	{
		m_pDrawList->AddText(pFont, size, ImVec2(x, y - 1), col_u32_black, buf, 0, wrap_width);
		m_pDrawList->AddText(pFont, size, ImVec2(x, y + 1), col_u32_black, buf, 0, wrap_width);
		m_pDrawList->AddText(pFont, size, ImVec2(x + 1, y), col_u32_black, buf, 0, wrap_width);
		m_pDrawList->AddText(pFont, size, ImVec2(x - 1, y), col_u32_black, buf, 0, wrap_width);

		m_pDrawList->AddText(pFont, size, ImVec2(x - 1, y - 1), col_u32_black, buf, 0, wrap_width);
		m_pDrawList->AddText(pFont, size, ImVec2(x + 1, y - 1), col_u32_black, buf, 0, wrap_width);
		m_pDrawList->AddText(pFont, size, ImVec2(x - 1, y + 1), col_u32_black, buf, 0, wrap_width);
		m_pDrawList->AddText(pFont, size, ImVec2(x + 1, y + 1), col_u32_black, buf, 0, wrap_width);
	}

	if (flags & UNDERLINE)
		AddLine(ImVec2(x, y + text_size.y), ImVec2(x + text_size.x, y + text_size.y), col_black);

	const auto col_u32 = ImGui::ColorConvertFloat4ToU32(col);

	m_pDrawList->AddText(pFont, size, ImVec2(x, y), col_u32, buf, 0, wrap_width);
	m_pDrawList->PopTextureID();
}

void CDrawList::AddTextWrapped(float x, float y, const ImColor& col, float wrap_width, const char* fmt, ...)
{
	if (!fmt || !strlen(fmt))
		return;

	char buf[256] = { '\0' };
	va_list vlist = nullptr;
	va_start(vlist, fmt);
	vsprintf_s(buf, fmt, vlist);
	va_end(vlist);

	AddTextWrapped(x, y, col, 0, wrap_width, NONE, buf);
}

void CDrawList::AddLine(const ImVec2& a, const ImVec2& b, const ImColor& col, float thickness)
{
	const auto col_u32 = ImGui::ColorConvertFloat4ToU32(col);

	if ((col_u32 & IM_COL32_A_MASK) == 0)
		return;

	m_pDrawList->PathLineTo(a + 0.5f);
	m_pDrawList->PathLineTo(b + 0.5f);
	m_pDrawList->PathStroke(col_u32, false, thickness);
}

void CDrawList::AddRect(const ImVec2& a, const ImVec2& b, const ImColor& col, 
	bool outline, float rounding, int rounding_corners_flags, float thickness)
{
	static auto Rect = [&](const ImVec2 & a, const ImVec2 & b, const ImU32 & col, float rounding, int rounding_corners_flags)
	{
		m_pDrawList->PathRect(a + 0.5f, b - 0.49f, rounding, rounding_corners_flags);
		m_pDrawList->PathStroke(col, true, 1.f);
	};

	const auto col_u32 = ImGui::ColorConvertFloat4ToU32(col);

	if ((col_u32 & IM_COL32_A_MASK) == 0)
		return;

	if (outline)
	{
		const auto col_outline = ImColor(0.00f, 0.00f, 0.00f, col.Value.w);
		
		AddRect(a - 1, b + 1, col_outline, false, rounding, rounding_corners_flags, thickness);
		AddRect(a + 1, b - 1, col_outline, false, rounding, rounding_corners_flags, thickness);
	}

	for (int i = 0, pos_offset = 0; i < static_cast<int>(thickness); i++)
	{
		if (i == 0 || i % 2 == 0)
		{
			Rect(a + pos_offset, b - pos_offset, col_u32, rounding, rounding_corners_flags);
		}
		else
		{
			pos_offset++;
			Rect(a - pos_offset, b + pos_offset, col_u32, rounding, rounding_corners_flags);
		}
	}
}

void CDrawList::AddRectCorner(const ImVec2& a, const ImVec2& b, const ImColor& col, bool outline, float thickness)
{
	static auto RectCorner = [&](const ImVec2& a, const ImVec2& b, const ImColor& col, float align)
	{
		const auto w = (b.x - a.x) * 0.25f + align;
		const auto h = (b.y - a.y) * 0.25f + align;

		AddLine(a, ImVec2(a.x + w, a.y), col);
		AddLine(a, ImVec2(a.x, a.y + h), col);

		AddLine(ImVec2(b.x - 1, a.y), ImVec2(b.x - 1 - w, a.y), col);
		AddLine(ImVec2(b.x - 1, a.y), ImVec2(b.x - 1, a.y + h), col);

		AddLine(ImVec2(a.x, b.y - 1), ImVec2(a.x + w, b.y - 1), col);
		AddLine(ImVec2(a.x, b.y - 1), ImVec2(a.x, b.y - 1 - h), col);

		AddLine(ImVec2(b.x - 1, b.y - 1), ImVec2(b.x - 1, b.y - 1 - h), col);
		AddLine(ImVec2(b.x - 1, b.y - 1), ImVec2(b.x - 1 - w, b.y - 1), col);
	};
	
	if (outline)
	{
		const auto col_outline = ImColor(0.00f, 0.00f, 0.00f, col.Value.w);

		AddRectCorner(ImVec2(a.x - 1, a.y + 1), ImVec2(b.x - 1, b.y + 1), col_outline, false, thickness);
		AddRectCorner(ImVec2(a.x - 1, a.y - 1), ImVec2(b.x - 1, b.y - 1), col_outline, false, thickness);
		AddRectCorner(ImVec2(a.x + 1, a.y + 1), ImVec2(b.x + 1, b.y + 1), col_outline, false, thickness);
		AddRectCorner(ImVec2(a.x + 1, a.y - 1), ImVec2(b.x + 1, b.y - 1), col_outline, false, thickness);

		AddRectCorner(ImVec2(a.x, a.y - 1), ImVec2(b.x, b.y - 1), col_outline, false, thickness);
		AddRectCorner(ImVec2(a.x - 1, a.y), ImVec2(b.x - 1, b.y), col_outline, false, thickness);
		AddRectCorner(ImVec2(a.x, a.y + 1), ImVec2(b.x, b.y + 1), col_outline, false, thickness);
		AddRectCorner(ImVec2(a.x + 1, a.y), ImVec2(b.x + 1, b.y), col_outline, false, thickness);
	}

	for (int i = 0, pos_offset = 0; i < static_cast<int>(thickness); i++)
	{
		if (i == 0 || i % 2 == 0)
		{
			const auto align = static_cast<float>(pos_offset) * (0.75f * (0.8f * static_cast<float>(pos_offset)));
			RectCorner(a + pos_offset, b - pos_offset, col, -align);
		}
		else
		{
			pos_offset++;
			const auto align = static_cast<float>(pos_offset) * (0.75f * (0.8f * static_cast<float>(pos_offset)));
			RectCorner(a - pos_offset, b + pos_offset, col, align);
		}
	}
}

void CDrawList::AddRectFilled(const ImVec2& a, const ImVec2& b, const ImColor& col, float rounding, int rounding_corners_flags)
{
	const auto col_u32 = ImGui::ColorConvertFloat4ToU32(col);

	if ((col_u32 & IM_COL32_A_MASK) == 0)
		return;

	if (rounding > 0.f)
	{
		m_pDrawList->PathRect(a, b, rounding, rounding_corners_flags);
		m_pDrawList->PathFillConvex(col_u32);
	}
	else
	{
		m_pDrawList->PrimReserve(6, 4);
		m_pDrawList->PrimRect(a, b, col_u32);
	}
}

void CDrawList::AddRectFilledMultiColor(const ImVec2& a, const ImVec2& b, const ImColor& col_upr_left,
	const ImColor& col_upr_right, const ImColor& col_bot_right, const ImColor& col_bot_left)
{
	const auto col_u32_upr_left = ImGui::ColorConvertFloat4ToU32(col_upr_left);
	const auto col_u32_upr_right = ImGui::ColorConvertFloat4ToU32(col_upr_right);

	const auto col_u32_bot_left = ImGui::ColorConvertFloat4ToU32(col_bot_left);
	const auto col_u32_bot_right = ImGui::ColorConvertFloat4ToU32(col_bot_right);

	m_pDrawList->AddRectFilledMultiColor(a, b, col_u32_upr_left, col_u32_upr_right, col_u32_bot_left, col_u32_bot_right);
}

void CDrawList::AddQuad(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImColor& col, float thickness)
{
	const auto col_u32 = ImGui::ColorConvertFloat4ToU32(col);

	if ((col_u32 & IM_COL32_A_MASK) == 0)
		return;

	m_pDrawList->PathLineTo(a);
	m_pDrawList->PathLineTo(b);
	m_pDrawList->PathLineTo(c);
	m_pDrawList->PathLineTo(d);
	m_pDrawList->PathStroke(col_u32, true, thickness);
}

void CDrawList::AddQuadFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImColor& col)
{
	const auto col_u32 = ImGui::ColorConvertFloat4ToU32(col);

	if ((col_u32 & IM_COL32_A_MASK) == 0)
		return;

	m_pDrawList->PathLineTo(a);
	m_pDrawList->PathLineTo(b);
	m_pDrawList->PathLineTo(c);
	m_pDrawList->PathLineTo(d);
	m_pDrawList->PathFillConvex(col_u32);
}

void CDrawList::AddTriangle(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImColor& col, float thickness)
{
	const auto col_u32 = ImGui::ColorConvertFloat4ToU32(col);

	if ((col_u32 & IM_COL32_A_MASK) == 0)
		return;

	m_pDrawList->PathLineTo(a);
	m_pDrawList->PathLineTo(b);
	m_pDrawList->PathLineTo(c);
	m_pDrawList->PathStroke(col_u32, true, thickness);
}

void CDrawList::AddTriangleFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImColor& col)
{
	const auto col_u32 = ImGui::ColorConvertFloat4ToU32(col);

	if ((col_u32 & IM_COL32_A_MASK) == 0)
		return;

	m_pDrawList->PathLineTo(a);
	m_pDrawList->PathLineTo(b);
	m_pDrawList->PathLineTo(c);
	m_pDrawList->PathFillConvex(col_u32);
}

void CDrawList::AddCircle(const ImVec2& centre, float radius, const ImColor& col, int num_segments, float thickness)
{
	const auto col_u32 = ImGui::ColorConvertFloat4ToU32(col);

	if ((col_u32 & IM_COL32_A_MASK) == 0 || num_segments <= 2)
		return;

	// Because we are filling a closed shape we remove 1 from the count of segments/points
	const auto a_max = IM_PI * 2.f * (static_cast<float>(num_segments) - 1.f) / static_cast<float>(num_segments);
	m_pDrawList->PathArcTo(centre, radius - 0.5f, 0.f, a_max, num_segments - 1);
	m_pDrawList->PathStroke(col_u32, true, thickness);
}

void CDrawList::AddCircleFilled(const ImVec2& centre, float radius, const ImColor& col, int num_segments)
{
	const auto col_u32 = ImGui::ColorConvertFloat4ToU32(col);

	if ((col_u32 & IM_COL32_A_MASK) == 0 || num_segments <= 2)
		return;

	// Because we are filling a closed shape we remove 1 from the count of segments/points
	const auto a_max = IM_PI * 2.f * (static_cast<float>(num_segments) - 1.f) / static_cast<float>(num_segments);
	m_pDrawList->PathArcTo(centre, radius, 0.f, a_max, num_segments - 1);
	m_pDrawList->PathFillConvex(col_u32);
}

void CDrawList::AddImage(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a, const ImVec2& uv_b, ImU32 col)
{
	m_pDrawList->AddImage(user_texture_id, a, b, uv_a, uv_b, col);
}

void CDrawList::AddImageQuad(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& c, 
	const ImVec2& d, const ImVec2& uv_a, const ImVec2& uv_b, const ImVec2& uv_c, const ImVec2& uv_d, ImU32 col)
{
	m_pDrawList->AddImageQuad(user_texture_id, a, b, c, d, uv_a, uv_b, uv_c, uv_d, col);
}

void CDrawList::AddImageRounded(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b,
	const ImVec2& uv_a, const ImVec2& uv_b, ImU32 col, float rounding, int rounding_corners)
{
	m_pDrawList->AddImageRounded(user_texture_id, a, b, uv_a, uv_b, col, rounding, rounding_corners);
}

void CDrawList::AddPolyline(const ImVec2* points, int num_points, const ImColor& col, bool closed, float thickness)
{
	const auto col_u32 = ImGui::ColorConvertFloat4ToU32(col);
	m_pDrawList->AddPolyline(points, num_points, col_u32, closed, thickness);
}

void CDrawList::AddConvexPolyFilled(const ImVec2* points, int num_points, const ImColor& col)
{
	const auto col_u32 = ImGui::ColorConvertFloat4ToU32(col);
	m_pDrawList->AddConvexPolyFilled(points, num_points, col_u32);
}

void CDrawList::AddBezierCurve(const ImVec2& pos0, const ImVec2& cp0, const ImVec2& cp1,
	const ImVec2& pos1, const ImColor& col, float thickness, int num_segments)
{
	const auto col_u32 = ImGui::ColorConvertFloat4ToU32(col);

	if ((col_u32 & IM_COL32_A_MASK) == 0)
		return;

	m_pDrawList->PathLineTo(pos0);
	m_pDrawList->PathBezierCurveTo(cp0, cp1, pos1, num_segments);
	m_pDrawList->PathStroke(col_u32, false, thickness);
}