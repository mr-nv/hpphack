#include "menu_control.h"

CMenuControl::CMenuControl() : m_IsOpened(true)
{
}

CMenuControl::~CMenuControl()
{
}

bool CMenuControl::IsOpened()
{
	return m_IsOpened;
}

void CMenuControl::Open()
{
	if (!m_IsOpened)
		m_IsOpened = true;
}

void CMenuControl::Close()
{
	if (m_IsOpened)
		m_IsOpened = false;
}

void CMenuControl::Toggle()
{
	m_IsOpened = !m_IsOpened;
}