#include "menu_keynum_assign.h"

#include "..\..\..\ValveSDK\misc\vfunc.hpp"
#include "..\..\..\ValveSDK\interfaces\IInputSystem.hpp"

CMenuKeynumAssign::CMenuKeynumAssign() :
	m_assignment(false), m_keynum(KEY_NONE)
{
}

CMenuKeynumAssign::~CMenuKeynumAssign()
{
}

bool CMenuKeynumAssign::IsKeyAssignment()
{
	return m_assignment;
}

void CMenuKeynumAssign::KeyAssignmentStart()
{
	m_assignment = true;
	m_keynum = KEY_NONE;
}

int CMenuKeynumAssign::GetAssigmentKeynum()
{
	return m_keynum;
}

void CMenuKeynumAssign::KeyAssignment(unsigned code)
{
	switch (code)
	{
	case 0x01: m_keynum = MOUSE_LEFT; break;
	case 0x02: m_keynum = MOUSE_RIGHT; break;
	case 0x03: m_keynum = MOUSE_MIDDLE; break;
	case 0x04: m_keynum = MOUSE_4; break;
	case 0x05: m_keynum = MOUSE_5; break;
	case 0x08: m_keynum = KEY_BACKSPACE; break;
	case 0x09: m_keynum = KEY_TAB; break;
	case 0x10: m_keynum = KEY_LSHIFT; break;
	case 0x11: m_keynum = KEY_LCONTROL; break;
	case 0x12: m_keynum = KEY_LALT; break;
	case 0x14: m_keynum = KEY_CAPSLOCK; break;
	case 0x0D: m_keynum = KEY_ENTER; break;
	case 0x20: m_keynum = KEY_SPACE; break;
	case 0x21: m_keynum = KEY_PAGEUP; break;
	case 0x22: m_keynum = KEY_PAGEDOWN; break;
	case 0x23: m_keynum = KEY_END; break;
	case 0x24: m_keynum = KEY_HOME; break;
	case 0x25: m_keynum = KEY_LEFT; break;
	case 0x26: m_keynum = KEY_UP; break;
	case 0x27: m_keynum = KEY_RIGHT; break;
	case 0x28: m_keynum = KEY_DOWN; break;
	case 0x2D: m_keynum = KEY_INSERT; break;
	case 0x2E: m_keynum = KEY_DELETE; break;
	case 0x30: m_keynum = KEY_0; break;
	case 0x31: m_keynum = KEY_1; break;
	case 0x32: m_keynum = KEY_2; break;
	case 0x33: m_keynum = KEY_3; break;
	case 0x34: m_keynum = KEY_4; break;
	case 0x35: m_keynum = KEY_5; break;
	case 0x36: m_keynum = KEY_6; break;
	case 0x37: m_keynum = KEY_7; break;
	case 0x38: m_keynum = KEY_8; break;
	case 0x39: m_keynum = KEY_9; break;
	case 0x41: m_keynum = KEY_A; break;
	case 0x42: m_keynum = KEY_B; break;
	case 0x43: m_keynum = KEY_C; break;
	case 0x44: m_keynum = KEY_D; break;
	case 0x45: m_keynum = KEY_E; break;
	case 0x46: m_keynum = KEY_F; break;
	case 0x47: m_keynum = KEY_G; break;
	case 0x48: m_keynum = KEY_H; break;
	case 0x49: m_keynum = KEY_I; break;
	case 0x4A: m_keynum = KEY_J; break;
	case 0x4B: m_keynum = KEY_K; break;
	case 0x4C: m_keynum = KEY_L; break;
	case 0x4D: m_keynum = KEY_M; break;
	case 0x4E: m_keynum = KEY_N; break;
	case 0x4F: m_keynum = KEY_O; break;
	case 0x50: m_keynum = KEY_P; break;
	case 0x51: m_keynum = KEY_Q; break;
	case 0x52: m_keynum = KEY_R; break;
	case 0x53: m_keynum = KEY_S; break;
	case 0x54: m_keynum = KEY_T; break;
	case 0x55: m_keynum = KEY_U; break;
	case 0x56: m_keynum = KEY_V; break;
	case 0x57: m_keynum = KEY_W; break;
	case 0x58: m_keynum = KEY_X; break;
	case 0x59: m_keynum = KEY_Y; break;
	case 0x5A: m_keynum = KEY_Z; break;
	case 0x60: m_keynum = KEY_PAD_0; break;
	case 0x61: m_keynum = KEY_PAD_1; break;
	case 0x62: m_keynum = KEY_PAD_2; break;
	case 0x63: m_keynum = KEY_PAD_3; break;
	case 0x64: m_keynum = KEY_PAD_4; break;
	case 0x65: m_keynum = KEY_PAD_5; break;
	case 0x66: m_keynum = KEY_PAD_6; break;
	case 0x67: m_keynum = KEY_PAD_7; break;
	case 0x68: m_keynum = KEY_PAD_8; break;
	case 0x69: m_keynum = KEY_PAD_9; break;
	case 0x6A: m_keynum = KEY_PAD_MULTIPLY; break;
	case 0x6B: m_keynum = KEY_PAD_PLUS; break;
	case 0x6D: m_keynum = KEY_PAD_MINUS; break;
	case 0x6E: m_keynum = KEY_PAD_DECIMAL; break;
	case 0x6F: m_keynum = KEY_PAD_DIVIDE; break;
	case 0x70: m_keynum = KEY_F1; break;
	case 0x71: m_keynum = KEY_F2; break;
	case 0x72: m_keynum = KEY_F3; break;
	case 0x73: m_keynum = KEY_F4; break;
	case 0x74: m_keynum = KEY_F5; break;
	case 0x75: m_keynum = KEY_F6; break;
	case 0x76: m_keynum = KEY_F7; break;
	case 0x77: m_keynum = KEY_F8; break;
	case 0x78: m_keynum = KEY_F9; break;
	case 0x79: m_keynum = KEY_F10; break;
	case 0x7A: m_keynum = KEY_F11; break;
	case 0x7B: m_keynum = KEY_F12; break;
	case 0xBB: m_keynum = KEY_EQUAL; break;
	case 0xBC: m_keynum = KEY_COMMA; break;
	case 0xBD: m_keynum = KEY_MINUS; break;
	case 0xBE: m_keynum = KEY_PERIOD; break;
	case 0xDB: m_keynum = KEY_LBRACKET; break;
	case 0xDD: m_keynum = KEY_RBRACKET; break;
	default: m_keynum = KEY_NONE; break;
	}

	m_assignment = false;
}