#pragma once
class CGame 
{
public:
	CGame();
	~CGame() {}
	
	std::string Name;

	HWND g_hWnd;
	CPad* pad;

	void EnableMouseInput();
	void DisableMouseInput();

	void SetCoordBlip(CVector coord, uint unk, eBlipDisplay blipDisplay);
	
	bool IsWindowActive();

	void Run();
	void OnConnected();
	void Exit();

	void StartMissionScript();

	static void HasGameLoaded();

	void EnableHUD();
	void DisableHUD();

	CObject* SpawnModel(int modelid, CVector position, RpClump* clump);
	void CustomModelLoad(int id);

	int remotePlayers = 1;
	CPed * remotePlayerPeds[MAX_PLAYERS];
	CPad remotePlayerKeys[MAX_PLAYERS];
	CAMERA_AIM remotePlayerLookFrontX[MAX_PLAYERS];
	int remotePlayerCameraMode[MAX_PLAYERS];
	CPed *FindLocalPed();

	bool debugEngineOutput = false;
	
	static bool bLoadingDone;
	
	bool bMissionScriptStarted = false;

	static int keyPressTime;

	CVehicle * CreateVehicle(int modelIndex, CVector position);

private:
	void InitPreGamePatches(); 
};


LRESULT CALLBACK wnd_proc(HWND wnd, UINT umsg, WPARAM wparam, LPARAM lparam);