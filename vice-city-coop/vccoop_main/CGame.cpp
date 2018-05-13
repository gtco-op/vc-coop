#include "main.h"

bool			CGame::bLoadingDone = false;

int				CGame::keyPressTime;

WNDPROC			orig_wndproc;
HWND			orig_wnd;

bool bIsLoadingModel = false;

CGame::CGame()
{
	for (int i = 1; i < MAX_PLAYERS; i++)
	{
		this->remotePlayerPeds[i] = NULL;
	}
	this->InitPreGamePatches();
	keyPressTime = 0;

	gLog->Log("[CGame] CGame initialized.\n");
}

void CGame::HasGameLoaded()
{
	if (bLoadingDone == true)
		return;


	bLoadingDone = true;
	gLog->Log("[CGame] Game finished loading.\n");
}
CPed * CGame::FindLocalPed()
{
	return CWorld::Players[0].m_pPed;
}
static RpClump* LoadModel(const char *filename) {
	RpClump *result = nullptr;
	auto stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, filename);
	if (stream) {
		if (RwStreamFindChunk(stream, rwID_CLUMP, NULL, NULL)) {
			result = RpClumpStreamRead(stream);
			gLog->Log("Loaded model '%s'!\n", filename);
		}
		else
			gLog->Log("Unable to read model file: '%s'\n", filename);
		RwStreamClose(stream, nullptr);
	}
	else
		gLog->Log("Unable to open model file: '%s'\n", filename);
	return result;
}

static void UnloadModel(RpClump *model) {
	if (model)
		RpClumpDestroy(model);
}

CObject* CGame::SpawnModel(int modelid, CVector position, RpClump* clump = nullptr)
{
	CObject* object				= new CObject(modelid, false);
	
	object->m_nType				= eObjectType::OBJECT_MISSION;
	object->m_placement.pos		= position + CVector(2.0f, 2.0f, 2.0f);
	object->m_fAttachForce		= 0.0f;

	object->m_placement.UpdateRW();
	object->UpdateRwFrame();
	
	CWorld::Add(object);

	return object;
}

void CGame::OnConnected()
{
#ifdef VCCOOP_VERBOSE_LOG
	gLog->Log("[CGame] OnConnected called!\n");
#endif

	gChat->AddChatMessage("[" VCCOOP_NAME "] " "Connection successful.");
}
void CGame::StartMissionScript()
{
	LoadMissionScript();
}

void CGame::Run()
{
	if (GetKey(VK_TAB))
	{
		gGame->bShowScoreboard = true;
	}
	else
	{
		gGame->bShowScoreboard = false;
	}

	if (GetTwoKeys(0x12, 'O'))
	{
		keyPressTime = CTimer::m_snTimeInMilliseconds;
	}
	else if (GetKey(0x2C) & 1)
	{
		keyPressTime = CTimer::m_snTimeInMilliseconds;
		gRender->TakeScreenshot();
	}
#ifdef VCCOOP_DEBUG
	else if (GetKey(223))
	{
		keyPressTime = CTimer::m_snTimeInMilliseconds;
		gRender->bConsole = !gRender->bConsole;
		ImGui::GetIO().ClearInputCharacters();
	}
#endif
	else if (GetKey('T') && !gChat->chatToggled && IsWindowActive())
	{
		keyPressTime = CTimer::m_snTimeInMilliseconds;
		if (!gChat->chatToggled && !gRender->bGUI && !gRender->bConnecting && !gRender->bAboutWindow && !gRender->bEscMenu)
			gChat->ToggleChat(true); ImGui::GetIO().ClearInputCharacters(); 
	}
}
void CGame::Exit()
{
	delete gNetwork;
	delete gRender;

	gLog->Log("[CGame] CGame shutting down.\n");
	delete gLog;
	exit(0);
}
/*
int CheckModel(int iModelID)
{
	if (iModelID < 0) {
		DWORD * dwUnknown = (DWORD *)0x7D1DE0;
		return dwUnknown[-7 * iModelID];
	}
	return iModelID;
}

//-----------------------------------------------------------

void CGame::RequestModel(int iModelID)
{
	DWORD dwModelID = CheckModel(iModelID);
	DWORD dwFlags = 0x16;
	DWORD dwFunc = FUNC_RequestModel;
	_asm
	{
		push dwFlags
		push dwModelID
		call dwFunc
		add esp, 8
	}
}

//-----------------------------------------------------------

void CGame::LoadRequestedModels()
{
	DWORD dwFunc = FUNC_LoadRequestedModels;
	_asm
	{
		push 0
		call dwFunc
		add esp, 4
	}
}

//------------------------------------------------------------

BOOL CGame::IsModelLoaded(int iModelID)
{
	DWORD dwModelID = CheckModel(iModelID);
	BYTE * byteModelInfo = (BYTE *)VAR_ModelInfo;
	if (byteModelInfo[20 * dwModelID] == 1) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}
*/
bool IsModelLoaded(int modelid)
{
	if (CStreaming::ms_aInfoForModel[modelid].m_nLoadState != LOADSTATE_LOADED)
	{
		return false; 
	}
	else if (CStreaming::ms_aInfoForModel[modelid].m_nLoadState == LOADSTATE_LOADED)
	{
		return true;
	}
	else return false;
}

void CGame::CustomModelLoad(int id)
{
	if (id == NULL)
		return;

	int modelid = id;
	while (!IsModelLoaded(modelid)) {
		if (!bIsLoadingModel) {
			CStreaming::RequestModel(modelid, 22);
			bIsLoadingModel = true;
		}
		Sleep(1);
	}
	
	bIsLoadingModel = false;
}
/*
void CGame::CustomModelLoad(int id)
{
	if (CStreaming::ms_aInfoForModel[id].m_nLoadState == LOADSTATE_NOT_LOADED)
	{
		CStreaming::RequestModel(id, eStreamingFlags::MISSION_REQUEST);
		CStreaming::LoadAllRequestedModels(false);
		if (CStreaming::ms_aInfoForModel[id].m_nLoadState == LOADSTATE_LOADED && !CStreaming::ms_aInfoForModel[id].m_nFlags & 1)
		{
			CStreaming::SetModelIsDeletable(id);
			Call(0x40ADF0, id); //SetModelTxdIsDeletable
		}
	}
}
*/

bool CGame::IsWindowActive()
{
	return (GetActiveWindow() == orig_wnd ? true : false);
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
	Call(0x4C3C80, eBlipType::BLIP_COORD, coord, unk, blipDisplay);
}

CPlayerPed* CWorld_Players;

void RedirectAllPointers(int startaddr, int endaddr, MemoryPointer with, MemoryPointer dest)
{
	for (int i = startaddr; i < endaddr; i++)
	{
		MemoryPointer at = ReadRelativeOffset(i);
		if (at == with)
		{
			patch::SetPointer(i, dest);
		}
	}
}

void CGame::InitPreGamePatches()
{
	DWORD flOldProtect;
	VirtualProtect((LPVOID)0x401000, 0x27CE00u, PAGE_EXECUTE_READWRITE, &flOldProtect);

	CWorld_Players = (CPlayerPed*)malloc(0x170 * MAX_PLAYERS);
	
	RedirectAllPointers(0x401000, 0x67DD05, 0xA10AFB, CWorld_Players);

#ifdef VCCOOP_DEBUG
	// Patch to allow multiple instances of the game
	SYSTEMTIME time; 
	GetSystemTime(&time); 
	char StreamName[60]; 
	sprintf_s(StreamName, "CdStream%02d%02d%02d", time.wHour, time.wMinute, time.wMilliseconds); 
	auto Pointer = (DWORD *)0x408968; 
	
	DWORD Protect; 
	VirtualProtect(Pointer, 4, PAGE_READWRITE, &Protect); 
	*Pointer = (DWORD)StreamName; 
	VirtualProtect(Pointer, 4, Protect, &Protect);
#endif

	//disable gamestate initialize
	MakeNop(0x601B3B, 10);

	//Set game state to loaded 
	MemWrite<u32>(0x9B5F08, 5);
	  
	//Set bDrawMenu to 0
	MemWrite<u8>(0x869642, 0);

	//Set bMenuVisible = 0
	MemWrite<u8>(0x869668, 0);

	//Set bGameStarted = 1
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
	MemCpy((void*)0x4C02E4, "\x6A\x00\x68\x3E\x80\x00\x00", 7);//its 1000 i tested it

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

	//Dont pause the game on big building loading
	MakeNop(0x40DFE4, 5);
	MakeNop(0x40E242, 5);

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

	// flhreference patch
	MemWrite<BYTE>(0x628E00, 5);

	//Fix crash on colliding PED with vehicle
	MemWrite<BYTE>(0x50A4BE, 0x69);
	MemWrite<BYTE>(0x50A4BE, 0x1E); 
	MemWrite<BYTE>(0x50A4BE, 0xE8);

	//disable CPop::RemovePedIfPoolGetsFUll
	MakeRet(0x53D560);

	//PlayOneShotScriptObject patching something in statement
	MemWrite<BYTE>(0x5F9233, 0x4C);
	MemWrite<BYTE>(0x5C45A6, 0xEE);
	MemWrite<BYTE>(0x5C4688, 0x36);

	// ProcessEntitiesInSectorList currArea patch
	MemWrite<BYTE>(0x40E7FC, 0xEBu);
	MemWrite<BYTE>(0x40E8CC, 0xEBu);
	MemWrite<BYTE>(0x40E686, 0xEBu);
	MemWrite<BYTE>(0x40E6C3, 0xEBu);
	MemWrite<BYTE>(0x40E703, 0xEBu);
	MemWrite<BYTE>(0x40E743, 0xEBu);

	// CTxdStore::AddRef patch
	MemWrite<BYTE>(0x580A7F, 0xF);
	// CTxdStore::RemoveRef patch
	MemWrite<BYTE>(0x580A2F, 0x22);

	// disable CPlayerPed::KeepAreaAroundPlayerClear
	MakeRet(0x531D60);

	//Disable choking
	MakeNop(0x4D4E0F, 6);

	//nop smth in RenderPedCB
	MakeNop(0x581A63, 2);

	//CPed::Refresh patch
	MakeNop(0x50D96A, 5);

	//disable random melee thing
	MakeNop(0x5D2E88, 6);

	//disable CPlayerInfo::MakePlayerSafe | causes crash
	//MakeRet(0x4BBC10, 8);

	// Disable ValidateVersion
	// Contains a stupid check for 'grandtheftauto3' string in peds.col
	MakeRet(0x4A5320);

	//cAudioManager::GetPedCommentSfx()
	MemWrite<BYTE>(0x5EA1FC, 0x75);

	//Stop time passing on death
	MakeNop(0x42BD69, 15); 

	// Stop the loading of ambient traffic models and textures
	// by skipping CStreaming::StreamVehiclesAndPeds() and CStreaming::StreamZoneModels()
	MemWrite<BYTE>(0x40EF27, 0xEB);

	//Set streaming memory to 128MB its 64 by default
	MemWrite<int>(0x410799+6,134217728);

	//Disable CPlayerInfo::Process
	//MakeRet(0x4BCA90);

	// Disable CRubbish::Init() (crashfix)
	MakeRet(0x568550);

	// Disable Cheat_strncmp
	MakeRet(0x4ACF60);

	//Init hooks (no shit sherlock)
	CHooks::InitHooks();

	// Fix shooting M4 etc around peds
	MakeRet(0x50BE00);

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
void WaitForModelLoad(int modelIndex)
{
	bool loadedModel = false;
	while (loadedModel == false && IsModelLoaded(modelIndex) == false) {
		if (!bIsLoadingModel)
			gGame->CustomModelLoad(modelIndex);
		else
			Sleep(10);
	}
}
CVehicle * CGame::CreateVehicle(int modelIndex, CVector position)
{
	CVehicle *vehicle = nullptr;

	if (modelIndex <= 0)
		return nullptr;

	WaitForModelLoad(modelIndex);

	gLog->Log("[CGame] Model ID: %d\tVehicle type: %d\n", modelIndex, reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[modelIndex])->m_nVehicleType);

	switch (reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[modelIndex])->m_nVehicleType)
	{
		case VEHICLE_HELI:
			vehicle = new CHeli(modelIndex, 2);
			break;
		case VEHICLE_PLANE:
			vehicle = new CPlane(modelIndex, 2);
			break;
		case VEHICLE_BIKE:
			vehicle = new CBike(modelIndex, 2);
			break;
		case VEHICLE_BOAT:
			vehicle = new CBoat(modelIndex, 2);
			break;
		default://0
			vehicle = new CAutomobile(modelIndex, 2);
			break;
	}
	if (vehicle)
	{
		vehicle->m_nLockStatus		= 1;
		vehicle->m_nState			= (unsigned char)0x4;
		vehicle->m_placement.pos	= position;
		
		// invalid position set.. fuck that.. reset it!
		if (vehicle->m_placement.pos.x <= -10000.f || vehicle->m_placement.pos.y <= -10000.f || vehicle->m_placement.pos.z <= -10000.f)		{
			vehicle->m_placement.pos = { VCCOOP_DEFAULT_SPAWN_POSITION };
		}

		CWorld::Add(vehicle);

		return vehicle;
	}
	gLog->Log("[CGame] CreateVehicle: model was not loaded!\n");
	return nullptr;
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

			if (gChat->chatToggled && vkey == VK_RETURN)
				return ImGui_ImplWin32_WndProcHandler(wnd, umsg, wparam, lparam);

			if (!gRender->bEscMenu && (ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard))
				break;

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
			if (vkey == VK_F6)
			{
				gRender->bServerView = !gRender->bServerView;
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
				LocalPlayer()->m_placement.pos = { VCCOOP_DEFAULT_SPAWN_POSITION };
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