#include "esp\esp.h"

class CVisuals : public CESP
{
public:
	CVisuals();
	~CVisuals();

	CVisuals(const CVisuals&) = delete;

	void Draw();
};

extern CVisuals g_Visuals;