#pragma once

class CGuard
{
public:
	CGuard();
	~CGuard();

	void Run();

private:
	bool CheckProcessHacker();
	bool CheckCheatEngine();
	bool CheckIDAPro();
	bool CheckOllyDbg();
	bool CheckWireshark();
	bool CheckHttpAnalyzer();
	bool Checkx96dbg();
	bool CheckProcessExplorer();
};