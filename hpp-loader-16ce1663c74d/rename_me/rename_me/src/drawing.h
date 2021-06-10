#pragma once

enum ThemesList
{
	WhiteTheme,
	DarkTheme
};

struct DrawSettings
{
	bool modal_opened;
	int themenum;
	int rounding;
};

class CDrawing
{
public:
	CDrawing();
	~CDrawing();

	DrawSettings& GetDrawSettings();

	void InitPageList();
	void Run();

private:
	std::unique_ptr<CPageList> m_page_list;
	DrawSettings m_draw_settings;
};