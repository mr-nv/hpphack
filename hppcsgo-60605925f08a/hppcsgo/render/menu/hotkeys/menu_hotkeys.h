class CMenuHotkeys
{
public:
	CMenuHotkeys();
	~CMenuHotkeys();

protected:
	bool IsHotkey(const char* key);

private:
	size_t GetKeysCount(const char* key);
	void StringReplace(char* buf, const char* search, const char* replace);

	bool IsPressed(const char* key);
};