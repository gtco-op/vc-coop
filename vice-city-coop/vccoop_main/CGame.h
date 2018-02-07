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

	int GamePool_Ped_GetIndex(CPed *pPed);
	CPlayerPed * CGame::GamePool_Ped_GetAt(int iID);

	DWORD players[MAX_PLAYERS];
	GTA_CONTROLSET remotePlayerKeys[MAX_PLAYERS];
	CAMERA_AIM remotePlayerLookFrontX[MAX_PLAYERS];
	int remotePlayerCameraMode[MAX_PLAYERS];

	void Run();

	int GetFreePlayerID();

private:
	void InitPreGamePatches(); 
};


LRESULT CALLBACK wnd_proc(HWND wnd, UINT umsg, WPARAM wparam, LPARAM lparam);