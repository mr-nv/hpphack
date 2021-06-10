#include "SimpleIni/SimpleIni.h"

class CSettings
{
public:
	CSettings();

	void Load(const char* pcszSettingsName);
	void Save(const char* pcszSettingsName);
	void Restore(const char* pcszSettingsName, bool bSetAsDefault = true);
	void Remove(const char* pcszSettingsName);

private:
	enum FileList
	{
		File_Globals,
		File_RageBot,
		File_LegitBot,
		File_Visuals,
		File_Kreedz,
		File_Misc,
		File_GUI,
		File_MAX_COUNT
	};

	enum SettingsActions
	{
		Settings_Load,
		Settings_Save
	};

	CSimpleIniA m_Ini;
	std::string m_sIniFiles[File_MAX_COUNT];
	std::string m_sSection;
	unsigned m_SettingAction;

	void SetSettingAction(unsigned SettingAction);
	void SetSection(const char* pcszSection);

	void Key(std::string name, bool* v, bool v_default = false);
	void Key(std::string name, int* v, int v_default = 0);
	void Key(std::string name, float* v, float v_default = 0.0F);
	void Key(std::string name, char* v, const char* v_default = "null");
	void Key(std::string name, float(&v)[3], const char* v_default = "255,255,255");
	void Key(std::string name, float(&v)[4], const char* v_default = "255,255,255,255");

	void StringToArray(const char* string, float* v, unsigned size);
	void ColorByteToFloat(float* v, unsigned size);
	void ColorFloatToByte(float* v, unsigned size);
	std::string ColorToString(float* v, unsigned size);

	void FileGlobals(std::string sSettingsPath);
	void FileRageBot(std::string sSettingsPath);
	void FileLegitBot(std::string sSettingsPath);
	void FileVisuals(std::string sSettingsPath);
	void FileKreedz(std::string sSettingsPath);
	void FileMisc(std::string sSettingsPath);
	void FileGUI(std::string sSettingsPath);
};

extern CSettings g_Settings;