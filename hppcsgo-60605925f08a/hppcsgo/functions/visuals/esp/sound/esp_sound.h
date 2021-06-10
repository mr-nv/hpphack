#include "esp_sound_list.h"

#include "..\..\..\..\imgui\imgui.h"

class CDrawSound : public CSoundList
{
public:
	CDrawSound();
	~CDrawSound();

protected:
	void DrawSound();

private:
	enum SoundTypesList
	{
		SoundType_Boxes,
		SoundType_Circles,
	};

	float InterpolateFadeout(float time, float start, float end);

	ImColor GetTeamColor(int index);
	ImColor GetProcessedColor(const SoundData& data);

	void Circle(const SoundData& data);
	void Box(const SoundData& data, float* flScreenPos);
};