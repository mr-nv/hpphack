#include "main.h"

CUtils g_Utils;

bool CUtils::CalcScreen(Vector a_vecOrigin, float* a_pflVecScreen)
{
	const auto result = g_pEngine->pTriAPI->WorldToScreen(a_vecOrigin, a_pflVecScreen);

	if (a_pflVecScreen[0] < 1 && a_pflVecScreen[1] < 1 && a_pflVecScreen[0] > -1 && a_pflVecScreen[1] > -1 && !result)
	{
		a_pflVecScreen[0] = a_pflVecScreen[0] * (g_Screen.iWidth / 2) + (g_Screen.iWidth / 2);
		a_pflVecScreen[1] = -a_pflVecScreen[1] * (g_Screen.iHeight / 2) + (g_Screen.iHeight / 2);

		return true;
	}

	return false;
}

void CUtils::VectorAngles(const float* forward, float* angles)
{
	float tmp, yaw, pitch;

	if (forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if (forward[2] > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / IM_PI);
		if (yaw < 0)
			yaw += 360;

		tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = (atan2(-forward[2], tmp) * 180 / IM_PI);
		if (pitch < 0)
			pitch += 360;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;

	while (angles[0] < -89)
	{
		angles[0] += 180;
		angles[1] += 180;
	}

	while (angles[0] > 89)
	{
		angles[0] -= 180;
		angles[1] += 180;
	}

	while (angles[1] < -180)
		angles[1] += 360;

	while (angles[1] > 180)
		angles[1] -= 360;
}


void CUtils::FixMoveStart(usercmd_s* cmd)
{
	NormalizeAngles(cmd->viewangles);

	_vecPreviousAngles = cmd->viewangles;
}

void CUtils::FixMoveEnd(usercmd_s* cmd)
{
	auto speed = Vector(cmd->forwardmove, cmd->sidemove, 0.f).Length2D();

	if (speed == 0.f)
		return;

	auto yaw = cmd->viewangles.y - _vecPreviousAngles.y;
	auto move_yaw_rad = atan2(cmd->sidemove, cmd->forwardmove) + DEG2RAD(yaw);

	cmd->forwardmove = cos(move_yaw_rad) * speed;
	cmd->sidemove = sin(move_yaw_rad) * speed;

	auto old_backwards = (_vecPreviousAngles.x > 90.f || _vecPreviousAngles.x < -90.f);
	auto new_backwards = (cmd->viewangles.x > 90.f || cmd->viewangles.x < -90.f);

	if (old_backwards != new_backwards)
		cmd->forwardmove = -cmd->forwardmove;
}

void CUtils::NormalizeAngles(float* flAngles)
{
	for (auto i = 0; i < 3; ++i)
	{
		auto revolutions = flAngles[i] / 360;

		if (flAngles[i] > 180 || flAngles[i] < -180)
		{
			if (revolutions < 0)
				revolutions = -revolutions;

			revolutions = round(revolutions);

			if (flAngles[i] < 0)
				flAngles[i] = (flAngles[i] + 360 * revolutions);
			else
				flAngles[i] = (flAngles[i] - 360 * revolutions);
		}
	}
}

void CUtils::MakeAngle(bool a_bAddAngles, float* a_flAngles, usercmd_s * cmd)
{
	FixMoveStart(cmd);

	if (a_bAddAngles)
	{
		cmd->viewangles[0] += a_flAngles[0];
		cmd->viewangles[1] += a_flAngles[1];
		cmd->viewangles[2] += a_flAngles[2];
	}
	else
	{
		cmd->viewangles[0] = a_flAngles[0];
		cmd->viewangles[1] = a_flAngles[1];
		cmd->viewangles[2] = a_flAngles[2];
	}

	FixMoveEnd(cmd);
}

bool CUtils::IsBoxIntersectingRay(const Vector& vecBoundBoxMin, const Vector& vecBoundBoxMax, const Vector& vecOrigin, const Vector& vecDelta)
{
	// FIXME: Surely there's a faster way
	float tmin = -FLT_MAX;
	float tmax = FLT_MAX;

	for (int i = 0; i < 3; ++i)
	{
		// Parallel case...
		if (fabs(vecDelta[i]) < 1e-8)
		{
			// Check that origin is in the box
			// if not, then it doesn't intersect..
			if ((vecOrigin[i] < vecBoundBoxMin[i]) || (vecOrigin[i] > vecBoundBoxMax[i]))
				return false;

			continue;
		}

		// non-parallel case
		// Find the t's corresponding to the entry and exit of
		// the ray along x, y, and z. The find the furthest entry
		// point, and the closest exit point. Once that is done,
		// we know we don't collide if the closest exit point
		// is behind the starting location. We also don't collide if
		// the closest exit point is in front of the furthest entry point

		float invDelta = 1.f / vecDelta[i];
		float t1 = (vecBoundBoxMin[i] - vecOrigin[i]) * invDelta;
		float t2 = (vecBoundBoxMax[i] - vecOrigin[i]) * invDelta;

		if (t1 > t2)
		{
			float temp = t1;
			t1 = t2;
			t2 = temp;
		}

		if (t1 > tmin)
			tmin = t1;

		if (t2 < tmax)
			tmax = t2;

		if (tmin > tmax)
			return false;

		if (tmax < 0)
			return false;
	}

	return true;
}

int CUtils::LookupSequence(model_s* a_pModel, const char** a_pcszLabel, const int a_nSize)
{
	const auto* const pstudiohdr = (studiohdr_t*)(g_Studio.Mod_Extradata(a_pModel));

	if (pstudiohdr == nullptr)
		return ACT_INVALID;

	const auto * const pseqdesc = (mstudioseqdesc_t*)((byte*)pstudiohdr + pstudiohdr->seqindex);

	if (pseqdesc == nullptr)
		return ACT_INVALID;

	for (int i = 0; i < pstudiohdr->numseq; ++i)
	{
		for (int j = 0; j < a_nSize; ++j)
		{
			if (!_stricmp(pseqdesc[i].label, a_pcszLabel[j]))
				return i;
		}
	}

	return ACT_INVALID;
}

void CUtils::VectorTransform(Vector in1, float in2[3][4], float* out, int xyz, float multi)
{
	in1[xyz] *= multi;

	out[0] = in1.Dot(in2[0]) + in2[0][3];
	out[1] = in1.Dot(in2[1]) + in2[1][3];
	out[2] = in1.Dot(in2[2]) + in2[2][3];
}

void CUtils::VectorTransform(Vector in1, float in2[3][4], float* out)
{
	out[0] = in1.Dot(in2[0]) + in2[0][3];
	out[1] = in1.Dot(in2[1]) + in2[1][3];
	out[2] = in1.Dot(in2[2]) + in2[2][3];
}

void CUtils::GetHitboxes(const cl_entity_s* a_pGameEntity)
{
	if (!a_pGameEntity)
		return;

	auto resolvedIndex = a_pGameEntity->index;

	if (resolvedIndex > 0 && resolvedIndex <= MAX_CLIENTS && resolvedIndex != g_Local.m_iIndex)
	{
		auto pPlayer = g_World.GetPlayer(resolvedIndex);

		auto pStudioHeader = (studiohdr_t*)g_pStudio->Mod_Extradata(a_pGameEntity->model);
		
		auto pBoneMatrix = (TransformMatrix*)g_pStudio->StudioGetBoneTransform();
		auto pHitbox = (mstudiobbox_t*)((byte*)(pStudioHeader)+pStudioHeader->hitboxindex);

		if (pHitbox == nullptr)
			return;

		Vector vecBoundBoxMins, vecBoundBoxMaxs, vecTransform, vecMultiPoint;

		for (int i = 0; i < pStudioHeader->numhitboxes; i++)
		{
			VectorTransform(pHitbox[i].bbmin, (*pBoneMatrix)[pHitbox[i].bone], vecBoundBoxMins);
			VectorTransform(pHitbox[i].bbmax, (*pBoneMatrix)[pHitbox[i].bone], vecBoundBoxMaxs);

			pPlayer->m_vecHitboxMin[i] = vecBoundBoxMins;
			pPlayer->m_vecHitboxMax[i] = vecBoundBoxMaxs;
			pPlayer->m_vecHitbox[i] = (vecBoundBoxMaxs + vecBoundBoxMins) * 0.5f;
			pPlayer->m_vecHitboxPoints[i][0] = vecBoundBoxMaxs;
			pPlayer->m_vecHitboxPoints[i][1] = vecBoundBoxMins;

			vecMultiPoint = (pHitbox[i].bbmin / pHitbox[i].bbmax);

			VectorTransform(pHitbox[i].bbmax, (*pBoneMatrix)[pHitbox[i].bone], vecTransform, 0, vecMultiPoint.x);
			pPlayer->m_vecHitboxPoints[i][2] = vecTransform;

			VectorTransform(pHitbox[i].bbmax, (*pBoneMatrix)[pHitbox[i].bone], vecTransform, 1, vecMultiPoint.y);
			pPlayer->m_vecHitboxPoints[i][3] = vecTransform;

			VectorTransform(pHitbox[i].bbmax, (*pBoneMatrix)[pHitbox[i].bone], vecTransform, 2, vecMultiPoint.z);
			pPlayer->m_vecHitboxPoints[i][4] = vecTransform;

			vecMultiPoint = (pHitbox[i].bbmax / pHitbox[i].bbmin);

			VectorTransform(pHitbox[i].bbmin, (*pBoneMatrix)[pHitbox[i].bone], vecTransform, 0, vecMultiPoint.x);
			pPlayer->m_vecHitboxPoints[i][5] = vecTransform;

			VectorTransform(pHitbox[i].bbmin, (*pBoneMatrix)[pHitbox[i].bone], vecTransform, 1, vecMultiPoint.y);
			pPlayer->m_vecHitboxPoints[i][6] = vecTransform;

			VectorTransform(pHitbox[i].bbmin, (*pBoneMatrix)[pHitbox[i].bone], vecTransform, 2, vecMultiPoint.z);
			pPlayer->m_vecHitboxPoints[i][7] = vecTransform;
		}
	}
}
