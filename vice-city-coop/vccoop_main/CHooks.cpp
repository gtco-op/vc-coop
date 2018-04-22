#include "main.h"

CPad			localPlayerKeys;
CAMERA_AIM		localPlayerLookFrontX;
BYTE			localPlayerCameraMode;
BYTE			internalPlayerID			= 0;
CVehicle *		_pVehicle;
static bool		scriptProcessed				= false;

char(__thiscall* original_CPed__InflictDamage)(CPed*, CEntity*, eWeaponType, float, ePedPieceTypes, UCHAR);
int(__thiscall* original_CPed__SetDead)(CPed*);
signed int(__cdecl* original_ShowExceptionBox)(DWORD*, int, int);
char(__thiscall* original_CAutomobile__ProcessControl)(CVehicle*);
char(__thiscall* original_CPlayerPed__ProcessControl)(CPlayerPed*);
int(__thiscall* original_CWeapon__DoBulletImpact)(CWeapon*This, CEntity*, CEntity*, CVector*, CVector*, CColPoint*, CVector2D);
CPed*(__cdecl* original_FindPlayerPed)(void);
BOOL(__thiscall* original_CPed_IsPlayer)(CPed* This);
int(__thiscall* original_SetWantedLevel)(CPlayerPed* This, int level);

void LoadMissionScript()
{
	// Load the SCM Script (restore)..
	MemCpy(0x4506D6, "\xE8\x55\xD9\x03\x00", 5);

	// Restore the CRunningScript::Process() hook..
	MemCpy((void*)0x450245, "\xE8\x26\xFB\xFF\xFF", 5);

	// Call CTheScripts::Init(void)
	Call(0x450330);
	// Call CTheScripts::Init(int)
	Call(0x450270, 0);
}

template<class A, class B = A>
void CHooks::InitPool(CPool<A, B> *pool, int nSize)
{
	pool->m_pObjects = static_cast<B*>(operator new(sizeof(B) * nSize));
	pool->m_byteMap = static_cast<tPoolObjectFlags*>(operator new(nSize));

	pool->m_nSize = nSize;
	pool->m_nFirstFree = -1;

	for (int i = 0; i < nSize; ++i) 
	{
		pool->m_byteMap[i].bEmpty = true;
		pool->m_byteMap[i].nId = 0;
	}
}

int FindIDForPed(CPed * ped)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (gGame->remotePlayerPeds[i] == ped)return i;
	}
	return -1;
}

int CHooks::FindFreeIDForPed()
{
	for (int i = 1; i < MAX_PLAYERS; i++)
	{
		if (gGame->remotePlayerPeds[i] == NULL)return i;
	}
	return -1;
}

void Hooked_DbgPrint(char * msg, ...)
{
	char buffer[256];
	bool newline = false;

	va_list args;

	va_start(args, msg);
	vsprintf(buffer, msg, args);
	va_end(args);

	if (!strstr(buffer, "\n")) {
		newline = true;
	}
	
#ifdef VCCOOP_DEBUG
	if (gRender->gDebugScreen->gDevConsole != nullptr && debugEnabled)
	{
		gRender->gDebugScreen->gDevConsole->AddLog("%s%s", buffer, (newline ? "\n" : ""));
	}
#endif
#ifdef VCCOOP_DEBUG_ENGINE
	gDbgLog->Log(" %s%s", buffer, (newline ? "\n" : ""));
	gDbgEngine->AddDebugMessage("[DEBUG] %s", buffer);
#endif
	return;
}

void Hooked_LoadingScreen(char * message, char * message2, char * splash)
{
	Hooked_DbgPrint("Loading screen: %s %s %s\n", (message ? message : "0"), (message2 ? message2 : "0"), (splash ? splash : "0"));
	Call(0x4A69D0, message, message2, splash);
	return;
}

char __fastcall CPed__InflictDamage_Hook(CPed * This, DWORD _EDX, CEntity* entity, eWeaponType weapon, float damage, ePedPieceTypes bodypart, UCHAR unk)
{
	if (entity == LocalPlayer())
	{
		gLog->Log("You did %f damage on someone with %d\n", damage, weapon);
		return 0;
	}
	if (This == entity)//yes its possible
	{
		return 0;
	}
	return original_CPed__InflictDamage(This, entity, weapon, damage, bodypart, unk);
}


int __fastcall CPed__SetDead_Hook(CPed * This, DWORD _EDX)
{
	if (This == LocalPlayer())
	{
		gLog->Log("player is ded 0x%X\n", This);
		deathData dData;
		dData.killer = gNetwork->GetNetworkIDFromEntity(This->m_pLastDamEntity);
		dData.weapon = This->m_nLastDamWep;
		librg_message_send_all(&gNetwork->ctx, VCOOP_PED_IS_DEAD, &dData, sizeof(deathData));
	}
	return original_CPed__SetDead(This);
}

signed int __cdecl ShowExceptionBox_Hook(DWORD* a1, int a2, int a3)
{
	gLog->Log("Exception %08X occurred at address %08X\n", *a1, *(DWORD *)(a3 + 184));
	return original_ShowExceptionBox(a1, a2, a3);
}

void Hooked_SpawnPedAfterDeath()
{
	CTimer::Update();
	gLog->Log("game tried to spawn me\n");
	CPed * ped = LocalPlayer();
	ped->m_placement.pos = { VCCOOP_DEFAULT_SPAWN_POSITION };
	librg_message_send_all(&gNetwork->ctx, VCOOP_RESPAWN_AFTER_DEATH, NULL, 0);
}

void Hook_CRunningScript__Process()
{
	if (!scriptProcessed)
	{
		CPools::ms_pPedPool->Clear();
		CPools::ms_pVehiclePool->Clear();

		CHooks::InitPool(CPools::ms_pPedPool, MAX_PEDS);
		CHooks::InitPool(CPools::ms_pVehiclePool, MAX_VEHICLES);

		// Change player model ID
		MemWrite<u8>(0x5384FA + 1, 7); //Not important if we set a new one after spawn

		// Setup own ped on 0 game ID
		CPlayerPed::SetupPlayerPed(0);
		gGame->remotePlayerPeds[0] = LocalPlayer();

		// Set player position
		LocalPlayer()->m_placement.pos = { VCCOOP_DEFAULT_SPAWN_POSITION };

		// CStreaming::LoadScene
		CVector scenePosition(VCCOOP_DEFAULT_SPAWN_POSITION);
		Call(0x40AF60, &scenePosition);

		CCameraStuff::SetPlayerCameraPosition(627.544556f, 599.654846f, 12.071713f, 0, 0, 0);
		CCameraStuff::CameraLookAtPoint(VCCOOP_DEFAULT_CAMERA_LOOKPOSITION, 1);

		CWorld::Players[0].m_bNeverGetsTired = true;

		gLog->Log("[CGame] CRunningScript::Process() hook finished.\n");

		gRender->ToggleGUI();

		gGame->DisableHUD();

		CStreaming::RequestModel(269, 1);
		CStreaming::RequestModel(270, 1);
		CStreaming::RequestModel(275, 1);
		CStreaming::RequestModel(278, 1);
		CStreaming::RequestModel(284, 1);
		CStreaming::RequestModel(280, 1);
		CStreaming::RequestModel(286, 1);
		CStreaming::RequestModel(290, 1);
		CStreaming::RequestModel(294, 1);
		CStreaming::RequestModel(268, 1);
		CStreaming::RequestModel(270, 1);
		CStreaming::RequestModel(291, 1);
		CStreaming::RequestModel(275, 1);
		CStreaming::RequestModel(279, 1);
		CStreaming::RequestModel(283, 1);
		CStreaming::RequestModel(280, 1);
		CStreaming::RequestModel(286, 1);
		CStreaming::RequestModel(287, 1);
		CStreaming::RequestModel(259, 1);
		CStreaming::RequestModel(264, 1);
		CStreaming::RequestModel(272, 1);
		CStreaming::RequestModel(274, 1);
		CStreaming::RequestModel(277, 1);
		CStreaming::RequestModel(281, 1);
		CStreaming::RequestModel(276, 1);
		CStreaming::RequestModel(285, 1);
		CStreaming::RequestModel(288, 1);
		CStreaming::LoadAllRequestedModels(0);

		// First tick processed
		scriptProcessed = true;
	}
}

char __fastcall CAutomobile__ProcessControl_Hook(CVehicle * This, DWORD _EDX)
{
	//gLog->Log("[CAutomobile::ProcessControl()] Processing for 0x%X 0x%X\n", This, This->m_pDriver);
	if (This->m_pDriver && This->m_pDriver != LocalPlayer())
	{
		int currentPlayerID = FindIDForPed((CPed*)This->m_pDriver);
		if (currentPlayerID == -1)return 0;

		// set remote player to focus
		CWorld::PlayerInFocus = currentPlayerID;

		// save local player's keys
		localPlayerKeys = *CPad::GetPad(0);

		// set remote player's keys
		*CPad::GetPad(0) = gGame->remotePlayerKeys[currentPlayerID];

		// call the internal CPlayerPed[]::Process
		original_CAutomobile__ProcessControl(This);

		// restore local player's keys
		*CPad::GetPad(0) = localPlayerKeys;

		// restore the local player's id
		CWorld::PlayerInFocus = 0;
		return 0;
	}
	return original_CAutomobile__ProcessControl(This);
}

char __fastcall CPlayerPed__ProcessControl_Hook(CPlayerPed * This, DWORD _EDX)
{
	if (This != LocalPlayer())
	{
		int currentPlayerID = FindIDForPed((CPed*)This);
		if (currentPlayerID == -1)return 0;

		// set player to focus
		CWorld::PlayerInFocus = currentPlayerID;

		// save local player's keys
		localPlayerKeys = *CPad::GetPad(0);

		// set remote player's keys
		*CPad::GetPad(0) = gGame->remotePlayerKeys[currentPlayerID];

		// save the internal cammode.
		localPlayerCameraMode = (BYTE)TheCamera.Cams[TheCamera.ActiveCam].Mode;

		// onfoot mouse looking mode.
		TheCamera.Cams[TheCamera.ActiveCam].Mode = 4;

		// save local player's aim
		localPlayerLookFrontX = *(CAMERA_AIM*)&TheCamera.Cams[TheCamera.ActiveCam].Front;

		// set remote player's aim
		*(CAMERA_AIM*)&TheCamera.Cams[TheCamera.ActiveCam].Front = gGame->remotePlayerLookFrontX[currentPlayerID];

		// call the internal CPlayerPed[]::Process
		original_CPlayerPed__ProcessControl(This);

		// restore the camera mode.
		TheCamera.Cams[TheCamera.ActiveCam].Mode = localPlayerCameraMode;

		// restore local player keys
		*CPad::GetPad(0) = localPlayerKeys;

		// restore the local player's id
		CWorld::PlayerInFocus = 0;

		//restore local player's aim
		*(CAMERA_AIM*)&TheCamera.Cams[TheCamera.ActiveCam].Front = localPlayerLookFrontX;
		return 0;
	}
	return original_CPlayerPed__ProcessControl(This);
}

int __fastcall CWeapon__DoBulletImpact_Hook(CWeapon*This, DWORD _EDX, CEntity* source, CEntity* target, CVector* start, CVector* end, CColPoint* colpoint, CVector2D ahead)//probably unnecessary
{
	if (source != LocalPlayer()) // we dont need original bullets from remote players because we will sync it with librg messages
	{
		return 0;
	}
	else
	{
		bulletSyncData bsData;
		bsData.player = gNetwork->GetNetworkIDFromEntity(source);
		bsData.targetEntityID = gNetwork->GetNetworkIDFromEntity(target);
		bsData.start = *start;
		bsData.end = *end;
		bsData.colPoint = *colpoint;
		bsData.ahead = ahead;

		librg_message_send_all(&gNetwork->ctx, VCOOP_BULLET_SYNC, &bsData, sizeof(bulletSyncData));
	}
	return original_CWeapon__DoBulletImpact(This, source, target, start, end, colpoint, ahead);
}

void CHooks::DoBulletImpact(CWeapon*This, CEntity* source, CEntity* target, CVector* start, CVector* end, CColPoint* colpoint, CVector2D ahead)
{
	original_CWeapon__DoBulletImpact(This, source, target, start, end, colpoint, ahead);
}

CPed* FindPlayerPed_Hook(void)
{
	return CWorld::Players[0].m_pPed;
}

BOOL __fastcall CPed_IsPlayer_Hook(CPed* This)
{
	return original_CPed_IsPlayer(This);
}

int __fastcall SetWantedLevel_Hook(CPlayerPed* This, int level)
{
	gLog->Log("Setting wanted level to %d\n", level);
	return original_SetWantedLevel(This, level);
}

void CHooks::InitHooks()
{
	original_CPed__InflictDamage			= (char(__thiscall*)(CPed*, CEntity*, eWeaponType, float, ePedPieceTypes, UCHAR))DetourFunction((PBYTE)0x525B20, (PBYTE)CPed__InflictDamage_Hook);
	original_CPed__SetDead					= (int(__thiscall*)(CPed*))DetourFunction((PBYTE)0x4F6430, (PBYTE)CPed__SetDead_Hook);
	original_ShowExceptionBox				= (signed int(__cdecl*)(DWORD*, int, int))DetourFunction((PBYTE)0x677E40, (PBYTE)ShowExceptionBox_Hook);
	original_CPlayerPed__ProcessControl		= (char(__thiscall*)(CPlayerPed*))DetourFunction((PBYTE)0x537270, (PBYTE)CPlayerPed__ProcessControl_Hook);
	original_CAutomobile__ProcessControl	= (char(__thiscall*)(CVehicle*))DetourFunction((PBYTE)0x593030, (PBYTE)CAutomobile__ProcessControl_Hook);
	original_CWeapon__DoBulletImpact		= (int(__thiscall*)(CWeapon*This, CEntity*, CEntity*, CVector*, CVector*, CColPoint*, CVector2D))DetourFunction((PBYTE)0x5CEE60, (PBYTE)CWeapon__DoBulletImpact_Hook);

	original_FindPlayerPed					= (CPed*(__cdecl*)(void))DetourFunction((PBYTE)0x4BC120, (PBYTE)FindPlayerPed_Hook);
	
	original_CPed_IsPlayer					= (BOOL(__thiscall*)(CPed* This))DetourFunction((PBYTE)0x4F4930, (PBYTE)CPed_IsPlayer_Hook);

	original_SetWantedLevel					= (int(__thiscall*)(CPlayerPed* This, int level))DetourFunction((PBYTE)0x532090, (PBYTE)SetWantedLevel_Hook);

#ifdef VCCOOP_DEBUG_ENGINE
	patch::ReplaceFunction(0x401000, Hooked_DbgPrint);//we overwrite the original func because thats not needed
	RedirectAllCalls(0x401000, 0x67DD05, 0x6F2434, Hooked_DbgPrint);//the original is needed
	RedirectAllCalls(0x401000, 0x67DD05, 0x4A69D0, Hooked_LoadingScreen);//the original is needed
	patch::ReplaceFunction(0x648AC0, Hooked_DbgPrint);
	debugEnabled = true;
#endif

	MakeCall(0x42BE05, Hooked_SpawnPedAfterDeath);

	// Hook script process (so we can spawn a local player)
	MakeCall(0x450245, Hook_CRunningScript__Process);

	//MemWrite<DWORD>(0x694D90, (DWORD)Patched_CPlayerPed__ProcessControl);
	//MemWrite<DWORD>(0x69ADB0, (DWORD)Patched_CAutomobile_ProcessControl);
}