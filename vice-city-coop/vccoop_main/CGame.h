#pragma once

class CGame 
{
public:
	CGame();
	~CGame();
	
	HWND g_hWnd;

	CPad* pad;
	void EnableMouseInput();
	void DisableMouseInput();

	void Run();

private:
	void InitPreGamePatches(); 
};


LRESULT CALLBACK wnd_proc(HWND wnd, UINT umsg, WPARAM wparam, LPARAM lparam);