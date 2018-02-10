#include "main.h"

static bool		scriptProcessed = false;
WNDPROC			orig_wndproc;
HWND			orig_wnd;

DWORD			dwCurPlayerActor = 0;
CPed*			localPlayer = NULL;
GTA_CONTROLSET	localPlayerKeys;
CAMERA_AIM		localPlayerLookFrontX;
BYTE			localPlayerCameraMode;
int				currentPlayerID = 0;

void Hook_CRunningScript__Process();

CGame::CGame()
{
	this->InitPreGamePatches();
	keyPressTime = 0;

	gLog->Log("[CGame] CGame initialized.\n");
}
CGame::~CGame()
{
	gLog->Log("[CGame] CGame shutting down.\n");
}
void CGame::Run()
{
	if (GetAsyncKeyState(0x2C) & 1 && CTimer::m_snTimeInMilliseconds - keyPressTime > 500 && IsWindowActive())
	{
		keyPressTime = CTimer::m_snTimeInMilliseconds;
		gRender->TakeScreenshot();
	}
#ifdef VCCOOP_DEBUG
	if (KeyPressed(223) && CTimer::m_snTimeInMilliseconds - keyPressTime > 500 && IsWindowActive())
	{
		keyPressTime = CTimer::m_snTimeInMilliseconds;
		gRender->bConsole = !gRender->bConsole;
		ImGui::GetIO().ClearInputCharacters();
	}
#endif
	if (KeyPressed('T') && CTimer::m_snTimeInMilliseconds - keyPressTime > 500 && !gChat->chatToggled && IsWindowActive())
	{
		keyPressTime = CTimer::m_snTimeInMilliseconds;
		if (!gChat->chatToggled && !gRender->bGUI && !gRender->bConnecting && !gRender->bAboutWindow)
			gChat->ToggleChat(true); ImGui::GetIO().ClearInputCharacters(); 
	}
}
bool CGame::IsWindowActive()
{
	return (GetActiveWindow() == orig_wnd ? true : false);
}
void CGame::SetPlayerCameraPosition(float fX, float fY, float fZ, float fRotationX, float fRotationY, float fRotationZ)
{
	Command<0x015F>(fX, fY, fZ, fRotationX, fRotationY, fRotationZ);
}
void CGame::CameraLookAtPoint(float fX, float fY, float fZ, int iType)
{
	Command<0x0160>(fX, fY, fZ, iType);
}
CVector CGame::GetCameraPos()
{
	return CVector(MemRead<float>(0x7E46B8), MemRead<float>(0x7E46BC), MemRead<float>(0x7E46C0));
}
void CGame::DisableHUD()
{
	MemWrite<BYTE>(0x86963A, 0x00);
	MemWrite<BYTE>(0xA10B45, 0x00);
}
void CGame::EnableHUD()
{
	MemWrite<int>(0x86963A, 1);
	MemWrite<int>(0xA10B45, 1);
}
int FindIDForPed(CPed * ped)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (gGame->remotePlayerPeds[i] == ped)return i;
	}
	return -1;
}

void  _declspec(naked) Patched_CPlayerPed__ProcessControl()
{
	_asm mov dwCurPlayerActor, ecx
	_asm pushad

	currentPlayerID = FindIDForPed((CPed*)dwCurPlayerActor);

	//gLog->Log("[CPlayerPed::ProcessControl()] Processing for %d", currentPlayerID);
	localPlayer = FindPlayerPed();

	if (localPlayer && (CPed*)dwCurPlayerActor == localPlayer)
	{
		_asm popad
		_asm mov edx, 0x537270
		_asm call edx
		_asm pushad
	}
	else
	{
		// key switching
		localPlayerKeys = *(GTA_CONTROLSET*)0x7DBCB0;
		// set remote player's keys

		//remotePlayerKeys[currentPlayerID].wKeys1[KEY_ONFOOT_FORWARD] = 0xFF;
		//remotePlayerKeys[currentPlayerID].wKeys2[KEY_ONFOOT_FORWARD] = 0xFF;

		*(GTA_CONTROLSET*)0x7DBCB0 = gGame->remotePlayerKeys[currentPlayerID];

		// save the internal cammode.
		localPlayerCameraMode = MemRead<u8>(0x7E481C);

		// onfoot mouse looking mode.
		MemWrite<u8>(0x7E481C, 4);

		// aim switching
		localPlayerLookFrontX = *(CAMERA_AIM*)0x7E4978;
		*(CAMERA_AIM*)0x7E4978 = gGame->remotePlayerLookFrontX[currentPlayerID];

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
	_asm popad
	_asm ret
}

//pPlayerKeys.wKeys1[KEY_ONFOOT_FORWARD] = 0xFF;
//pPlayerKeys.wKeys2[KEY_ONFOOT_FORWARD] = 0xFF;

void InstallMethodHook(DWORD dwInstallAddress,
	DWORD dwHookFunction)
{
	DWORD oldProt, oldProt2;
	VirtualProtect((LPVOID)dwInstallAddress, 4, PAGE_EXECUTE_READWRITE, &oldProt);
	*(PDWORD)dwInstallAddress = (DWORD)dwHookFunction;
	VirtualProtect((LPVOID)dwInstallAddress, 4, oldProt, &oldProt2);
}

void LogDebug(char * msg, ...)
{
	char buffer[256];
	bool newline = false;

	va_list args;

	va_start(args, msg);
	vsprintf(buffer, msg, args);
	va_end(args);

	if (!strstr(buffer, "\n"))	{
		newline = true;
	}
#ifdef VCCOOP_DEBUG
	if (gRender->gDebugScreen->gDevConsole != nullptr && debugEnabled) {
		gRender->gDebugScreen->gDevConsole->AddLog("%s%s", buffer, (newline ? "\n" : ""));
		gRender->gDebugScreen->gDbgLog->Log(" %s%s", buffer, (newline ? "\n" : ""));
	}
#endif
	return;
}

char cdstream[65];
void CGame::InitPreGamePatches()
{
	#ifdef VCCOOP_DEBUG_ENGINE
	for (int i = 0x401000; i < 0x67DD05; i++)
	{
		if (MemRead<BYTE>(i) == (BYTE)0xE8)
		{
			MemoryPointer at = ReadRelativeOffset(i + 1);
			if (at == 0x401000)
			{
				MakeCall(i, LogDebug);
			}
		}
	}
	debugEnabled = true;
	#endif
	
	//Allow multiple instances of the game
	sprintf(cdstream, "vcc%u", GetTickCount());
	MakePushOffset(0x408967, cdstream);


	/*
	//MemWrite<u8>(0x408887,0x74);
	//MakeJmp(0x40FD86, 0x40FDA0);
	MemWrite<u8>(0x580A7F, 0xF);                       // CTxdStore::AddRef patch
	MemWrite<u8>(0x580A2F, 0x22);						// CTxdStore::RemoveRef patch
	MakeNop(0x62A667, 5);*/

	VirtualAlloc((PVOID)0x401001, 5, MEM_COMMIT, PAGE_EXECUTE_READWRITE); 
	
	//MakeRet(0x401006);
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
	//MakeRet(0x4A69D0);

	// Patch to load a random loading screen from array
	const char* splash_screens[] = { "LOADSC1","LOADSC2","LOADSC3","LOADSC4","LOADSC5","LOADSC6","LOADSC7" };
	int r = (rand() % 7);
	gLog->Log("[CGame] Using loading screen %s\n", splash_screens[r]);
	
	DWORD dwVP, dwVP2;
	VirtualProtect((PVOID)0x6D5E9C, 16, PAGE_EXECUTE_READWRITE, &dwVP);
	strcpy((PCHAR)0x6D5E9C, splash_screens[r]);
	VirtualProtect((PVOID)0x6D5E9C, 16, dwVP, &dwVP2);

	VirtualProtect((PVOID)0x68E594, 16, PAGE_EXECUTE_READWRITE, &dwVP);
	strcpy((PCHAR)0x68E594, splash_screens[r]);
	VirtualProtect((PVOID)0x68E594, 16, dwVP, &dwVP2);

	VirtualProtect((PVOID)0x68E6F4, 16, PAGE_EXECUTE_READWRITE, &dwVP);
	strcpy((PCHAR)0x68E6F4, splash_screens[r]);
	VirtualProtect((PVOID)0x68E6F4, 16, dwVP, &dwVP2);

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
	/*
	//-----Disable menu
	//Disable CMenuManage::Process
	MakeRet(0x49A01C);

	//Disable CMenuManager::DrawBackground
	MakeRet(0x4A212D);

	//Disable CMenuManager::DrawFrontEnd
	MakeRet(0x4A37A4);

	//Disable CMenuManager::LoadAllTextures
	MakeRet(0x4A3A13);*/

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
	MakeNop(0x53E5C6, 5); //3peds
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

	InstallMethodHook(0x694D90, (DWORD)Patched_CPlayerPed__ProcessControl);

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
		FindPlayerPed()->Teleport({ CVector(531.629761f, 606.497253f, 10.901563f)});
		
		// CStreaming::LoadScene
		CVector scenePosition(531.629761f, 606.497253f, 10.901563f);
		Call(0x40AF60, &scenePosition);

		gGame->SetPlayerCameraPosition(531.629761f, 606.497253f, 10.901563f, 0, 0, 0);
		gGame->CameraLookAtPoint(531.629761f, 606.497253f, 10.901563f, 1);

		CWorld::Players[0].m_bNeverGetsTired = true;

		CPools::ms_pPedPool->Clear();
		CPools::ms_pVehiclePool->Clear();

		CPools::ms_pPedPool->Init(1000, NULL, NULL);
		CPools::ms_pVehiclePool->Init(200, NULL, NULL);
		
		gLog->Log("[CGame] CRunningScript::Process() hook finished.\n");

		gRender->ToggleGUI();

		gGame->remotePlayerPeds[0] = FindPlayerPed();


		// First tick processed
		scriptProcessed = true;
	}
}

LRESULT CALLBACK wnd_proc(HWND wnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{

		case WM_DESTROY:
		case WM_CLOSE:
			ExitProcess(-1);
			break;

		case WM_MOUSEMOVE:
			POINT ul, lr;
			RECT rect;
			GetClientRect(wnd, &rect);

			ul.x = rect.left;
			ul.y = rect.top;
			lr.x = rect.right;
			lr.y = rect.bottom;

			MapWindowPoints(wnd, nullptr, &ul, 1);
			MapWindowPoints(wnd, nullptr, &lr, 1);

			rect.left = ul.x;
			rect.top = ul.y;
			rect.right = lr.x;
			rect.bottom = lr.y;

			if (gGame->IsWindowActive())
				ClipCursor(&rect);
			break;
		
		case WM_MOUSEHOVER:
			break;
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			int vkey = (int)wparam;
			if (vkey == 'P' && gNetwork->connected 
#ifdef VCCOOP_DEBUG
				&& !gRender->gDebugScreen->consoleToggled
#endif
				)
			{
				librg_message_send_all(&gNetwork->ctx, VCOOP_CREATE_PED, NULL, 0);
			}
			if (vkey == 'Z')
			{
				CClientPlayer * player = new CClientPlayer(0, gGame->remotePlayers);
				gGame->remotePlayerPeds[gGame->remotePlayers] = player->ped;
				gGame->remotePlayers++;
			}
			if (vkey == VK_ESCAPE && gNetwork->connected)
			{
				if(gChat->chatToggled) gChat->ToggleChat(false);
			}
			if (vkey == VK_F7 && gNetwork->connected)
			{
				gNetwork->StopClientThread(); 
				gRender->bConnecting	= false;
				gRender->bGUI			= true;

				gLog->Log("[CGame] Disconnecting from server.\n");
			}
			else if (vkey == VK_F9 && !gNetwork->connected)
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


	if (ImGui_ImplWin32_WndProcHandler(wnd, umsg, wparam, lparam)) return 0;

	return CallWindowProc(orig_wndproc, wnd, umsg, wparam, lparam);
}