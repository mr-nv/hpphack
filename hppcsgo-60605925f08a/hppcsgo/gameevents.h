class GameEvents : public IGameEventListener2
{
public:
	GameEvents();

	void Register();
	void Unregister();
	void FireGameEvent(IGameEvent* event);
	int GetEventDebugID();
};

extern GameEvents *g_GameEvents;