#include "esp_player_selection.h"

#include "..\..\..\..\imgui\imgui.h"
#include "..\..\..\..\imgui\imgui_internal.h"
#include "..\..\..\..\render\render.h"

class CDrawPlayer : public CSelectionPlayers
{
public:
	CDrawPlayer();
	~CDrawPlayer();

protected:
	void DrawPlayer();

private:
	enum BoxTypesList
	{
		BoxType_Default = 1,
		BoxType_Corner,
		BoxType_Round
	};

	enum Positions
	{
		Pos_Left = 1,
		Pos_Right,
		Pos_Top,
		Pos_Bot
	};

	enum OutPovList
	{
		Arrows = 1,
		CirclesName,
		CirclesWeapon,
		CirclesDistance
	};

	ImRect m_Rect;

	float InterpolateFadeout(int index, float alpha);
	void ReduceTransparency(int index, ImColor& color);
	void FadeoutDormant(int index, ImColor& color);

	ImColor GetTeamColor(int index);
	ImRect GetRect(float* flScreenTop, float* flScreenBot);

	ImVec2 GetTextSize(const char* text);
	FontFlag GetFlags(int position);

	float GetHeight();
	float GetWidth();

	float GetAlignOffset();
	float GetSpacingBar();
	float GetSpacingText(int index, int pos);
	float GetNextlineText(const ImVec2& text_size, int& pos);
	ImVec2 GetPositionText(float spacing, float nextline, const ImVec2& text_size, int pos);
	ImColor GetProcessedColor(int index, ImColor color);

	bool IsVisible(int index);
	bool CalcScreen(int index);

	void BoundBox(int index);
	void DrawBar(int pos, float value, const ImVec4& color, float spacing);
	void HealthBar(int index);
	void ArmorBar(int index);
	void NickName(int index);
	void WeaponName(int index);
	void Distance(int index);
	void Skeleton(int index);
	void OutPovScreen(int index);
};