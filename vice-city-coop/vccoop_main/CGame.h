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
	void Exit();

	static void HasGameLoaded();

	void EnableHUD();
	void DisableHUD();

	void CustomModelLoad(int id);

	int remotePlayers = 1;
	CPed * remotePlayerPeds[MAX_PLAYERS];
	CPad remotePlayerKeys[MAX_PLAYERS];
	CAMERA_AIM remotePlayerLookFrontX[MAX_PLAYERS];
	int remotePlayerCameraMode[MAX_PLAYERS];
	CPed *FindLocalPed();

	bool debugEngineOutput = false;
	
	static bool bLoadingDone;

	static int keyPressTime;

	CVehicle * CreateVehicle(unsigned int modelIndex, CVector position);

private:
	void InitPreGamePatches(); 
};


LRESULT CALLBACK wnd_proc(HWND wnd, UINT umsg, WPARAM wparam, LPARAM lparam);