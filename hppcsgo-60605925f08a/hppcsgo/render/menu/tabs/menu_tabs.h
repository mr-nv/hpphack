#include "..\popup\menu_popup_modals.h"
#include "..\themes\menu_themes_list.h"

#define IM_ARRAYCOMBO(arr) arr, IM_ARRAYSIZE(arr)

class CMenuTabs :
	public CMenuPopupModals, // CMenuDrawingElements, CMenuSettingsList, CMenuHotkeys
	public CMenuThemesList
{
public:
	CMenuTabs();
	~CMenuTabs();

protected:
	void TabRageBot();
	void TabLegitBot();
	void TabVisuals();
	void TabKreedz();
	void TabMisc();
	void TabGUI();
	void TabSettings();

	void ResetCurrentWeaponId();
	void GetCurrentWeaponId();

private:
	enum CSWeaponGroup
	{
		WEAPONGROUP_PISTOL,
		WEAPONGROUP_SUBMACHINEGUN,
		WEAPONGROUP_RIFLE,
		WEAPONGROUP_SHOTGUN,
		WEAPONGROUP_SNIPER_RIFLE,
		WEAPONGROUP_MACHINEGUN,
		WEAPONGROUP_MAX_COUNT
	};

	int m_current_weapon[WEAPONGROUP_MAX_COUNT];
	int m_current_weapon_group;
	int m_current_weapon_id;

	int WeaponList(const char* str_id);
	void WeaponCombos();

	float GetChildWidth();
	float GetButtonWidth();
};