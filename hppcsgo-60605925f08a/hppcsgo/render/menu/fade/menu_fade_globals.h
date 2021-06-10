class CMenuFadeGlobals
{
public:
	CMenuFadeGlobals();
	~CMenuFadeGlobals();

protected:
	void FadeGlobalsInit(bool is_opened);
	bool FadeGlobalsOpen();
	bool FadeGlobalsClose();

private:
	bool m_IsOpened;
	bool m_IsFadeGlobals;

	float m_DurationTime;
	float m_SavedTime;

	float Interpolate(float start, float end);
	void FadeGlobalsReset();
};