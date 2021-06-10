#include <vector>
#include <string>

class CMenuSettingsList
{
public:
	CMenuSettingsList();
	~CMenuSettingsList();

protected:
	std::vector<std::string>& GetSettingsList();

	int& GetSelectedSettings();
	int& GetSelectedFile();

	std::string& GetFocusSettings();

	void RefreshSettingsList();
	void SetFocusSettings(const char* name);

private:
	std::vector<std::string> m_settings_list;

	int m_selected_settings;
	int m_selected_file;
};