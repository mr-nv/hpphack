#include "framework.h"

CPageList::CPageList() : 
	m_current_index(0), 
	m_next_index(0), 
	m_swap_index(0), 
	m_anim_state(0), 
	m_last_time(0.f)
{
}

CPageList::~CPageList()
{
}

bool CPageList::Empty()
{
	return m_page_list.empty();
}

void CPageList::Push(const char* name, std::function<void()> fn)
{
	PageInfo page;
	page.name = name;
	page.index = m_next_index++;
	page.fn = fn;
	m_page_list.push_back(page);
}

void CPageList::Swap(const char* name)
{
	for (auto& page : m_page_list)
	{
		if (strcmp(page.name, name))
			continue;

		m_swap_index = page.index;
		break;
	}

	if (m_anim_state & PageAnim_None)
		m_last_time = g_GlobalVars.curtime;

	if (m_swap_index != m_current_index)
		m_anim_state = PageAnim_Close;
}

void CPageList::Call(const char* name)
{
	for (auto& page : m_page_list)
	{
		if (strcmp(page.name, name))
			continue;

		m_page_list[page.index].fn();
		break;
	}
}

void CPageList::Run()
{
	const auto anim_duration = 0.050f; // 50 milliseconds

	ImGuiStyle& style = ImGui::GetStyle();

	switch (m_anim_state)
	{
	case PageAnim_Open:
		style.Alpha = Lerp(g_GlobalVars.curtime, m_last_time, anim_duration, 0.0f, 1.0f);
		style.Alpha = std::clamp<float>(style.Alpha, 0.0f, 1.0f);

		if (m_last_time + anim_duration < g_GlobalVars.curtime)
		{
			m_anim_state = PageAnim_None;
			m_swap_index = -1;
		}
		break;
	case PageAnim_Close:
		style.Alpha = Lerp(g_GlobalVars.curtime, m_last_time, anim_duration, 1.0f, 0.0f);
		style.Alpha = std::clamp<float>(style.Alpha, 0.0f, 1.0f);

		if (m_last_time + anim_duration < g_GlobalVars.curtime)
		{
			m_anim_state = PageAnim_Open;
			m_current_index = m_swap_index;
			m_last_time = g_GlobalVars.curtime;
		}
	}

	m_page_list[m_current_index].fn();
}

void CPageList::Clear()
{
	m_page_list.clear();
	m_current_index = 0;
	m_next_index = 0;
	m_swap_index = 0;
	m_anim_state = 0;
	m_last_time = 0.f;
}

template<typename T>
T CPageList::Lerp(float curtime, float prevtime, float duration, T a, T b)
{
	return static_cast<T>(a + ((curtime - prevtime) / duration) * (b - a));
}