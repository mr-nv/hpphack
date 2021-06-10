#include <deque>

#include "..\..\..\..\ValveSDK\math\Vector.hpp"

struct SoundData
{
	float time;
	Vector origin;
	int index;
};

class CSoundList
{
public:
	CSoundList();
	~CSoundList();

	void GetActiveSounds();

protected:
	std::deque<SoundData>& GetSoundData();

private:	
	std::deque<SoundData> m_SoundData;

	void PushSound(const Vector& origin, int index);
	void PlaySound(const Vector& origin, int index);
};