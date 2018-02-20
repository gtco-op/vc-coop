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

	void SetCoordBlip(CVector coord, uint unk, eBlipDisplay blipDisplay);
	
	bool IsWindowActive();

	void Run();

	void EnableHUD();
	void DisableHUD();

	void SetPlayerCameraPosition(float fX, float fY, float fZ, float fRotationX, float fRotationY, float fRotationZ);
	void CameraLookAtPoint(float fX, float fY, float fZ, int iType);
	void SetCameraBehindPlayer();
	void RestoreCamera();

	CVector GetCameraPos();

	int remotePlayers = 1;
	CPed * remotePlayerPeds[MAX_PLAYERS];
	GTA_CONTROLSET remotePlayerKeys[MAX_PLAYERS];
	CAMERA_AIM remotePlayerLookFrontX[MAX_PLAYERS];
	int remotePlayerCameraMode[MAX_PLAYERS];
	CPed *FindLocalPed();

	void WaitUntilTheModelIsLoaded(int model);

	bool debugEngineOutput = false;

	static int keyPressTime;

private:
	void InitPreGamePatches(); 
};


LRESULT CALLBACK wnd_proc(HWND wnd, UINT umsg, WPARAM wparam, LPARAM lparam);