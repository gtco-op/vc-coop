#pragma once

class CGame 
{
public:
	CGame();
	~CGame();
	
	std::string Name;

	HWND g_hWnd;
	CPad* pad;

	void EnableMouseInput();
	void DisableMouseInput();

	void PatchAddToPopulation();
	void UnpatchAddToPopulation();

	void Run();

private:
	void InitPreGamePatches(); 
};


LRESULT CALLBACK wnd_proc(HWND wnd, UINT umsg, WPARAM wparam, LPARAM lparam);