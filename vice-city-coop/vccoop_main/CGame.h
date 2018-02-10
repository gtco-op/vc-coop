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

	void Run();

	void EnableHUD();
	void DisableHUD();

	CVector GetCameraPos();

	int remotePlayers = 1;
	CPed * remotePlayerPeds[MAX_PLAYERS];
	GTA_CONTROLSET remotePlayerKeys[MAX_PLAYERS];
	CAMERA_AIM remotePlayerLookFrontX[MAX_PLAYERS];
	int remotePlayerCameraMode[MAX_PLAYERS];

	bool debugEngineOutput = false;

private:
	void InitPreGamePatches(); 
};


LRESULT CALLBACK wnd_proc(HWND wnd, UINT umsg, WPARAM wparam, LPARAM lparam);