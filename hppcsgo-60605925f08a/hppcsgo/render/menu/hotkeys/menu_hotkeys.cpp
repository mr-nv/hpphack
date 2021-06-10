#include "menu_hotkeys.h"
#include "menu_hotkeys_map.h"

#include "..\..\..\imgui\imgui.h"

CMenuHotkeys::CMenuHotkeys()
{
}

CMenuHotkeys::~CMenuHotkeys()
{
}

bool CMenuHotkeys::IsHotkey(const char* key)
{
	const auto count = GetKeysCount(key);

	if (!count)
		return false;

	if (count > 1)
	{
		char buf[32];
		sprintf_s(buf, key);

		StringReplace(buf, "+", ",");
		StringReplace(buf, " ", "");

		for (size_t i = 0; i < count; i++)
		{
			const std::string buffer = buf;
			const auto pos = buffer.find(",", i);
			const auto skey = buffer.substr(i, pos - i);

			StringReplace(buf, skey.c_str(), "");

			if (!IsPressed(skey.c_str()))
				return false;
		}
	}
	else
	{
		if (!IsPressed(key))
			return false;
	}

	return true;
}

size_t CMenuHotkeys::GetKeysCount(const char* key)
{
	const std::string skey = key;
	size_t count = skey.size() ? 1 : 0;

	for (size_t i = 0; i < skey.size(); i++)
		if (skey[i] == '+') count++;

	return count;
}

void CMenuHotkeys::StringReplace(char* buf, const char* search, const char* replace)
{
	if (!strlen(buf) || !strlen(search))
		return;

	char* p = buf;

	const auto l1 = strlen(search);
	const auto l2 = strlen(replace);

	while ((p = strstr(p, search)) != 0)
	{
		memmove(p + l2, p + l1, strlen(p + l1) + 1U);
		memcpy(p, replace, l2);

		p += l2;
	}
}

bool CMenuHotkeys::IsPressed(const char* key)
{
	static const auto& io = ImGui::GetIO();
	const auto key_code = GetKeyCode(key);

	if (key_code == 0x00)
		return false;

	return io.KeysDown[key_code];
}