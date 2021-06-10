#include "esp_sound_list.h"

#include "..\..\..\..\main.h"

#ifdef PlaySound
#undef PlaySound
#endif

CSoundList::CSoundList()
{
}

CSoundList::~CSoundList()
{
}

std::deque<SoundData>& CSoundList::GetSoundData()
{
	return m_SoundData;
}

void CSoundList::GetActiveSounds()
{
	static CUtlVector<SndInfo_t> SoundList;
	
	if (!g_pIEngine->IsInGame())
		return;

	if (!cvar.visuals.esp_sound_enabled)
		return;

	SoundList.RemoveAll();
	g_pIEngineSound->GetActiveSounds(SoundList);

	for (int i = 0; i < SoundList.Count(); i++)
	{
		if (!SoundList[i].m_pOrigin)
			continue;

		if (SoundList[i].m_nChannel != CHAN_BODY)
			continue;

		const auto index = SoundList[i].m_nSoundSource;

		if (index > MAX_CLIENTS)
			continue;

		if (!cvar.visuals.esp_player_enemies && g_Player[index]->m_iTeamNum != g_Local->m_iTeamNum)
			continue;

		if (!cvar.visuals.esp_player_teammates && g_Player[index]->m_iTeamNum == g_Local->m_iTeamNum)
			continue;

		if (!SoundList[i].m_bUpdatePositions)
			continue;

		PlaySound(*SoundList[i].m_pOrigin, index);
	}
}

void CSoundList::PushSound(const Vector& origin, int index)
{
	SoundData sound;
	sound.time = g_pGlobalVars->curtime;
	sound.origin = origin;
	sound.index = index;
	m_SoundData.push_back(sound);
}

void CSoundList::PlaySound(const Vector& origin, int index)
{
	static std::deque<Vector> positions;

	if (g_Local->m_iEntIndex == index)
		return;

	for (auto& position : positions)
	{
		if (position == origin)
			return;
	}

	// FIX-ME: Clear useless origin copies
	if (positions.size() > 100)
		positions.clear();

	g_Player.UpdateOrigin(origin, index, true);
	PushSound(origin, index);
	positions.push_back(origin);
}