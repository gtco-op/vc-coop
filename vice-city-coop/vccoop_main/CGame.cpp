#include "main.h"

DWORD dwCurPlayerActor = 0;
CPed * localPlayer = NULL;
static bool scriptProcessed = false;
WNDPROC		orig_wndproc;
HWND		orig_wnd;


void Hook_CRunningScript__Process();

/*
	copyPed->ClearAll();

	CVector pos = copyPed2->GetPosition();
	pos.x += 1.0f;
	copyPed->Teleport(pos);

	copyPed->m_fHealth = copyPed2->m_fHealth;
	copyPed->m_fRotationCur = copyPed2->m_fRotationCur;
	copyPed->m_fRotationDest = copyPed2->m_fRotationDest;

	copyPed->m_fLookDirection = copyPed2->m_fLookDirection;

	copyPed->m_vecAnimMoveDelta = copyPed2->m_vecAnimMoveDelta;
	copyPed->m_dwAnimGroupId = copyPed2->m_dwAnimGroupId;

	copyPed->m_vecMoveSpeed = copyPed2->m_vecMoveSpeed;

	//Action sync
	copyPed->m_dwAction = copyPed2->m_dwAction;
	copyPed->m_dwActionTimer = copyPed2->m_dwActionTimer;
	copyPed->m_fActionX = copyPed2->m_fActionX;
	copyPed->m_fActionY = copyPed2->m_fActionY;

	//Objective sync
	copyPed->m_dwObjective = copyPed2->m_dwObjective;
	copyPed->m_dwObjectiveTimer = copyPed2->m_dwObjectiveTimer;
	copyPed->m_vecObjective = copyPed2->m_vecObjective;
	copyPed->m_fObjectiveAngle = copyPed2->m_fObjectiveAngle;
	copyPed->m_pObjectiveEntity = copyPed2->m_pObjectiveEntity;
	copyPed->m_pObjectiveVehicle = copyPed2->m_pObjectiveVehicle;

	//Wander path sync?
	if (copyPed->m_dwAction == 4 || copyPed->m_dwAction == 5)
	{
	copyPed->SetWanderPath(*(BYTE *)(copyPed2 + 820));
	}

	//Flee sync
	copyPed->m_dwFleeTimer = copyPed2->m_dwFleeTimer;
	copyPed->m_fFleeFromPosX = copyPed2->m_fFleeFromPosX;
	copyPed->m_fFleeFromPosY = copyPed2->m_fFleeFromPosY;
	copyPed->m_pFleeFrom = copyPed2->m_pFleeFrom;

	//Path sync
	copyPed->m_fPathNextNodeDir = copyPed2->m_fPathNextNodeDir;
	copyPed->wRouteCurDir = copyPed2->wRouteCurDir;
	copyPed->m_vecPathNextNode = copyPed2->m_vecPathNextNode;
	copyPed->m_dwPathNodeTimer = copyPed2->m_dwPathNodeTimer;
	copyPed->m_wCurPathNode = copyPed2->m_wCurPathNode;
	copyPed->m_wPathNodes = copyPed2->m_wPathNodes;
	for (int i = 0; i < 8; i++)
	{
	copyPed->m_aPathNodeStates[i] = copyPed2->m_aPathNodeStates[i];
	copyPed->m_apPathNodesStates[i] = copyPed2->m_apPathNodesStates[i];
	}
	copyPed->m_dwPathNodeType = copyPed2->m_dwPathNodeType;
	copyPed->m_nPathState = copyPed2->m_nPathState;
	copyPed->m_pLastPathNode = copyPed2->m_pLastPathNode;
	copyPed->m_pNextPathNode = copyPed2->m_pNextPathNode;
	copyPed->m_pPathRelEntity = copyPed2->m_pPathRelEntity;


	copyPed->SetMoveState((eMoveState)copyPed2->m_dwMoveState);

	//Seek sync
	copyPed->m_fSeekExAngle = copyPed2->m_fSeekExAngle;
	copyPed->m_pSeekTarget = copyPed2->m_pSeekTarget;
	copyPed->m_vecSeekPosEx = copyPed2->m_vecSeekPosEx;
	copyPed->m_vecOffsetSeek = copyPed2->m_vecOffsetSeek;

	//Event sync
	copyPed->m_dwEventType = copyPed2->m_dwEventType;
	copyPed->m_fAngleToEvent = copyPed2->m_fAngleToEvent;
	copyPed->m_fEventOrThreatX = copyPed2->m_fEventOrThreatX;
	copyPed->m_fEventOrThreatY = copyPed2->m_fEventOrThreatY;

	copyPed->m_pEventEntity = copyPed2->m_pEventEntity;
*/

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

void  _declspec(naked) Patched_CPlayerPed__ProcessControl()
{
	_asm mov dwCurPlayerActor, ecx
	_asm pushad

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
		//Redirect to CPed::ProccessControl instead
		ThisCall(0x505790, (CPed*)dwCurPlayerActor);
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
void CGame::PatchAddToPopulation()
{
	// AddToPopulation()
	//MakeRet(0x53BA80);

	MakeNop(0x53E5C6, 5);

	MakeNop(0x53E99B, 5);
}
void CGame::UnpatchAddToPopulation()
{
	// AddToPopulation()
	//MemCpy((void*)0x53BA80, "\x53", 1);

	// First Call to ^
	MemCpy((void*)0x53E5C6, "\xE8\xB5\xD4\xFF\xFF", 5);

	// Second Call to ^
	MemCpy((void*)0x53E99B, "\xE8\xE0\xD0\xFF\xFF", 5);
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

	PatchAddToPopulation();

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
		
		//Pedpool inc
		MemWrite<s32>(0x4C02C8, 1000);
		//vehicle pool inc
		MemWrite<s32>(0x4C02EA, 1000);

		CPools::ms_pPedPool->Init(1000, NULL, NULL);
		CPools::ms_pVehiclePool->Init(1000, NULL, NULL);

		gLog->Log("[CGame] CRunningScript::Process() hook finished.\n");

		gRender->ToggleGUI();

		// First tick processed
		scriptProcessed = true;
	}
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
			if (vkey == 0x54) //T
			{
				if (!gChat->chatToggled)
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