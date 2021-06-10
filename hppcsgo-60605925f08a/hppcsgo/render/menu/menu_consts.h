constexpr auto SAMELINE_PADDING_OFFSET = 12.f;

static const char* tab_names[] = 
{ 
	"RageBot", 
	"LegitBot", 
	"Visuals",
	"Kreedz",
	"Misc", 
	"GUI", 
	"Settings" 
};

static const char* settings_files_list[] = 
{ 
	"ragebot.ini", 
	"legitbot.ini", 
	"visuals.ini", 
	"kreedz.ini", 
	"misc.ini",
	"gui.ini" 
};

static const char* weapon_groups[] = 
{ 
	"pistol", 
	"submachinegun", 
	"rifle", 
	"shotgun", 
	"sniper", 
	"machinegun"
};

static const char* weapons_pistol[] = 
{ 
	"deagle", 
	"elite", 
	"fiveseven", 
	"glock", 
	"usp-s", 
	"cz75", 
	"tec9", 
	"p2000", 
	"p250", 
	"revolver" 
};

static const char* weapons_submachinegun[] = 
{ 
	"mac10", 
	"p90", 
	"mp5", 
	"ump45", 
	"bizon", 
	"mp7",
	"mp9" 
};

static const char* weapons_rifle[] = 
{ 
	"ak47", 
	"aug", 
	"famas", 
	"m4a4", 
	"sg553", 
	"galil", 
	"m4a1-s" 
};

static const char* weapons_shotgun[] = 
{ 
	"mag7", 
	"sawedoff", 
	"xm1014",
	"nova" 
};

static const char* weapons_sniper[] = 
{ 
	"awp",
	"g3sg1", 
	"scar20", 
	"ssg08" 
};

static const char* weapons_machinegun[] = 
{ 
	"m249", 
	"negev" 
};

static const char* trigger_accuracy_boost[] = 
{ 
	"None", 
	"Recoil", 
	"Recoil & Spread"
};

static const char* aim_accuracy_boost[] = 
{
	"None", 
	"Recoil", 
	"Recoil & Spread"
};

static const char* esp_player_box[] = 
{ 
	"Off", 
	"Default box", 
	"Corner box", 
	"Rounded box" 
};

static const char* esp_position[] = 
{ 
	"Off", 
	"Left", 
	"Right", 
	"Top", 
	"Bottom" 
};

static const char* esp_player_distance[] = 
{ 
	"In units", 
	"In meters" 
};

static const char* esp_out_of_pow[] = 
{ 
	"Off", 
	"Arrows", 
	"Circles: Name", 
	"Circles: Weapon", 
	"Circles: Distance" 
};

static const char* esp_sound[] =
{
	"Boxes",
	"Circles"
};

static const char* font_outline[] = 
{ 
	"Off", 
	"Shadow", 
	"Outline" 
};

static const char* gui_color_list[] =
{
	"Text",
	"TextDisabled",
	"WindowBg",
	"ChildBg",
	"PopupBg",
	"Border",
	"BorderShadow",
	"FrameBg",
	"FrameBgHovered",
	"FrameBgActive",
	"TitleBg",
	"TitleBgActive",
	"TitleBgCollapsed",
	"MenuBarBg",
	"ScrollbarBg",
	"ScrollbarGrab",
	"ScrollbarGrabHovered",
	"ScrollbarGrabActive",
	"CheckMark",
	"SliderGrab",
	"SliderGrabActive",
	"Button",
	"ButtonHovered",
	"ButtonActive",
	"Header",
	"HeaderHovered",
	"HeaderActive",
	"Separator",
	"SeparatorHovered",
	"SeparatorActive",
	"ResizeGrip",
	"ResizeGripHovered",
	"ResizeGripActive",
	"Tab",
	"TabHovered",
	"TabActive",
	"TabUnfocused",
	"TabUnfocusedActive",
	"PlotLines",
	"PlotLinesHovered",
	"PlotHistogram",
	"PlotHistogramHovered",
	"TextSelectedBg",
	"DragDropTarget",
	"NavHighlight",
	"NavWindowingHighlight",
	"NavWindowingDimBg",
	"ModalWindowDimBg",
};

static const char* color_themes[] =
{
	"Default",
	"Simplified White Red",
	"Simplified Night Red",
	"Simplified White Orange",
	"Simplified Night Orange",
	"Simplified White Cyan",
	"Simplified Night Cyan"
};