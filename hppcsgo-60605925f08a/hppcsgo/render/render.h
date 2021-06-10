#include "render_drawlist.h"

#include <functional>

class CRender : public CDrawList
{
public:
	CRender();
	~CRender();

	void Initialize();
	void Shutdown();

	void BeginScene(std::function<void()> RenderScene);

	ImDrawList* RenderScene();

private:
	std::shared_ptr<ImDrawList> m_pDrawListAct;
	std::shared_ptr<ImDrawList> m_pDrawListRendering;
};

extern CRender g_Render;