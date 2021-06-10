#include "main.h"

CCvars cvar;

CCvars::CCvars()
{
	RtlSecureZeroMemory(&legitbot, sizeof(legitbot));
	RtlSecureZeroMemory(&ragebot, sizeof(ragebot));
	RtlSecureZeroMemory(&visuals, sizeof(visuals));
	RtlSecureZeroMemory(&kreedz, sizeof(kreedz));
	RtlSecureZeroMemory(&misc, sizeof(misc));
	RtlSecureZeroMemory(&settings, sizeof(settings));
	RtlSecureZeroMemory(gui, sizeof(gui));
}