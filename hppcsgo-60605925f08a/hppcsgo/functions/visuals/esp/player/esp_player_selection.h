#include <vector>

struct SelectionData
{
	float distance;
	int index;
};

class CSelectionPlayers
{
public:
	CSelectionPlayers();
	~CSelectionPlayers();

protected:
	std::vector<SelectionData>& GetSelectionPlayers();

private:
	void PushPlayer(std::vector<SelectionData>& data, int index);
	std::vector<SelectionData>& SortSelectionPlayers(const std::vector<SelectionData>& data);
};