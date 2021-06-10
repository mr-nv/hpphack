#include "framework.h"

std::string Utils::GetExeDirectory(HMODULE hModule)
{
	char szFileName[MAX_PATH];

	if (!GetModuleFileName(hModule, szFileName, MAX_PATH))
		return std::string();

	return szFileName;
}

bool Utils::FileExists(const std::string path)
{
	return _access(path.c_str(), NULL) != -1;
}

std::string Utils::FormatString(const char* fmt, ...)
{
	char buf[4096] = { 0 };

	va_list vl;
	va_start(vl, fmt);
	vsprintf_s(buf, fmt, vl);
	va_end(vl);

	return buf;
}

std::string HexToString(const std::string& input)
{
	const char* const lut = "0123456789ABCDEF";
	const auto len = input.length();

	if (len & 1)
		return input;

	std::string output;
	output.reserve(len / 2);

	for (size_t i = 0; i < len; i += 2)
	{
		const char a = input[i];
		const char* p = std::lower_bound(lut, lut + 16, a);

		if (*p != a)
			return input;

		const char b = input[i + 1];
		const char* q = std::lower_bound(lut, lut + 16, b);

		if (*q != b)
			return input;

		output.push_back(((p - lut) << 4) | (q - lut));
	}

	return output;
}

std::string StringToHex(const std::string& input)
{
	const char* const lut = "0123456789ABCDEF";
	const auto len = input.length();

	std::string output;
	output.reserve(2 * len);

	for (size_t i = 0; i < len; i++)
	{
		const unsigned char c = input[i];
		output.push_back(lut[c >> 4]);
		output.push_back(lut[c & 15]);
	}

	return output;
}

std::string __declspec(noinline) GetUrlData(const std::string& url, int flags)
{
	g_GlobalVars.dwDownloadedBytes = 0;

	std::string request_data;

	HINTERNET hIntSession = InternetOpen(V("Hpp Hack"), INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, NULL);

	if (!hIntSession) 
	{
		InternetCloseHandle(hIntSession);
		return request_data;
	}

	HINTERNET hHttpSession = InternetConnect(hIntSession, V("hpp.ovh"), INTERNET_DEFAULT_HTTPS_PORT,
		nullptr, nullptr, INTERNET_SERVICE_HTTP, NULL, NULL);

	if (!hHttpSession) 
	{
		InternetCloseHandle(hHttpSession);
		return request_data;
	}

	HINTERNET hHttpRequest = HttpOpenRequest(hHttpSession, V("GET"), url.c_str(), V("HTTP/1.1"), nullptr, nullptr, flags, NULL);

	if (!hHttpRequest) 
	{
		InternetCloseHandle(hHttpRequest);
		return request_data;
	}

	if (HttpSendRequest(hHttpRequest, nullptr, NULL, nullptr, NULL))
	{
		char buf[1024] = { 0 };
		DWORD dwRead = 0;

		while (InternetReadFile(hHttpRequest, buf, sizeof(buf) - 1, &dwRead) && dwRead) 
		{
			request_data.append(buf, dwRead);
			g_GlobalVars.dwDownloadedBytes += dwRead;
		}
	}

	InternetCloseHandle(hHttpRequest);
	InternetCloseHandle(hHttpSession);
	InternetCloseHandle(hIntSession);
	return request_data;
}

std::string FindArgument(std::string input, int num, std::string iterator)
{
	auto tmp = input;
	auto count = 0;

	for (int i = 0; i < num; i++)
	{
		if (count && tmp.length() - count > 0)
			tmp = input.substr(count, tmp.length() - count);

		const auto pos = tmp.find(iterator);

		if (pos == std::string::npos)
			return std::string();

		tmp = tmp.substr(0, pos);
		count += tmp.length() + 1;
	}

	return tmp;
}

void CopyToClipboard(const char* data)
{
	if (!OpenClipboard(nullptr))
		return;

	EmptyClipboard();

	auto hgBuf = GlobalAlloc(GMEM_DDESHARE, strlen(data) + 1);
	if (!hgBuf)
		return;

	char* buf = static_cast<char*>(GlobalLock(hgBuf));
	if (!buf)
		return;

	strcpy(buf, data);
	GlobalUnlock(hgBuf);
	SetClipboardData(CF_TEXT, hgBuf);
	CloseClipboard();
}

DWORD FindProcessId(const std::string& sProcessName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (hSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32First(hSnapshot, &processInfo);

	if (!sProcessName.compare(processInfo.szExeFile))
	{
		CloseHandle(hSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32Next(hSnapshot, &processInfo))
	{
		if (!sProcessName.compare(processInfo.szExeFile))
		{
			CloseHandle(hSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(hSnapshot);
	return 0;
}

std::string GetLoaderSettingsPath()
{
	std::string path;
	path.append(getenv("appdata"));
	path.append("\\loader.ini");
	return path;
}

int GetValueInt(LPCSTR lpSection, LPCSTR lpValueKey, LPCSTR lpDefValue, LPCSTR lpPath)
{
	CHAR buf[32];
	GetPrivateProfileString(lpSection, lpValueKey, lpDefValue, buf, sizeof(buf), lpPath);
	return atoi(buf);
}

bool GetValueBool(LPCSTR lpSection, LPCSTR lpValueKey, LPCSTR lpDefValue, LPCSTR lpPath)
{
	CHAR buf[32];
	GetPrivateProfileString(lpSection, lpValueKey, lpDefValue, buf, sizeof(buf), lpPath);
	return static_cast<bool>(atoi(buf));
}

void SetValueInt(LPCSTR lpSection, LPCSTR lpValueKey, int value, LPCSTR lpPath)
{
	WritePrivateProfileStringA(lpSection, lpValueKey, std::to_string(value).c_str(), lpPath);
}

void SetValueBool(LPCSTR lpSection, LPCSTR lpValueKey, bool value, LPCSTR lpPath)
{
	WritePrivateProfileStringA(lpSection, lpValueKey, std::to_string(value).c_str(), lpPath);
}