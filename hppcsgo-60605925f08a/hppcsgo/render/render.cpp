#include "render.h"

#include <Windows.h>
#include <mutex>
#include <string>
#include <d3d9.h>

#include "..\globals.h"
#include "..\imgui\impl\imgui_impl_win32.h"
#include "..\imgui\impl\imgui_impl_dx9.h"

extern IDirect3DDevice9* g_pID3DDevice9;

static std::mutex render_mutex;

CRender::CRender() : m_pDrawListAct(nullptr), m_pDrawListRendering(nullptr)
{
}

CRender::~CRender()
{
}

void CRender::Initialize()
{
	ImGui_ImplWin32_Init(g_Globals.m_hWnd);
	ImGui_ImplDX9_Init(g_pID3DDevice9);

	m_pDrawList = std::make_shared<ImDrawList>(ImDrawList(ImGui::GetDrawListSharedData()));
	m_pDrawListAct = std::make_shared<ImDrawList>(ImDrawList(ImGui::GetDrawListSharedData()));
	m_pDrawListRendering = std::make_shared<ImDrawList>(ImDrawList(ImGui::GetDrawListSharedData()));
}

void CRender::Shutdown()
{
	if (m_pDrawList.get())
		m_pDrawList.reset();

	if (m_pDrawListAct.get())
		m_pDrawListAct.reset();

	if (m_pDrawListRendering.get())
		m_pDrawListRendering.reset();

	if (ImGui::GetCurrentContext())
	{
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
	}
}

void CRender::BeginScene(std::function<void()> RenderScene)
{
	m_pDrawList->Clear();
	m_pDrawList->PushClipRectFullScreen();

	RenderScene();

	render_mutex.lock();
	*m_pDrawListAct = *m_pDrawList;
	render_mutex.unlock();
}

ImDrawList* CRender::RenderScene()
{
	if (render_mutex.try_lock())
	{
		*m_pDrawListRendering = *m_pDrawListAct;
		render_mutex.unlock();
	}

	return m_pDrawListRendering.get();
}

CRender g_Render;