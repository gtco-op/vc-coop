#include "main.h"

int CGame::keyPressTime;

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
		if (!gChat->chatToggled && !gRender->bGUI && !gRender->bConnecting && !gRender->bAboutWindow && !gRender->bEscMenu)
			gChat->ToggleChat(true); ImGui::GetIO().ClearInputCharacters(); 
	}
}
bool CGame::IsWindowActive()
{
	return (GetActiveWindow() == orig_wnd ? true : false);
}
void CGame::SetPlayerCameraPosition(float fX, float fY, float fZ, float fRotationX, float fRotationY, float fRotationZ)
{
	TheCamera.SetCamPositionForFixedMode({ fX, fY, fZ }, { fRotationX, fRotationY, fRotationZ });
	//Command<0x015F>(fX, fY, fZ, fRotationX, fRotationY, fRotationZ);
}
void CGame::CameraLookAtPoint(float fX, float fY, float fZ, int iType)
{
	TheCamera.TakeControlNoEntity({ fX, fY, fZ }, iType, 1);
	//Command<0x0160>(fX, fY, fZ, iType);
}
void CGame::SetCameraBehindPlayer()
{
	TheCamera.SetCameraDirectlyBehindForFollowPed_CamOnAString();
	//Command<0x0373>();
}
void CGame::RestoreCamera()
{
	TheCamera.Restore();
	//Command<0x015A>();
}
CVector CGame::GetCameraPos()
{
	return TheCamera.pos;
	//return CVector(MemRead<float>(0x7E46B8), MemRead<float>(0x7E46BC), MemRead<float>(0x7E46C0));
}
void CGame::DisableHUD()
{
	*(BYTE*)0xA10B45 = 0;
}
void CGame::EnableHUD()
{
	*(BYTE*)0xA10B45 = 1;
}
void CGame::SetCoordBlip(CVector coord, uint unk, eBlipDisplay blipDisplay)
{
	Call(0x4c3c80, eBlipType::BLIP_COORD, coord, unk, blipDisplay);
}
int FindIDForPed(CPed * ped)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (gGame->remotePlayerPeds[i] == ped)return i;
	}
	return -1;
}

BYTE internalPlayerID = 0;
CVehicle * _pVehicle;

void  _declspec(naked) Patched_CAutomobile_ProcessControl()
{
	_asm mov _pVehicle, ecx
	_asm pushad

	internalPlayerID = *(BYTE *)0xA10AFB;

	localPlayer = FindPlayerPed();

	if(_pVehicle->m_pDriver && _pVehicle->m_pDriver != localPlayer && internalPlayerID == 0)
	{
		// get the current driver's player number
		currentPlayerID = FindIDForPed(_pVehicle->m_pDriver);

		// key switching
		localPlayerKeys = *(GTA_CONTROLSET*)0x7DBCB0;

		// set remote player's keys
		*(GTA_CONTROLSET*)0x7DBCB0 = gGame->remotePlayerKeys[currentPlayerID];

		MemWrite<BYTE>(0xA10AFB, currentPlayerID);

		_asm popad
		_asm mov edi, 0x593030
		_asm call edi
		_asm pushad

		// restore the local player's keys and the internal ID.
		MemWrite<BYTE>(0xA10AFB, 0);

		*(GTA_CONTROLSET*)0x7DBCB0 = localPlayerKeys;
	}
	else
	{
		_asm popad
		_asm mov edi, 0x593030
		_asm call edi
		_asm pushad
	}

	_asm popad
	_asm ret
}

void  _declspec(naked) Patched_CPlayerPed__ProcessControl()
{
	_asm mov dwCurPlayerActor, ecx
	_asm pushad

	currentPlayerID = FindIDForPed((CPed*)dwCurPlayerActor);

	//gLog->Log("[CPlayerPed::ProcessControl()] Processing for %d\n", currentPlayerID);
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

void Hooked_DbgPrint(char * msg, ...)
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

void Hooked_LoadingScreen(char * message, char * message2, char * splash)
{
	Hooked_DbgPrint("Loading screen: %s %s %s\n",(message ? message : "0"), (message2 ? message2 : "0"), (splash ? splash : "0"));
	Call(0x4A69D0, message, message2, splash);
	return;
}

char(__thiscall* original_CPed__InflictDamage)(void*, void*, eWeaponType, float, ePedPieceTypes, UCHAR);
char __fastcall CPed__InflictDamage_Hook(void * This, DWORD _EDX, void* entity, eWeaponType weapon, float damage, ePedPieceTypes bodypart, UCHAR unk)
{
	//gLog->Log("damage inflicted %d %f", weapon, damage);
	return original_CPed__InflictDamage(This, entity, weapon, damage, bodypart, unk);
}


int(__thiscall* original_CPed__SetDead)(void*);
int __fastcall CPed__SetDead_Hook(void * This, DWORD _EDX)
{
	CPed * ped = (CPed*)This;
	if (ped == FindPlayerPed())
	{
		gLog->Log("player is ded");
		deathData dData;
		dData.killer = gNetwork->GetNetworkIDFromEntity(ped->m_pLastDamEntity);
		dData.weapon = ped->m_nLastDamWep;
		librg_message_send_all(&gNetwork->ctx, VCOOP_PED_IS_DEAD, &dData, sizeof(deathData));
	}
	return original_CPed__SetDead(This);
}

void Hooked_SpawnPedAfterDeath()
{
	gLog->Log("game tried to spawn me");
	CPed * ped = FindPlayerPed();
	ped->Teleport({VCCOOP_DEFAULT_SPAWN_POSITION});
	CTimer::Stop();
	librg_message_send_all(&gNetwork->ctx, VCOOP_RESPAWN_AFTER_DEATH, NULL, 0);
}

void CGame::InitPreGamePatches()
{
	original_CPed__InflictDamage = (char(__thiscall*)(void*, void*, eWeaponType, float, ePedPieceTypes, UCHAR))DetourFunction((PBYTE)0x525B20, (PBYTE)CPed__InflictDamage_Hook);
	original_CPed__SetDead = (int(__thiscall*)(void*))DetourFunction((PBYTE)0x4F6430, (PBYTE)CPed__SetDead_Hook);

	#ifdef VCCOOP_DEBUG_ENGINE
	patch::RedirectFunction(0x401000, Hooked_DbgPrint);//we overwrite the original func because thats not needed
	RedirectAllCalls(0x401000, 0x67DD05, 0x6F2434, Hooked_DbgPrint);//the original is needed
	RedirectAllCalls(0x401000, 0x67DD05, 0x4A69D0, Hooked_LoadingScreen);//the original is needed
	debugEnabled = true;
	#endif

	MakeCall(0x42BDA8, Hooked_SpawnPedAfterDeath);

	// Patch to allow multiple instances of the game
	SYSTEMTIME time; 
	GetSystemTime(&time); 
	char StreamName[15]; 
	sprintf_s(StreamName, "CdStream%02d%02d%02d", time.wHour, time.wMinute, time.wSecond); 
	auto Pointer = (DWORD *)0x408968; 
	
	DWORD Protect; 
	VirtualProtect(Pointer, 4, PAGE_READWRITE, &Protect); 
	*Pointer = (DWORD)StreamName; 
	VirtualProtect(Pointer, 4, Protect, &Protect);

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

	MemCpy(0x6D5E9C, splash_screens[r], 8);
	MemCpy(0x68E594, splash_screens[r], 8);
	MemCpy(0x68E6F4, splash_screens[r], 8);

	// Skip splash screen when entering another city
	//MakeRet(0x4A6E80);
	MakeNop(0x48DD7E, 5);
	MakeNop(0x40E157, 5);

	MakeNop(0x62A322, 36);//Dont pause the game when checking for collisions
	MakeNop(0x62A34E, 5);//Dont resume because its not paused

	// Hook script process (so we can spawn a local player)
	MakeCall(0x450245, Hook_CRunningScript__Process);

	//Set fps limit
	//MemWrite(0x602D68, 500); 

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
	//vehicle pool inc
	MemCpy((void*)0x4C02E4, "\x6A\x00\x68\x3E\x80\x00\x00", 7);//i hope its 1000 now

	//Nop ped spawns
	MakeNop(0x53E5C6, 5); //3peds
	MakeNop(0x53E99B, 5);

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

	//Disable white splash on big building loading
	MakeRet(0x4A68A0);

	//nop CVisibilityPlugins::SetClumpAlpha in CPed::ProcessControl
	MakeNop(0x5057EF, 5);

	//Ped visibility patch
	MakeNop(0x581A63, 2);
	
	//Disable test script loading
	//MakeNop(0x4A4F57, 8);
	//MakeRet(0x44FE60); crashes the game for some reason
	
	//Disable game pause (to prevent bugs in syncing)
	MakeRet(0x4D0DA0);

	//SetCUrrentWeapon fix
	MemWrite<BYTE>(0x4FF970, 0x9D);

	//Dont spawn weapons and money on dead ped (should handle by the server)
	MakeNop(0x4F653C, 27);
	
	//Setting a model shouldn't set any animations
	MakeNop(0x50D96A, 5);

	//Disable replay check in processcontrol
	MakeNop(0x537723, 2);

	//Fix vehicle driveby bugs
	MemCpy((void*)0x5C91F5, "\x85\xC9\x74\x09", 4);
	MemCpy((void*)0x5C9558, "\x85\xC9\x74\x09", 4);

	//Disable replays
	MakeNop(0x4A45C3, 5);

	//Disable SetEvasiveDive
	MakeRet(0x4F6A20, 8);

	//Dont print messages on pickups
	MakeNop(0x440B2C, 5);

	//Dont remove models in CPed::PlayIdleAnimations
	MakeNop(0x535F17, 5);

	// probably disable body part removing on weapon hit
	MemWrite<BYTE>(0x526282, 0x00);                   
	MemWrite<BYTE>(0x52629F, 0x00);
	MemWrite<BYTE>(0x5262BC, 0x00);
	MemWrite<BYTE>(0x5262D9, 0x00); 

	//EntryInfoNode limit patch
	MemWrite<BYTE>(0x4C02A6, 0x75);
	MemWrite<BYTE>(0x4C02A7, 0x30);

	//EntryInfoNode limit patch
	MemWrite<BYTE>(0x4C0284, 0xEA);
	MemWrite<BYTE>(0x4C0285, 0x60);

	//Object limit patch
	MemWrite<BYTE>(0x4C0284, 0x13);
	MemWrite<BYTE>(0x4C0285, 0x88);

	//Dummys limit patch
	MemWrite<BYTE>(0x4C036C, 0x4E);
	MemWrite<BYTE>(0x4C036D, 0x20);

	//Buildings limit patch
	MemWrite<BYTE>(0x4C0309, 0x4E);
	MemWrite<BYTE>(0x4C030A, 0x20);
	

	MemWrite<DWORD>(0x694D90, (DWORD)Patched_CPlayerPed__ProcessControl);
	MemWrite<DWORD>(0x69ADB0, (DWORD)Patched_CAutomobile_ProcessControl);

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
}
void CGame::DisableMouseInput()
{
	//Disable CPad:UpdateMouse
	MakeRet(0x4AD820);
	//CControllerConfigManager::AffectPadFromKeyBoard nop
	MakeNop(0x4AB6E6, 5);
	//CControllerConfigManager::AffectPadFromMouse nop
	MakeNop(0x4AB6F0, 5);
}
void Hook_CRunningScript__Process()
{
	if (!scriptProcessed)
	{
		CPools::ms_pPedPool->Clear();
		CPools::ms_pVehiclePool->Clear();

		CPools::ms_pPedPool->Init(1000, NULL, NULL);
		CPools::ms_pVehiclePool->Init(1000, NULL, NULL);

		// Change player model ID
		MemWrite<u8>(0x5384FA + 1, 7); //Not important if we set a new one after spawn

		// Setup own ped on 0 game ID
		CPlayerPed::SetupPlayerPed(0);
		gGame->remotePlayerPeds[0] = FindPlayerPed();

		// Set player position
		FindPlayerPed()->Teleport({ VCCOOP_DEFAULT_SPAWN_POSITION });
		
		// CStreaming::LoadScene
		CVector scenePosition(VCCOOP_DEFAULT_SPAWN_POSITION);
		Call(0x40AF60, &scenePosition);

		gGame->SetPlayerCameraPosition(531.629761f, 606.497253f, 10.901563f, 0, 0, 0);
		gGame->CameraLookAtPoint(531.629761f, 606.497253f, 10.901563f, 1);

		CWorld::Players[0].m_bNeverGetsTired = true;
		
		gLog->Log("[CGame] CRunningScript::Process() hook finished.\n");

		gRender->ToggleGUI();

		gGame->DisableHUD();

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
			if (vkey == 'P' && gNetwork->connected)
			{
				librg_message_send_all(&gNetwork->ctx, VCOOP_CREATE_PED, NULL, 0);
			}
			if (vkey == 'Z')
			{
				//CClientPlayer * player = new CClientPlayer(0, gGame->remotePlayers);
				//gGame->remotePlayerPeds[gGame->remotePlayers] = player->ped;
				//gGame->remotePlayers++;
				CVector pos = FindPlayerPed()->GetPosition();

				bool res = false;

				//FindPlayerPed()->Teleport(pos);

				for (int i = 0; i < 30; i++)
				{
					int xrand = Random(0, 200) - 100;
					int yrand = Random(0, 200) - 100;

					int path = ThePaths.FindNodeClosestToCoors({ pos.x + xrand, pos.y + yrand, pos.z }, 1, 10000.0f, 1, 0, 0, 0);
					CPathNode node = ThePaths.nodes[path];

					CVector spawnPos = { (float)node.m_wPosX*0.125f, (float)node.m_wPosY*0.125f, CWorld::FindGroundZFor3DCoord((float)node.m_wPosX*0.125f, (float)node.m_wPosY*0.125f, (float)node.m_wPosZ*0.125f, &res) + 1.0f };

					CPed * ped = new CCivilianPed(ePedType::PEDTYPE_CIVMALE, 7);
					CWorld::Add(ped);
					ped->Teleport(spawnPos);
					ped->SetWanderPath(Random(0, 9650));
				}

				//gLog->Log("node %d: %f %f %f\n", path, (long double)array1[path]*0.125f, (long double)array2[path] * 0.125f, (long double)array3[path] * 0.125f);
			}
			if (vkey == VK_ESCAPE)
			{
				if (gChat->chatToggled)
				{
					gChat->ToggleChat(false);
					return 0;
				}
				else
				{
					gRender->bEscMenu = !gRender->bEscMenu;
					return 0;
				}
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