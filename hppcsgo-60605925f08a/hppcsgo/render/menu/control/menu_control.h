class CMenuControl
{
public:
	CMenuControl();
	~CMenuControl();

	bool IsOpened();

	void Open();
	void Close();
	void Toggle();

private:
	bool m_IsOpened;
};