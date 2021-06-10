#include "esp_player_selection.h"

#include "..\..\..\..\main.h"

CSelectionPlayers::CSelectionPlayers()
{
}

CSelectionPlayers::~CSelectionPlayers()
{
}

std::vector<SelectionData>& CSelectionPlayers::GetSelectionPlayers()
{
	static std::vector<SelectionData> selection_players;

	if (!selection_players.empty())
		selection_players.clear();

	for (int i = 1; i <= g_pIEngine->GetMaxClients(); i++)
		PushPlayer(selection_players, i);

	return SortSelectionPlayers(selection_players);
}

void CSelectionPlayers::PushPlayer(std::vector<SelectionData>& data, int index)
{
	if (g_Local->m_iEntIndex == index)
		return;

	const auto* pBasePlayer = C_BasePlayer::GetPlayerByIndex(index);

	if (!pBasePlayer)
		return;

	if (g_Player[index]->m_IsDead)
		return;

	if (!cvar.visuals.esp_player_enemies && g_Player[index]->m_iTeamNum != g_Local->m_iTeamNum)
		return;

	if (!cvar.visuals.esp_player_teammates && g_Player[index]->m_iTeamNum == g_Local->m_iTeamNum)
		return;

	if (g_Local->m_IsDead)
	{
		if (g_Local->m_iObserverMode != OBS_MODE_ROAMING)
			if (g_Local->m_hObserverTarget == C_BasePlayer::GetPlayerByIndex(index))
				return;
	}

	if (g_Player[index]->m_IsDormant)
	{
		if (g_pGlobalVars->curtime - g_Player[index]->m_flHistory > cvar.visuals.esp_player_history_time)
			return;
	}

	SelectionData player;
	player.distance = g_Local->m_vecOrigin.DistTo(g_Player[index]->m_vecOrigin);
	player.index = index;
	data.push_back(player);
}

std::vector<SelectionData>& CSelectionPlayers::SortSelectionPlayers(const std::vector<SelectionData>& data)
{
	static std::vector<SelectionData> selection_players;

	if (!selection_players.empty())
		selection_players.clear();

	for (size_t start_index = 0; start_index < data.size(); start_index++)
	{
		auto max_distance = 0.f;
		auto sort_index = 0;

		for (size_t current_index = 0; current_index < data.size(); current_index++)
		{
			bool skip_player = false;

			if (!selection_players.empty())
			{
				for (auto& player : selection_players)
				{
					if (player.index == data[current_index].index)
					{
						skip_player = true;
						break;
					}
				}

				if (skip_player)
					continue;
			}

			if (data[current_index].distance > max_distance)
			{
				max_distance = data[current_index].distance;
				sort_index = data[current_index].index;
			}
		}

		SelectionData player;
		player.distance = max_distance;
		player.index = sort_index;
		selection_players.push_back(player);
	}

	return selection_players;
}
