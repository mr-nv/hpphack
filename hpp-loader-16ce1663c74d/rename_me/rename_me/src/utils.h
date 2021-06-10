#pragma once

class Utils
{
public:
	static std::string GetExeDirectory(HMODULE hModule = NULL);
	static bool FileExists(const std::string path);
	static std::string FormatString(const char* fmt, ...);
};


std::string HexToString(const std::string& input);
std::string StringToHex(const std::string& input);

std::string __declspec(noinline) GetUrlData(const std::string& url, int flags = INTERNET_FLAG_SECURE |
	INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_NO_CACHE_WRITE);

std::string FindArgument(std::string input, int num, std::string iterator = ";");

void CopyToClipboard(const char* data);
DWORD FindProcessId(const std::string& sProcessName);

std::string GetLoaderSettingsPath();

int GetValueInt(LPCSTR lpSection, LPCSTR lpValueKey, LPCSTR lpDefValue, LPCSTR lpPath);
bool GetValueBool(LPCSTR lpSection, LPCSTR lpValueKey, LPCSTR lpDefValue, LPCSTR lpPath);

void SetValueInt(LPCSTR lpSection, LPCSTR lpValueKey, int value, LPCSTR lpPath);
void SetValueBool(LPCSTR lpSection, LPCSTR lpValueKey, bool value, LPCSTR lpPath);