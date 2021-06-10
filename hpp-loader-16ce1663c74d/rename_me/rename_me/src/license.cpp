#include "framework.h"

CLicense::CLicense()
{
}

CLicense::~CLicense()
{
}

std::string __forceinline xorstr(const std::string& input)
{
	const char arr[] = { '?','m','G','D','$','a','n','W','}','T','c','s','z','3', '5','4','v',
		'N','i','~','|','H','B','6','9','Z','Y','S','o','d','K','Q','F','b', 'M','I','J','w','x','h',
		'@','g','8','O','g','z','X','u','*','m','b','c','k','N','q','d','S','0','k','d','P','i','t','L' };

	std::string output = input;

	for (size_t i = 0; i < input.size(); i++)
		output[i] = input[i] ^ arr[i % (sizeof(arr) / sizeof(char))];

	return output;
}

std::string CLicense::GetHardwareId()
{
	IWbemLocator* pLocator = nullptr;
	IWbemServices* pServices = nullptr;
	IEnumWbemClassObject* pEnumerator = nullptr;
	IWbemClassObject* pClassObj = nullptr;
	ULONG ulReturn = NULL;

	HRESULT hResult = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if (FAILED(hResult))
		return {};

	hResult = CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, reinterpret_cast<LPVOID*>(&pLocator));

	if (FAILED(hResult))
		return {};

	hResult = pLocator->ConnectServer(bstr_t(V("ROOT\\CIMV2")),
		NULL, NULL, NULL, NULL, NULL, NULL, &pServices);

	if (FAILED(hResult))
		return {};

	hResult = CoSetProxyBlanket(pServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
		RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

	if (FAILED(hResult))
		return {};

	hResult = pServices->ExecQuery(bstr_t(V("WQL")), bstr_t(V("SELECT * FROM Win32_BaseBoard")),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);

	if (FAILED(hResult))
		return {};

	std::wstringstream wshwidstream;
	wshwidstream << std::time(0) << ';' << GetCurrentProcessId() << ';';

	while (pEnumerator)
	{
		pEnumerator->Next(WBEM_INFINITE, 1, &pClassObj, &ulReturn);

		if (!ulReturn)
			break;

		VARIANT vtProp;
		pClassObj->Get(W_V(L"SerialNumber"), NULL, &vtProp, NULL, NULL);
		wshwidstream << vtProp.bstrVal << ';';
		VariantClear(&vtProp);
		pClassObj->Release();
	}

	hResult = pServices->ExecQuery(bstr_t(V("WQL")), bstr_t(V("SELECT * FROM Win32_BIOS")),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);

	if (FAILED(hResult))
		return {};

	while (pEnumerator)
	{
		pEnumerator->Next(WBEM_INFINITE, 1, &pClassObj, &ulReturn);

		if (!ulReturn)
			break;

		VARIANT vtProp;
		pClassObj->Get(W_V(L"SerialNumber"), NULL, &vtProp, NULL, NULL);
		wshwidstream << vtProp.bstrVal << ';';
		VariantClear(&vtProp);
		pClassObj->Release();
	}

	hResult = pServices->ExecQuery(bstr_t(V("WQL")), bstr_t(V("SELECT * FROM Win32_ComputerSystemProduct")),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);

	if (FAILED(hResult))
		return {};

	while (pEnumerator)
	{
		pEnumerator->Next(WBEM_INFINITE, 1, &pClassObj, &ulReturn);

		if (!ulReturn)
			break;

		VARIANT vtProp;
		pClassObj->Get(W_V(L"UUID"), NULL, &vtProp, NULL, NULL);
		wshwidstream << vtProp.bstrVal << ';';
		VariantClear(&vtProp);
		pClassObj->Release();
	}

	hResult = pServices->ExecQuery(bstr_t(V("WQL")), bstr_t(V("SELECT * FROM Win32_DesktopMonitor")),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);

	if (FAILED(hResult))
		return {};

	while (pEnumerator)
	{
		pEnumerator->Next(WBEM_INFINITE, 1, &pClassObj, &ulReturn);

		if (!ulReturn)
			break;

		VARIANT vtProp;
		pClassObj->Get(W_V(L"PNPDeviceID"), NULL, &vtProp, NULL, NULL);

		if (!vtProp.bstrVal)
			continue;

		wshwidstream << vtProp.bstrVal << ';';
		VariantClear(&vtProp);
		pClassObj->Release();
	}

	hResult = pServices->ExecQuery(bstr_t(V("WQL")), bstr_t(V("SELECT * FROM Win32_DiskDrive")),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);

	if (FAILED(hResult))
		return {};

	while (pEnumerator)
	{
		pEnumerator->Next(WBEM_INFINITE, 1, &pClassObj, &ulReturn);

		if (!ulReturn)
			break;

		VARIANT vtProp;
		pClassObj->Get(W_V(L"MediaType"), NULL, &vtProp, NULL, NULL);

		if (!vtProp.bstrVal)
			continue;

		const std::wstring tmp = vtProp.bstrVal;

		if (tmp.find(W_V(L"Fixed")) == std::string::npos)
			continue;

		pClassObj->Get(W_V(L"SerialNumber"), NULL, &vtProp, NULL, NULL);

		if (!vtProp.bstrVal)
			continue;

		wshwidstream << vtProp.bstrVal << ';';
		VariantClear(&vtProp);
		pClassObj->Release();
	}

	pServices->Release();
	pLocator->Release();
	pEnumerator->Release();
	CoUninitialize();

	const auto wshwid = wshwidstream.str();
	std::string shashedhwid(wshwid.begin(), wshwid.end());
	return StringToHex(xorstr(shashedhwid));
}

std::string CLicense::GetBuildsResponse(const std::string& sHardwareId)
{
	return GetUrlData(V("/hpp/sockets/server.php?product=0&req=getbuilds&hwid=") + sHardwareId);
}