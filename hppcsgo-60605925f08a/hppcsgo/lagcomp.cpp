#include "main.h"

CLagCompensation g_LagComp;

void CLagCompensation::Restore(C_BasePlayer *player)
{
	if (player)
	{
		auto index = player->EntIndex();
		auto pBoneCache = player->GetBoneCache();
		auto pBoneCacheCount = player->GetBoneCacheCount();

		if (g_Player[index]->m_iBoneCacheCount && pBoneCache && pBoneCacheCount)
		{
			//*pBoneCacheCount = g_Player[index]->m_iBoneCacheCount;

			for (auto y = 0; y < *pBoneCacheCount; y++)
			{
				g_Utils.MatrixCopy(g_Player[index]->m_BoneMatrix[y], pBoneCache[y]);
			}
		//	RtlCopyMemory(pBoneCache, g_Player[index]->m_BoneMatrix, sizeof(g_Player[index]->m_BoneMatrix));
		}

		player->SetAbsOrigin(player->m_vecOrigin());
		player->SetAbsAngles(player->m_angEyeAngles());
	}
}

void CLagCompensation::SetInterpolationFlags(C_BasePlayer* player, int flag) 
{
	auto VarMap = reinterpret_cast<uintptr_t>(player) + 36; // tf2 = 20
	auto VarMapSize = *reinterpret_cast<int*>(VarMap + 20);

	for (auto index = 0; index < VarMapSize; index++)
		* reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(VarMap) + index * 12) = flag;
}

void CLagCompensation::ProcessCmd(CUserCmd* cmd, CHistory record)
{
	auto old = cmd->tick_count;

	cmd->tick_count = TIME_TO_TICKS(record.m_flSimulationTime/* + GetLerpTime()*/);

	/*g_pIEngine->Con_NPrintf(1, "diff ticks: %i", old - cmd->tick_count);
	g_pIEngine->Con_NPrintf(2, "back ticks: %i", TIME_TO_TICKS(record.m_flSimulationTime));
	g_pIEngine->Con_NPrintf(3, "lerp ticks: %i", TIME_TO_TICKS(GetLerpTime()));*/
}

void CLagCompensation::Store()
{
	for (auto i = 1; i <= g_pGlobalVars->maxClients; ++i)
	{
		const auto player = C_BasePlayer::GetPlayerByIndex(i);

		if (!player)
			continue;

		if (player == g_pLocalPlayer)
			continue;

		if (!player->IsPlayer())
			continue;

		auto& history = g_Player.GetHistory(i);

		if (!player->IsAlive())
		{
			history.clear();
			continue;
		}

		if (player->IsDormant())
			continue;

		//SetInterpolationFlags(player, FALSE);

		CHistory record;
		record.m_iIndex = player->EntIndex();
		record.m_QEyeAngles = player->m_angEyeAngles();
		record.m_vecOrigin = player->m_vecOrigin();
		record.m_flSimulationTime = player->m_flSimulationTime();

		const auto pBoneCacheCount = player->GetBoneCacheCount();

		if (!pBoneCacheCount)
			continue;

		record.m_iBoneCacheCount = *pBoneCacheCount;

		if (record.m_iBoneCacheCount <= 0)
			continue;

		const auto pModel = player->GetModel();

		if (!pModel)
			continue;

		const auto pStudioModel = g_pIModelInfo->GetStudiomodel(pModel);

		if (pStudioModel == nullptr)
			continue;

		const auto pHitboxSet = pStudioModel->GetHitboxSet(0);

		if (!pHitboxSet)
			continue;

		/*static matrix3x4_t BoneMatrix[MAXSTUDIOBONES];

		if (!player->SetupBones(BoneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.f))
			continue;*/

		const auto BoneMatrix = player->GetBoneCache();

		if (!BoneMatrix)
			continue;

		for (auto y = 0; y < MAXSTUDIOBONES; y++)
			g_Utils.MatrixCopy(BoneMatrix[y], record.m_BoneMatrix[y]);

		Vector vecBoundBoxMin, vecBoundBoxMax;

		for (auto i = 0; i < pHitboxSet->numhitboxes; i++)
		{
			const auto pHitbox = pHitboxSet->GetHitbox(i);

			if (pHitbox == nullptr)
				continue;

			g_Utils.VectorTransform(pHitbox->bbmin, BoneMatrix[pHitbox->bone], vecBoundBoxMin);
			g_Utils.VectorTransform(pHitbox->bbmax, BoneMatrix[pHitbox->bone], vecBoundBoxMax);

			record.m_flHitboxRadius[i] = pHitbox->m_flRadius;
			record.m_vecHitbox[i] = (vecBoundBoxMin + vecBoundBoxMax) * 0.5f;
			record.m_vecHitboxMin[i] = vecBoundBoxMin;
			record.m_vecHitboxMax[i] = vecBoundBoxMax;
		}

		if (!history.size())
		{
			history.push_back(record);
			continue;
		}

		if (history.front().m_flSimulationTime != player->m_flSimulationTime())
			history.push_back(record);

		while (abs(player->m_flSimulationTime() - history.front().m_flSimulationTime) > LAG_COMPENSATION_DATA_TIME)
			history.pop_front();
	}
}

float CLagCompensation::GetLerpTime()
{
	static const auto *cl_updaterate = g_pICvar->FindVar("cl_updaterate");
	static const auto *sv_minupdaterate = g_pICvar->FindVar("sv_minupdaterate");
	static const auto *sv_maxupdaterate = g_pICvar->FindVar("sv_maxupdaterate");
	static const auto *cl_interp_ratio = g_pICvar->FindVar("cl_interp_ratio");
	static const auto *cl_interp = g_pICvar->FindVar("cl_interp");
	static const auto *cl_interpolate = g_pICvar->FindVar("cl_interpolate");

	if (cl_interpolate->GetInt() != 0)
	{
		int m_nUpdateRate = cl_updaterate->GetInt();

		if (sv_minupdaterate && sv_maxupdaterate)
			m_nUpdateRate = ImClamp(m_nUpdateRate, (int)sv_minupdaterate->GetFloat(), (int)sv_maxupdaterate->GetFloat());

		float flLerpRatio = cl_interp_ratio->GetFloat();

		if (flLerpRatio == 0)
			flLerpRatio = 1.0f;

		float flLerpAmount = cl_interp->GetFloat();

		static const auto *sv_client_min_interp_ratio = g_pICvar->FindVar("sv_client_min_interp_ratio");
		static const auto *sv_client_max_interp_ratio = g_pICvar->FindVar("sv_client_max_interp_ratio");

		if (sv_client_min_interp_ratio && sv_client_max_interp_ratio && sv_client_min_interp_ratio->GetFloat() != -1)
			flLerpRatio = ImClamp(flLerpRatio, sv_client_min_interp_ratio->GetFloat(), sv_client_max_interp_ratio->GetFloat());

		return max(flLerpAmount, flLerpRatio / m_nUpdateRate);
	}

	return 0.0f;
}

bool CLagCompensation::IsValidRecord(CHistory record)
{
	static auto sv_maxunlag = g_pICvar->FindVar("sv_maxunlag");

	float correct = GetLerpTime();

	auto nci = g_pIEngine->GetNetChannelInfo();

	if (nci)
	{
		correct += nci->GetLatency(FLOW_OUTGOING);
		correct += nci->GetLatency(FLOW_INCOMING);
	}

	correct = ImClamp(correct, 0.0f, sv_maxunlag->GetFloat());

	float deltaTime = correct - (g_pGlobalVars->curtime - record.m_flSimulationTime + GetLerpTime());

	if (fabs(deltaTime) > 0.2f)
		return false;

	return true;
}

void CLagCompensation::SetRecord(C_BasePlayer *player, CHistory record)
{
	if (player->EntIndex() != record.m_iIndex)
		return;

	if (!record.m_iBoneCacheCount)
		return;

	auto pBoneCache = player->GetBoneCache();
	auto pBoneCacheCount = player->GetBoneCacheCount();

	if (pBoneCache && pBoneCacheCount)
	{
		//*pBoneCacheCount = record.m_iBoneCacheCount;

		//RtlCopyMemory(pBoneCache, record.m_BoneMatrix, sizeof(record.m_BoneMatrix));
		for (auto y = 0; y < *pBoneCacheCount; y++)
		{
			g_Utils.MatrixCopy(record.m_BoneMatrix[y], pBoneCache[y]);
		}

		player->SetAbsOrigin(record.m_vecOrigin);//fix bbox traces
		player->SetAbsAngles(record.m_QEyeAngles);
	}
}