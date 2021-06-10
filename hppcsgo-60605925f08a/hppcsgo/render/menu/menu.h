#include "control\menu_control.h"
#include "fade\menu_fade_background.h"
#include "fade\menu_fade_elements.h"
#include "fade\menu_fade_globals.h"
#include "drawing\menu_drawing.h"
#include "tabs\menu_tabs.h"

class CMenu :
	public CMenuDrawing,
	public CMenuControl,
	public CMenuFadeBackground,
	public CMenuFadeElements,
	public CMenuFadeGlobals,
	public CMenuTabs
{
public:
	CMenu();
	~CMenu();

	CMenu(const CMenu&) = delete;

	void Draw();
	void DrawCursor();

private:
	void ApplyColorsChanges();
};

extern CMenu g_Menu;