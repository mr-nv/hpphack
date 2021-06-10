class CMenuDrawing
{
public:
	CMenuDrawing();
	~CMenuDrawing();

protected:
	enum TABID : unsigned
	{
		RAGEBOT,
		LEGITBOT,
		VISUALS,
		KREEDZ,
		MISC,
		GUI,
		SETTINGS,
		MAX_COUNT
	};

	unsigned GetCurrentTabId();

	void DrawWindowTitle();
	bool DrawFeaturesTabs();

private:
	unsigned m_current_tab_id;
};