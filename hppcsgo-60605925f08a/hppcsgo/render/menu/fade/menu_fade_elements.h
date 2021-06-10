#include "..\..\..\imgui\imgui.h"

class CMenuFadeElements
{
public:
	CMenuFadeElements();
	~CMenuFadeElements();

protected:
	void FadeElementsInit();
	void FadeElementsRun();

private:
	bool m_IsFadeElements;

	float m_DurationTime;
	float m_SavedTime;

	float m_ColorsAlpha[ImGuiCol_COUNT];

	float Interpolate(float alpha);
};