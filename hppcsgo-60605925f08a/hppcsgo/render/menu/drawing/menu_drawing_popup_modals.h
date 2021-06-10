#include "..\..\..\imgui\imgui.h"

class CMenuDrawingPopupModals
{
public:
	CMenuDrawingPopupModals();
	~CMenuDrawingPopupModals();

protected:
	bool BeginPopupModal(const char* name, ImVec2 size);
	void PopupModalMessage(const char* message);
	void PopupModalInput(const char* name, char* buf, size_t buf_size);
	bool PopupModalButton(const char* name);
	void EndPopupModal();

private:
	ImVec2 m_popup_modal_button_size;

	float GetItemWidth();
};