class CMenuFadeBackground
{
public:
	CMenuFadeBackground();
	~CMenuFadeBackground();

	void FadeBackgroundInit(bool is_opened);
	void FadeBackgroundRun();

private:
	bool m_IsOpened;
	bool m_IsFadeBackground;

	float m_DurationTime;
	float m_SavedTime;

	float Interpolate(float start, float end);
};