#include "..\menu_consts.h"
#include "menu_popup_modals.h"

#include <Windows.h>
#include <string>
#include <deque>

#include "..\..\..\globals.h"
#include "..\..\..\settings.h"
#include "..\..\..\notifications.h"

CMenuPopupModals::CMenuPopupModals()
{
}

CMenuPopupModals::~CMenuPopupModals()
{
}

void CMenuPopupModals::DialogSettingsSaveAs(char* buf, size_t buf_size)
{
	PopupModalInput("Settings name:", buf, buf_size);
	
	if (PopupModalButton("Save") || IsHotkey("ENTER"))
	{
		if (strlen(buf))
		{
			const auto path_name = g_Globals.m_sSettingsPath + buf;
			const auto result = CreateDirectoryA(path_name.c_str(), 0);

			if (result)
			{
				RefreshSettingsList();
				SetFocusSettings(buf);

				const auto& settings_name = GetFocusSettings();
				g_Settings.Save(settings_name.c_str());
			}
			else
			{
				char title[128];
				sprintf_s(title, "Error saving settings \"%s\".", buf);

				const auto error_code = GetLastError();
				const char* message = nullptr;

				switch (error_code)
				{
				case ERROR_ALREADY_EXISTS:
					message = "The specified directory already exists.";
					break;
				case ERROR_PATH_NOT_FOUND:
					message = "One or more intermediate directories do not exist.";
					break;
				default:
					message = "Could not create directory.";
				}

				g_Notification.Push(title, message);
			}

			RtlSecureZeroMemory(buf, buf_size);
			ImGui::CloseCurrentPopup();
		}
	}

	if (PopupModalButton("Cancel") || IsHotkey("ESC"))
	{
		RtlSecureZeroMemory(buf, buf_size);
		ImGui::CloseCurrentPopup();
	}
}

void CMenuPopupModals::DialogSettingsRename(char* buf, size_t buf_size)
{
	PopupModalInput("Settings name:", buf, buf_size);

	if (PopupModalButton("Rename") || IsHotkey("ENTER"))
	{
		if (strlen(buf) && _strcmpi(buf, "default"))
		{
			const auto path_name_exist = g_Globals.m_sSettingsPath + GetFocusSettings();
			const auto path_name_new = g_Globals.m_sSettingsPath + buf;
			const auto result = MoveFileA(path_name_exist.c_str(), path_name_new.c_str());

			if (result)
			{
				RefreshSettingsList();
				SetFocusSettings(buf);
			}
			else
			{
				char title[128];
				sprintf_s(title, "Error renaming settings \"%s\".", buf);
				g_Notification.Push(title, "Could not rename directory.");
			}
		}

		RtlSecureZeroMemory(buf, buf_size);
		ImGui::CloseCurrentPopup();
	}

	if (PopupModalButton("Cancel") || IsHotkey("ESC"))
	{
		RtlSecureZeroMemory(buf, buf_size);
		ImGui::CloseCurrentPopup();
	}
}

void CMenuPopupModals::DialogSettingsRemove()
{
	PopupModalMessage("Are you sure you want to delete?");

	if (PopupModalButton("Delete") || IsHotkey("ENTER"))
	{
		const auto path_name = g_Globals.m_sSettingsPath + GetFocusSettings();
		g_Settings.Remove(path_name.c_str());

		RefreshSettingsList();
		GetFocusSettings();

		ImGui::CloseCurrentPopup();
	}

	if (PopupModalButton("Cancel") || IsHotkey("ESC"))
		ImGui::CloseCurrentPopup();
}

void CMenuPopupModals::DialogSettingsRestore()
{
	PopupModalMessage("Choose the type of restore.");

	if (PopupModalButton("Set as default") || IsHotkey("ENTER"))
	{
		const auto& settings_name = GetFocusSettings();
		g_Settings.Restore(settings_name.c_str(), true);

		ImGui::CloseCurrentPopup();
	}

	if (PopupModalButton("Save settings") || IsHotkey("CTRL + S"))
	{
		const auto& settings_name = GetFocusSettings();
		g_Settings.Restore(settings_name.c_str(), false);

		ImGui::CloseCurrentPopup();
	}

	if (IsHotkey("ESC"))
		ImGui::CloseCurrentPopup();
}