#include <iostream>
#include <intrin.h>
#include <Windows.h>

static unsigned __int16 getVolumeHash()
{
	DWORD serialNum = 0;
	GetVolumeInformation(NULL, NULL, 0, &serialNum, NULL, NULL, NULL, 0);
	unsigned __int16 hash = (unsigned __int16)((serialNum + (serialNum >> 16)) & 0xFFFF);
	return hash;
}

static unsigned __int16 getCpuHash()
{
	int cpuinfo[4] = { 0, 0, 0, 0 };
	__cpuid(cpuinfo, 0);
	unsigned __int16 hash = 0;
	unsigned __int16* ptr = (unsigned __int16*)(&cpuinfo[0]);

	for (unsigned __int32 i = 0; i < 8; i++)
		hash += ptr[i];

	return hash;
}

int main()
{
	auto hwid = 1337 * getVolumeHash() + 228 * getCpuHash() + 1488 * getCpuHash() + 696 * getVolumeHash();

    std::cout << "HWID Generator for hpp league\n"; 
	std::cout << "copy+paste: " << hwid  << "\n";

	system("pause");
}

