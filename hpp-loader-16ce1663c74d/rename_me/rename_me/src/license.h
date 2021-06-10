#pragma once

struct CheatInfo
{
	std::string name;
	std::string version;
	std::string game;
	std::string url;
	std::string folder;
	std::string process;
	std::string description;
	std::string changelog;
};

struct LicenseInfo
{
	std::string sHardwareId;
	bool bIsAviableUpdate;
	bool bIsHaveSubscription;
	bool bIsUpdatedCheatList;
};

class CLicense
{
public:
	CLicense();
	~CLicense();

	std::string GetHardwareId();
	std::string GetBuildsResponse(const std::string& sHardwareId);
};