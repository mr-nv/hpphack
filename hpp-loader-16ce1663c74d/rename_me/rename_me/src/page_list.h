#pragma once

struct PageInfo
{
	const char* name;
	int index;
	std::function<void()> fn;
};

class CPageList
{
private:
	using PageList = std::vector<PageInfo>;

public:
	CPageList();
	~CPageList();

	bool Empty();
	void Push(const char* name, std::function<void()> fn);
	void Swap(const char* name);
	void Call(const char* name);
	void Run();
	void Clear();

private:
	enum PageAnimList
	{
		PageAnim_None = 1,
		PageAnim_Open,
		PageAnim_Close
	};

	PageList m_page_list;

	int m_current_index;
	int m_next_index;
	int m_swap_index;
	int m_anim_state;
	float m_last_time;

	template<typename T>
	T Lerp(float curtime, float prevtime, float duration, T a, T b);
};