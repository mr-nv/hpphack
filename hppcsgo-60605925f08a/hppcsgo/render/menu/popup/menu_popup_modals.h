#include "..\drawing\menu_drawing_elements.h"
#include "..\settings\menu_settings_list.h"
#include "..\hotkeys\menu_hotkeys.h"

class CMenuPopupModals : 
	public CMenuDrawingElements, // CMenuDrawingPopupModals
	public CMenuSettingsList,
	public CMenuHotkeys
{
public:
	CMenuPopupModals();
	~CMenuPopupModals();

protected:
	void DialogSettingsSaveAs(char* buf, size_t buf_size);
	void DialogSettingsRename(char* buf, size_t buf_size);
	void DialogSettingsRemove();
	void DialogSettingsRestore();
};