#include "menu_settings_list.h"

#include <Windows.h>

#include "..\..\..\globals.h"

CMenuSettingsList::CMenuSettingsList() :
	m_selected_settings(0), m_selected_file(0)
{
}

CMenuSettingsList::~CMenuSettingsList()
{
}

std::vector<std::string>& CMenuSettingsList::GetSettingsList()
{
	return m_settings_list;
}

int& CMenuSettingsList::GetSelectedSettings()
{
	return m_selected_settings;
}

int& CMenuSettingsList::GetSelectedFile()
{
	return m_selected_file;
}

std::string& CMenuSettingsList::GetFocusSettings()
{
	const auto i = static_cast<int>(m_settings_list.size() - 1);

	m_selected_settings = min(m_selected_settings, i);
	m_selected_settings = max(m_selected_settings, 0);

	return m_settings_list[m_selected_settings];
}

void CMenuSettingsList::RefreshSettingsList()
{
	m_settings_list.clear();

	const auto path_name = g_Globals.m_sSettingsPath + "*";

	WIN32_FIND_DATA FindFileData;
	HANDLE hFileFind = FindFirstFileA(path_name.c_str(), &FindFileData);

	if (hFileFind == INVALID_HANDLE_VALUE)
		return;

	do
	{
		if (!strstr(FindFileData.cFileName, "."))
		{
			char buf[MAX_PATH];
			LPSTR lpFilePart = NULL;
			GetFullPathNameA(path_name.c_str(), sizeof(buf), buf, &lpFilePart);

			strcpy(lpFilePart, FindFileData.cFileName);

			if (!strstr(FindFileData.cFileName, "*.ini"))
				m_settings_list.push_back(FindFileData.cFileName);
		}

	} while (FindNextFileA(hFileFind, &FindFileData));

	FindClose(hFileFind);
}

void CMenuSettingsList::SetFocusSettings(const char* name)
{
	for (size_t i = 0; i < m_settings_list.size(); i++)
	{
		if (!m_settings_list[i].compare(name))
		{
			m_selected_settings = static_cast<int>(i);
			break;
		}
	}
}