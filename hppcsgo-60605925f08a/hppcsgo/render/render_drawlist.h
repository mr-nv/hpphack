#include "..\imgui\imgui.h"

#include <memory>

enum FontFlag : unsigned
{
	NONE,
	SHADOW = 1 << 0,
	OUTLINE = 1 << 2,
	CENTER_X = 1 << 3,
	CENTER_Y = 1 << 4,
	CENTER_XY = CENTER_X | CENTER_Y,
	UNDERLINE = 1 << 5,
};

class CDrawList
{
public:
	CDrawList();
	~CDrawList();

	void AddText(float x, float y, const ImColor& col, float size, FontFlag flags, const char* fmt, ...);
	void AddText(float x, float y, const ImColor& col, const char* fmt, ...);

	void AddTextWrapped(float x, float y, const ImColor& col, float size, float wrap_width, FontFlag flags, const char* fmt, ...);
	void AddTextWrapped(float x, float y, const ImColor& col, float wrap_width, const char* fmt, ...);

	void AddLine(const ImVec2& a, const ImVec2& b, const ImColor& col, float thickness = 1.f);
	
	void AddRect(const ImVec2& a, const ImVec2& b, const ImColor& col, bool outline = false, float rounding = 0.f, int rounding_corners_flags = ImDrawCornerFlags_All, float thickness = 1.f);
	void AddRectCorner(const ImVec2& a, const ImVec2& b, const ImColor& col, bool outline = false, float thickness = 1.f);
	void AddRectFilled(const ImVec2& a, const ImVec2& b, const ImColor& col, float rounding = 0.f, int rounding_corners_flags = ImDrawCornerFlags_All);
	void AddRectFilledMultiColor(const ImVec2& a, const ImVec2& b, const ImColor& col_upr_left, const ImColor& col_upr_right, const ImColor& col_bot_right, const ImColor& col_bot_left);
	
	void AddQuad(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImColor& col, float thickness = 1.f);
	void AddQuadFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImColor& col);
	
	void AddTriangle(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImColor& col, float thickness = 1.f);
	void AddTriangleFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImColor& col);
	
	void AddCircle(const ImVec2& centre, float radius, const ImColor& col, int num_segments = 12, float thickness = 1.f);
	void AddCircleFilled(const ImVec2& centre, float radius, const ImColor& col, int num_segments = 12);
	
	void AddImage(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a = ImVec2(0, 0), const ImVec2& uv_b = ImVec2(1, 1), ImU32 col = 0xFFFFFFFF);
	void AddImageQuad(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImVec2& uv_a = ImVec2(0, 0), const ImVec2& uv_b = ImVec2(1, 0), const ImVec2& uv_c = ImVec2(1, 1), const ImVec2& uv_d = ImVec2(0, 1), ImU32 col = 0xFFFFFFFF);
	void AddImageRounded(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a, const ImVec2& uv_b, ImU32 col, float rounding, int rounding_corners = ImDrawCornerFlags_All);
	
	void AddPolyline(const ImVec2* points, int num_points, const ImColor& col, bool closed, float thickness = 1.f);
	void AddConvexPolyFilled(const ImVec2* points, int num_points, const ImColor& col);
	void AddBezierCurve(const ImVec2& pos0, const ImVec2& cp0, const ImVec2& cp1, const ImVec2& pos1, const ImColor& col, float thickness, int num_segments);

protected:
	std::shared_ptr<ImDrawList> m_pDrawList;
};