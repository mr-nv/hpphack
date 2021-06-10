#include "player\esp_player.h"
#include "sound\esp_sound.h"

class CESP : 
	public CDrawPlayer, 
	public CDrawSound
{
public:
	CESP();
	~CESP();

protected:
	void DrawESP();
};