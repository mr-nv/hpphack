#include "main.h"

CUtils g_Utils;

bool CUtils::IsValidPlayer(C_BasePlayer *player, bool teammates)
{
	if (!player)
		return false;

	if (player == g_pLocalPlayer)
		return false;

	if (!player->IsPlayer())
		return false;

	if (!player->IsAlive())
		return false;

	if (player->m_bGunGameImmunity())
		return false;

	if (player->IsDormant())
		return false;

	if (player->EntIndex() < 1 || player->EntIndex() > g_pGlobalVars->maxClients)
		return false;

	if (!teammates && player->m_iTeamNum() == g_Local->m_iTeamNum)
		return false;

	return true;
}

bool CUtils::IsBehindSmoke(Vector src, Vector rem)
{
	using LineGoesThroughSmokeFn = bool(__cdecl*)(Vector, Vector, int16_t);

	static auto LineGoesThroughSmoke = (LineGoesThroughSmokeFn)(g_Offsets.FindDownPattern((PCHAR)"\x55\x8B\xEC\x83\xEC\x08\x8B\x15\xFF\xFF\xFF\xFF\x0F\x57\xC0", (PCHAR)"xxxxxxxx????xxx", g_Offsets.client_panorama.base, g_Offsets.client_panorama.end));

	return LineGoesThroughSmoke(src, rem, true);
}

bool CUtils::IsVisible(int iEntIndex, const Vector& vecTarget, bool bSmokeCheck)
{
	if (iEntIndex == g_Local->m_iEntIndex)
		return true;

	C_BasePlayer * pLocalPlayer = static_cast<C_BasePlayer*>(g_pLocalPlayer);

	if (g_Local->m_IsDead)
	{
		if (g_Local->m_iObserverMode == OBS_MODE_IN_EYE && g_Local->m_hObserverTarget)
			pLocalPlayer = static_cast<C_BasePlayer*>(g_pIEntityList->GetClientEntityFromHandle(g_Local->m_hObserverTarget));

		if (!pLocalPlayer)
			return true;
	}

	const Vector vecEyePos = pLocalPlayer->GetEyePos();

	QAngle QViewAngles;
	g_pIEngine->GetViewAngles(&QViewAngles);

	Ray_t ray;
	ray.Init(vecEyePos, vecTarget);

	CTraceFilter traceFilter;
	traceFilter.pSkip = pLocalPlayer;

	trace_t tr;
	g_pIEngineTrace->TraceRay(ray, MASK_SHOT, &traceFilter, &tr);

	if (bSmokeCheck && IsBehindSmoke(vecEyePos, vecTarget))
		return false;

	return tr.hit_entity == C_BaseEntity::GetEntityByIndex(iEntIndex);
}

void CUtils::MatrixCopy(const matrix3x4_t& in, matrix3x4_t& out)
{
	memcpy(out.Base(), in.Base(), sizeof(float) * 3 * 4);
}

float CUtils::raySphereIntersect(Vector vecOrigin, Vector vecDirection, Vector vecSphereCenter, float flSphereRadius) {
	// - r0: ray origin
	// - rd: normalized ray direction
	// - s0: sphere center
	// - sr: sphere radius
	// - Returns distance from r0 to first intersecion with sphere,
	//   or -1.0 if no intersection.
	float a = vecDirection.Dot(vecDirection);
	Vector s0_r0 = vecOrigin - vecSphereCenter;
	float b = 2.0 *vecDirection.Dot(s0_r0);
	float c = s0_r0.Dot(s0_r0) - (flSphereRadius * flSphereRadius);

	if (b*b - 4.0*a*c < 0.0)
		return -1.0;

	return (-b - sqrt((b*b) - 4.0*a*c)) / (2.0*a);
}
void CUtils::FixMoveStart(CUserCmd * cmd)
{
	_QPreviousAngles = cmd->viewangles;
}

void CUtils::FixMoveEnd(CUserCmd * cmd)
{
	auto speed = Vector(cmd->forwardmove, cmd->sidemove, 0.f).Length2D();

	if (speed == 0.f)
		return;

	auto yaw = cmd->viewangles.y - _QPreviousAngles.y;
	auto move_yaw_rad = atan2(cmd->sidemove, cmd->forwardmove) + DEG2RAD(yaw);

	cmd->forwardmove = cos(move_yaw_rad) * speed;
	cmd->sidemove = sin(move_yaw_rad) * speed;

	auto old_backwards = (_QPreviousAngles.x > 90.f || _QPreviousAngles.x < -90.f);
	auto new_backwards = (cmd->viewangles.x > 90.f || cmd->viewangles.x < -90.f);

	if (old_backwards != new_backwards)
		cmd->forwardmove = -cmd->forwardmove;

	if (cmd->forwardmove > 450.f)
		cmd->forwardmove = 450.f;
	else if (cmd->forwardmove < -450.f)
		cmd->forwardmove = -450.f;

	if (cmd->sidemove > 450.f)
		cmd->sidemove = 450.f;
	else if (cmd->sidemove < -450.f)
		cmd->sidemove = -450.f;

	if (cmd->upmove > 320.f)
		cmd->upmove = 320.f;
	else if (cmd->upmove < -320.f)
		cmd->upmove = -320.f;
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

void CUtils::MakeAngle(bool a_bAddAngles, float* a_flAngles, CUserCmd * cmd)
{
	NormalizeAngles(cmd->viewangles);

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

	NormalizeAngles(cmd->viewangles);

	ClampAngles(cmd->viewangles);
}

float CUtils::AngleBetween(Vector a, Vector b)
{
	a.Normalize();
	b.Normalize();

	return RAD2DEG(acosf(a.Dot(b)));
}

void CUtils::RandomSeed(int iSeed)
{
	static auto ranSeed = reinterpret_cast<void(*)(int)>(GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomSeed"));

	ranSeed(iSeed);
}

float CUtils::RandomFloat(float min, float max)
{
	static auto ranFloat = reinterpret_cast<float(*)(float, float)>(GetProcAddress(g_Offsets.vstdlib.handle, "RandomFloat"));

	return ranFloat(min, max);
}

void CUtils::VectorAngles(const Vector & forward, QAngle & angles)
{
	float	tmp, yaw, pitch;

	if (forward[1] == 0 && forward[0] == 0) {
		yaw = 0;
		if (forward[2] > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else {
		yaw = (atan2(forward[1], forward[0]) * 180 / DirectX::XM_PI);
		if (yaw < 0)
			yaw += 360;

		tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = (atan2(-forward[2], tmp) * 180 / DirectX::XM_PI);
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

	while (angles[2] < -180)
		angles[2] += 360;

	while (angles[2] > 180)
		angles[2] -= 360;
}

void CUtils::ClampAngles(QAngle & angles)
{
	if (angles.x > 89.0f)
		angles.x = 89.0f;
	else if (angles.x < -89.0f)
		angles.x = -89.0f;

	if (angles.y > 180.0f)
		angles.y = 180.0f;
	else if (angles.y < -180.0f)
		angles.y = -180.0f;

	angles.z = 0;
}

void CUtils::VectorTransform(Vector in1, const matrix3x4_t &in2, Vector & out, int xyz, float multi)
{
	in1[xyz] *= multi;

	out[0] = in1.Dot(in2[0]) + in2[0][3];
	out[1] = in1.Dot(in2[1]) + in2[1][3];
	out[2] = in1.Dot(in2[2]) + in2[2][3];
}

void CUtils::VectorTransform(const Vector & in1, const matrix3x4_t & in2, Vector & out)
{
	out[0] = in1.Dot(in2[0]) + in2[0][3];
	out[1] = in1.Dot(in2[1]) + in2[1][3];
	out[2] = in1.Dot(in2[2]) + in2[2][3];
}

bool CUtils::IsBoxIntersectingRay(const Vector & vecBoundBoxMin, const Vector & vecBoundBoxMax, const Vector & vecOrigin, const Vector & vecDelta)
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

bool CUtils::IsGun(const std::string& name)
{
	if (name.compare("unknown") == 0)
		return false;

	if (name.compare("flashbang") == 0 || name.compare("hegrenade") == 0 ||
		name.compare("smokegrenade") == 0 || name.compare("molotov") == 0 ||
		name.compare("decoy") == 0 || name.compare("incgrenade") == 0)
		return false;

	if (name.compare("taser") == 0 || name.compare("c4") == 0)
		return false;

	if (name.compare("knife") == 0 || name.compare("glove") == 0)
		return false;

	return true;
}

bool CUtils::ScreenTransform(const Vector & vecPoint, float* flScreenPos)
{
	const auto& WorldToScreenMatrix = g_pIEngine->WorldToScreenMatrix();

	flScreenPos[0] = WorldToScreenMatrix.m[0][0] * vecPoint[0] + WorldToScreenMatrix.m[0][1] *
		vecPoint[1] + WorldToScreenMatrix.m[0][2] * vecPoint[2] + WorldToScreenMatrix.m[0][3];

	flScreenPos[1] = WorldToScreenMatrix.m[1][0] * vecPoint[0] + WorldToScreenMatrix.m[1][1] *
		vecPoint[1] + WorldToScreenMatrix.m[1][2] * vecPoint[2] + WorldToScreenMatrix.m[1][3];

	const float w = WorldToScreenMatrix.m[3][0] * vecPoint[0] + WorldToScreenMatrix.m[3][1] *
		vecPoint[1] + WorldToScreenMatrix.m[3][2] * vecPoint[2] + WorldToScreenMatrix.m[3][3];

	if (w < 0.001f)
	{
		flScreenPos[0] *= 100000.f;
		flScreenPos[1] *= 100000.f;
		return false;
	}

	flScreenPos[0] /= w;
	flScreenPos[1] /= w;

	return true;
}

bool CUtils::WorldToScreen(const Vector & vecOrigin, float* flScreenPos)
{
	if (ScreenTransform(vecOrigin, flScreenPos))
	{
		int iScreenWidth, iScreenHeight;
		g_pIEngine->GetScreenSize(iScreenWidth, iScreenHeight);

		const float flScreenWidth = static_cast<float>(iScreenWidth);
		const float flScreenHeight = static_cast<float>(iScreenHeight);

		if (flScreenPos[0] < 1.045f && flScreenPos[1] < 1.045f && flScreenPos[0] > -1.045f && flScreenPos[1] > -1.045f)
		{
			flScreenPos[0] = flScreenWidth * 0.5f + flScreenPos[0] * flScreenWidth * 0.5f;
			flScreenPos[1] = flScreenHeight * 0.5f - flScreenPos[1] * flScreenHeight * 0.5f;
			return true;
		}
	}

	return false;
}

void TraceLog(const char* fmt, ...)
{
	va_list argptr;
	static char string[4096];

	RtlSecureZeroMemory(string, sizeof(string));

	va_start(argptr, fmt);
	vsnprintf(string, sizeof(string), fmt, argptr);
	va_end(argptr);

	FILE* fp = fopen(g_Globals.m_sDebugFile.c_str(), "a+");

	if (!fp)
		return;

	fprintf(fp, "%s", string);
	fclose(fp);

	/*if (g_pConsole && cvar.debug_console)
		g_pConsole->DPrintf(string);

	if (cvar.debug_visuals)
		g_ScreenLog.Log(ImColor(IM_COL32_WHITE), string);*/
}

void UnloadCheatDLL()
{
	g_Globals.m_bUnloadLibrary = true;
}

bool IsUnloadingCheatDLL()
{
	return g_Globals.m_bUnloadLibrary;
}

bool FileExists(const char* _FileName)
{
	return _access(_FileName, 0) != -1;
}

float fInterp(float s1, float s2, float s3, float f1, float f3)
{
	if (s2 == s1)
		return f1;

	if (s2 == s3)
		return f3;

	if (s3 == s1)
		return f1;

	return f1 + ((s2 - s1) / (s3 - s1)) * (f3 - f1);
}