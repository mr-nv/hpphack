#include "..\keynum\menu_keynum_assign.h"
#include "menu_drawing_popup_modals.h"

#include <string>
#include <vector>

class CMenuDrawingElements : 
	public CMenuDrawingPopupModals,
	public CMenuKeynumAssign
{
public:
	CMenuDrawingElements();
	~CMenuDrawingElements();

protected:
	void BeginTab();
	void BeginTab(bool* v, const char* label, int* key = nullptr, bool have_on_line = false);
	void EndTab();

	void BeginColumn();
	void NextColumn();
	void EndColumn();

	void BeginChild(const char* label, ImVec2 size);
	void EndChild();

	void PushId(const char* str_id);
	void PushCondition(bool condition);
	void PopCondition(size_t count = 1);

	void Text(const char* fmt, ...);
	bool Checkbox(bool* v, const char* label, const char* msg = 0);
	bool Combo(int* v, const char* label, const char* items[], int size, const char* msg = 0);
	bool SliderFloat(float* v, const char* label, float min, float max, const char* fmt, float power, const char* msg = 0);
	bool SliderInt(int* v, const char* label, int min, int max, const char* fmt, const char* msg = 0);
	bool ListBoxArray(const char* label, int* v, std::vector<std::string>& values, int height_in_items = 1);
	void ColorButton4(float* col, const char* label, float sameline = -1.f);
	bool Button(const char* label, ImVec2 size);
	void Key(int* key, const char* name, bool have_owner = false, bool global_owner = false);

private:
	std::string m_str_id;
	std::vector<bool> m_conditions;

	std::string Base(const char* label, const char* message);

	void TooltipMessage(const char* message);
	void ClampColor(float* col, size_t size);
};