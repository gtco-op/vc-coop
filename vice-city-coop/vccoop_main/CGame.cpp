#include "main.h"

DWORD dwCurPlayerActor = 0;
CPed * localPlayer = NULL;
bool usedPlayerID[MAX_PLAYERS];
int playerids = 1;
int currentPlayerID = 0;

GTA_CONTROLSET localPlayerKeys;
CAMERA_AIM localPlayerLookFrontX;
BYTE localPlayerCameraMode;

GTA_CONTROLSET remotePlayerKeys[MAX_PLAYERS];
CAMERA_AIM remotePlayerLookFrontX[MAX_PLAYERS];
int remotePlayerCameraMode[MAX_PLAYERS];

static bool scriptProcessed = false;

WNDPROC		orig_wndproc;
HWND		orig_wnd;
void Hook_CRunningScript__Process();

CGame::CGame()
{
	this->InitPreGamePatches();

	gLog->Log("[CGame] CGame initialized.\n");
}
CGame::~CGame()
{
	gLog->Log("[CGame] CGame shutting down.\n");
}
void CGame::Run()
{

}
int CGame::GetFreePlayerID()
{
	for (int i = 1; i < MAX_PLAYERS; i++)
	{
		if (usedPlayerID[i] == false)
		{
			usedPlayerID[i] = true;
			return i;
		}
	}
	return -1;
}
void InitGameVariables()
{
	memset(&localPlayerKeys, 0, sizeof(GTA_CONTROLSET));
	memset(&localPlayerLookFrontX, 0, sizeof(CAMERA_AIM));
	localPlayerCameraMode = 0;

	memset(&remotePlayerKeys[0], 0, sizeof(GTA_CONTROLSET) * MAX_PLAYERS);
	memset(&remotePlayerLookFrontX[0], 0, sizeof(CAMERA_AIM) * MAX_PLAYERS);
	memset(&remotePlayerCameraMode[0], 0, sizeof(int) * MAX_PLAYERS);

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		gGame->players[i] = (DWORD)0;
		usedPlayerID[i] = false;
	}
}
int GetIDFromPed(DWORD ped)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (gGame->players[i] == ped) return i;
	}
	return 0;
}
void  _declspec(naked) CPlayerPed_ProcessControl_Hook()
{
	_asm mov dwCurPlayerActor, ecx
	_asm pushad

	currentPlayerID = GetIDFromPed(dwCurPlayerActor);

	localPlayer = FindPlayerPed();
	if (dwCurPlayerActor && localPlayer && currentPlayerID != 0)
	{
		// key switching
		localPlayerKeys = *(GTA_CONTROLSET*)0x7DBCB0;
		// set remote player's keys
		*(GTA_CONTROLSET*)0x7DBCB0 = remotePlayerKeys[currentPlayerID];

		// save the internal cammode.
		localPlayerCameraMode = MemRead<u8>(0x7E481C);

		// onfoot mouse looking mode.
		MemWrite<u8>(0x7E481C, 4); 

		// aim switching
		localPlayerLookFrontX = *(CAMERA_AIM*)0x7E4978;
		*(CAMERA_AIM*)0x7E4978 = remotePlayerLookFrontX[currentPlayerID];

		MemWrite<BYTE>(0xA10AFB, currentPlayerID);

		// call the internal CPlayerPed[]::Process
		_asm popad
		_asm mov edx, 0x537270
		_asm call edx
		_asm pushad

		// restore the camera mode.
		MemWrite<u8>(0x7E481C, localPlayerCameraMode);

		// restore the local player's keys and the internal ID.
		MemWrite<BYTE>(0xA10AFB, 0);

		*(GTA_CONTROLSET*)0x7DBCB0 = localPlayerKeys;
		*(CAMERA_AIM*)0x7E4978 = localPlayerLookFrontX;
	}
	else // it's the local player
	{
		_asm popad
		_asm mov edx, 0x537270
		_asm call edx
		_asm pushad
	}

	_asm popad
	_asm ret
}

void InstallMethodHook(DWORD dwInstallAddress,
	DWORD dwHookFunction)
{
	DWORD oldProt, oldProt2;
	VirtualProtect((LPVOID)dwInstallAddress, 4, PAGE_EXECUTE_READWRITE, &oldProt);
	*(PDWORD)dwInstallAddress = (DWORD)dwHookFunction;
	VirtualProtect((LPVOID)dwInstallAddress, 4, oldProt, &oldProt2);
}

void CGame::InitPreGamePatches()
{
	//disable gamestate initialize
	MakeNop(0x601B3B, 10);

	//Set game state to loaded
	MemWrite<u32>(0x9B5F08, 5);

	//Set bStartGame to 1
	MemWrite<u8>(0x869641, 1);

	//Set bMenuVisible = 0
	MemWrite<u8>(0x869668, 0);

	//Set bGameLoaded = 1
	MemWrite<u8>(0x86969C, 1);

	// fix CPedStats::GetPesStatType crash
	Call(0x530260, 0);//initialize probably

	// Don't load the SCM Script
	MakeShortJmp(0x4506D6, 0x45070E);

	// Skip loading screen
	MakeRet(0x4A69D0);

	// Skip splash screen when entering another city
	//MakeRet(0x4A6E80);
	MakeNop(0x48DD7E, 5);
	MakeNop(0x40E157, 5);

	MakeNop(0x62A322, 36);//Dont pause the game when checking for collisions
	MakeNop(0x62A34E, 5);//Dont resume because its not paused

	// Hook script process (so we can spawn a local player)
	MakeCall(0x450245, Hook_CRunningScript__Process);

	//Set fps limit
	MemWrite(0x602D68, 500); 

	// Disable re-initialization of DirectInput mouse device by the game
	MakeNop(0x49908B, 5);
	MakeNop(0x498F92, 5);
	MakeNop(0x499014, 5);

	MakeNop(0x49908B + 0x7, 5);
	MakeNop(0x498F92 + 0x7, 5);
	MakeNop(0x499014 + 0x7, 5);

	//-----Disable menu
	//Disable CMenuManage::Process
	MakeRet(0x49A01C);

	//Disable CMenuManager::DrawBackground
	MakeRet(0x4A212D);

	//Disable CMenuManager::DrawFrontEnd
	MakeRet(0x4A37A4);

	//Disable CMenuManager::LoadAllTextures
	MakeRet(0x4A3A13);

	//Disable menu after focus loss
	MakeRet(0x4A4FD0);

	// Disable CCarCtrl::GenerateRandomCars
	MakeRet(0x4292A0);

	// Disable CCarCtrl::GenerateOneRandomCar
	MakeRet(0x426DB0);

	// Disable CPopulation::AddToPopulation()
	//MakeRet(0x53BA80);

	// Disable CPopulation::AddPedsAtStartOfGame()
	//MakeRet(0x53E3E0);

	//Pedpool inc
	MemWrite<s32>(0x4C02C8, 1000);
	//vehicle pool inc todo: fix crash
	//MemWrite<s32>(0x4C02EA, 250);
	MemCpy((void*)0x4C02E4, "\x6A\x00\x68\xC8\x00\x00\x00", 7);

	//Nop ped spawns
	//MakeNop(0x53E5C6, 5);
	MakeNop(0x53E99B, 5);

	// Disable CCarCtrl::GenerateRandomCars
	MakeRet(0x4292A0);

	// Disable CCarCtrl::GenerateOneRandomCar
	MakeRet(0x426DB0);

	//disable cworld:remove in CPopulation::ManagePopulation
	MakeRet(0x53D690);

	//Nop removals in CPopulation::Remove
	MakeNop(0x53D896, 23);
	MakeNop(0x53D82F, 23);
	MakeNop(0x53D9E5, 19);
	MakeNop(0x53DBEC, 32);
	MakeNop(0x53E2E6, 12);

	//Disable CPopulation::Removeped
	MakeRet(0x53B160);

	//A Cworld crash fix
	MakeNop(0x531D40, 8);

	InstallMethodHook(0x694D90, (DWORD)CPlayerPed_ProcessControl_Hook);

	gLog->Log("[CGame] InitPreGamePatches() finished.\n");
}

void CGame::EnableMouseInput()
{
	//Enable CPad:UpdateMouse
	MemCpy((void*)0x4AD820, "\x53", 1);
	//CControllerConfigManager::AffectPadFromKeyBoard restore
	MemCpy((void*)0x4AB6E6, "\xE8\x45\xCE\x16\x00", 5);
	//CControllerConfigManager::AffectPadFromMouse restore
	MemCpy((void*)0x4AB6F0, "\xE8\x9B\xCD\x16\x00", 5);

	gRender->device->ShowCursor(false);
	ImGui::GetIO().MouseDrawCursor = false;
}
void CGame::DisableMouseInput()
{
	//Disable CPad:UpdateMouse
	MakeRet(0x4AD820);
	//CControllerConfigManager::AffectPadFromKeyBoard nop
	MakeNop(0x4AB6E6, 5);
	//CControllerConfigManager::AffectPadFromMouse nop
	MakeNop(0x4AB6F0, 5);

	gRender->device->ShowCursor(true);
	ImGui::GetIO().MouseDrawCursor = true;
}
void Hook_CRunningScript__Process()
{
	if (!scriptProcessed)
	{
		// Change player model ID
		MemWrite<u8>(0x5384FA + 1, 7);

		// CPlayerPed::SetupPlayerPed
		Call(0x5383E0, 0);

		// Set player position
		FindPlayerPed()->Teleport({ VCCOOP_DEFAULT_SPAWN_POSITION });

		// CStreaming::LoadScene
		CVector scenePosition(VCCOOP_DEFAULT_SPAWN_POSITION);
		Call(0x40AF60, &scenePosition);

		CWorld::Players[0].m_bNeverGetsTired = true;

		CPools::ms_pPedPool->Clear();
		CPools::ms_pVehiclePool->Clear();

		CPools::ms_pPedPool->Init(1000, NULL, NULL);
		CPools::ms_pVehiclePool->Init(200, NULL, NULL);

		InitGameVariables();

		gLog->Log("[CGame] CRunningScript::Process() hook finished.\n");

		gRender->ToggleGUI();

		// First tick processed
		scriptProcessed = true;
	}
}

CPlayerPed * CGame::GamePool_Ped_GetAt(int iID)
{
	CPlayerPed *pActorRet;

	_asm mov ebx, 0x97F2AC
	_asm mov ecx, [ebx]
		_asm push iID
	_asm mov ebx, 0x451CB0
	_asm call ebx
	_asm mov pActorRet, eax

	return pActorRet;
}

//-----------------------------------------------------------

int CGame::GamePool_Ped_GetIndex(CPed *pPed)
{
	int iRetVal;

	_asm mov ebx, 0x97F2AC
	_asm mov ecx, [ebx]
		_asm push pPed
	_asm mov ebx, 0x451CF0
	_asm call ebx
	_asm mov iRetVal, eax

	return iRetVal;
}

LRESULT CALLBACK wnd_proc(HWND wnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(wnd, umsg, wparam, lparam)) return 0;
	
	switch (umsg)
	{
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			int vkey = (int)wparam;
			if (vkey == 'P')
			{
				librg_message_send_all(&gNetwork->ctx, VCOOP_CREATE_PED, NULL, 0);
			}
			if (vkey == 'T')
			{
				if (!gChat->chatToggled && !gRender->bGUI)
					gChat->ToggleChat(true);
			}
			if (vkey == VK_ESCAPE)
			{
				if(gChat->chatToggled) gChat->ToggleChat(false);
			}
			if (vkey == VK_F7 && gNetwork->connected)
			{
				gNetwork->StopClientThread(); 
				gLog->Log("[CGame] Disconnecting from server.\n");
			}
			else if (vkey == VK_F8)
			{
				gRender->ToggleGUI(); 
				gLog->Log("[CGame] Toggling GUI.\n");
			}
			else if (vkey == VK_F9)
			{
				FindPlayerPed()->Teleport({ VCCOOP_DEFAULT_SPAWN_POSITION });
				gLog->Log("[CGame] Teleporting to X: %.f Y: %.f Z: %.f\n", VCCOOP_DEFAULT_SPAWN_POSITION);
			}
			else if (vkey == VK_F10 && !gNetwork->connected) // crashfix
			{
				gNetwork->AttemptConnect("127.0.0.1", VCCOOP_DEFAULT_SERVER_PORT);
				gLog->Log("[CGame] Attempting to connect to local server\n");
			}
			break;
		}
	}
	return CallWindowProc(orig_wndproc, wnd, umsg, wparam, lparam);
}