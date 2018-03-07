#include "main.h"

GTA_CONTROLSET	localPlayerKeys;
CAMERA_AIM		localPlayerLookFrontX;
BYTE			localPlayerCameraMode;
BYTE			internalPlayerID			= 0;
CVehicle *		_pVehicle;
static bool		scriptProcessed				= false;

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

/*
void  _declspec(naked) Patched_CAutomobile_ProcessControl()
{
	_asm mov _pVehicle, ecx
	_asm pushad

	internalPlayerID = *(BYTE *)0xA10AFB;

	localPlayer = LocalPlayer();

	if (_pVehicle->m_pDriver && _pVehicle->m_pDriver != localPlayer && internalPlayerID == 0)
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
*/
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

char(__thiscall* original_CPed__InflictDamage)(CPed*, CEntity*, eWeaponType, float, ePedPieceTypes, UCHAR);
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


int(__thiscall* original_CPed__SetDead)(CPed*);
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

signed int(__cdecl* original_ShowExceptionBox)(DWORD*, int, int);
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
	ped->Teleport({ VCCOOP_DEFAULT_SPAWN_POSITION });
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
		LocalPlayer()->Teleport({ VCCOOP_DEFAULT_SPAWN_POSITION });

		// CStreaming::LoadScene
		CVector scenePosition(VCCOOP_DEFAULT_SPAWN_POSITION);
		Call(0x40AF60, &scenePosition);

		CCameraStuff::SetPlayerCameraPosition(531.629761f, 606.497253f, 10.901563f, 0, 0, 0);
		CCameraStuff::CameraLookAtPoint(531.629761f, 606.497253f, 10.901563f, 1);

		CWorld::Players[0].m_bNeverGetsTired = true;

		gLog->Log("[CGame] CRunningScript::Process() hook finished.\n");

		gRender->ToggleGUI();

		gGame->DisableHUD();

		// First tick processed
		scriptProcessed = true;
/*
		eStreamingFlags loadingType = eStreamingFlags::GAME_REQUEST;

		CStreaming::RequestModel(269, loadingType);
		CStreaming::RequestModel(270, loadingType);
		CStreaming::RequestModel(275, loadingType);
		CStreaming::RequestModel(278, loadingType);
		CStreaming::RequestModel(284, loadingType);
		CStreaming::RequestModel(280, loadingType);
		CStreaming::RequestModel(286, loadingType);
		CStreaming::RequestModel(290, loadingType);
		CStreaming::RequestModel(294, loadingType);
		CStreaming::RequestModel(268, loadingType);
		CStreaming::RequestModel(270, loadingType);
		CStreaming::RequestModel(291, loadingType);
		CStreaming::RequestModel(275, loadingType);
		CStreaming::RequestModel(279, loadingType);
		CStreaming::RequestModel(283, loadingType);
		CStreaming::RequestModel(280, loadingType);
		CStreaming::RequestModel(286, loadingType);
		CStreaming::RequestModel(287, loadingType);
		CStreaming::RequestModel(259, loadingType);
		CStreaming::RequestModel(264, loadingType);
		CStreaming::RequestModel(272, loadingType);
		CStreaming::RequestModel(274, loadingType);
		CStreaming::RequestModel(277, loadingType);
		CStreaming::RequestModel(281, loadingType);
		CStreaming::RequestModel(276, loadingType);
		CStreaming::RequestModel(285, loadingType);
		CStreaming::RequestModel(288, loadingType);

		for (int i = 130; i < 236; i++)
		{
			CStreaming::RequestModel(i, loadingType);
		}

		CStreaming::LoadAllRequestedModels(0);*/
	}
}

char(__cdecl* original_RemoveModel)(int);
char __cdecl RemoveModel_Hook(int model)
{
	if (model < 300 && model >= 268)
	{
		gLog->Log("[CHooks]Game removed weapon model: %d\n", model);
		return 0;
	}
	return original_RemoveModel(model);
}

char(__thiscall* original_CAutomobile__ProcessControl)(CVehicle*);
char __fastcall CAutomobile__ProcessControl_Hook(CVehicle * This, DWORD _EDX)
{
	//gLog->Log("[CAutomobile::ProcessControl()] Processing for 0x%X 0x%X\n", This, This->m_pDriver);
	if (This->m_pDriver && This->m_pDriver != LocalPlayer())
	{
		int currentPlayerID = FindIDForPed((CPed*)This->m_pDriver);
		if (currentPlayerID == -1)return 0;

		//gLog->Log("[CAutomobile::ProcessControl()] Processing for %d\n", currentPlayerID);
		CWorld::PlayerInFocus = currentPlayerID;

		// set remote player's keys
		*CPad::GetPad(0) = gGame->remotePlayerKeys[currentPlayerID];

		// call the internal CPlayerPed[]::Process
		original_CAutomobile__ProcessControl(This);

		// restore the local player's keys and the internal ID.
		CWorld::PlayerInFocus = 0;
		return 0;
	}
	return original_CAutomobile__ProcessControl(This);
}

char(__thiscall* original_CPlayerPed__ProcessControl)(CPlayerPed*);
char __fastcall CPlayerPed__ProcessControl_Hook(CPlayerPed * This, DWORD _EDX)
{
	if (This != LocalPlayer())
	{
		int currentPlayerID = FindIDForPed((CPed*)This);
		if (currentPlayerID == -1)return 0;

		//gLog->Log("[CPlayerPed::ProcessControl()] Processing for %d\n", currentPlayerID);

		CWorld::PlayerInFocus = currentPlayerID;

		// set remote player's keys
		*CPad::GetPad(0) = gGame->remotePlayerKeys[currentPlayerID];

		// save the internal cammode.
		localPlayerCameraMode = (BYTE)TheCamera.Cams[TheCamera.ActiveCam].Mode;

		// onfoot mouse looking mode.
		TheCamera.Cams[TheCamera.ActiveCam].Mode = 4;

		// aim switching
		localPlayerLookFrontX = *(CAMERA_AIM*)&TheCamera.Cams[TheCamera.ActiveCam].Front;
		*(CAMERA_AIM*)&TheCamera.Cams[TheCamera.ActiveCam].Front = gGame->remotePlayerLookFrontX[currentPlayerID];

		// call the internal CPlayerPed[]::Process
		original_CPlayerPed__ProcessControl(This);

		// restore the camera mode.
		TheCamera.Cams[TheCamera.ActiveCam].Mode = localPlayerCameraMode;

		// restore the local player's keys and the internal ID.
		CWorld::PlayerInFocus = 0;

		*(CAMERA_AIM*)&TheCamera.Cams[TheCamera.ActiveCam].Front = localPlayerLookFrontX;
		return 0;
	}
	return original_CPlayerPed__ProcessControl(This);
}

CPad*(__cdecl* original_GetPad)(int);
CPad* __cdecl GetPad_Hook(int pad)
{
	return original_GetPad(CWorld::PlayerInFocus);
}

CPad * CHooks::GetPad(int padnumber)
{
	return original_GetPad(padnumber);
}

int(__thiscall* original_CPed__SetIdle)(CPed*);
int __fastcall CPed__SetIdle_Hook(CPed * This, DWORD _EDX)//probably unnecessary
{
	gLog->Log("SetIdle called");
	if (This->IsPlayer())
	{
		gLog->Log("prevented SetIdle on player ped");
		return 0;
	}
	return original_CPed__SetIdle(This);
}

int(__thiscall* original_CWeapon__DoBulletImpact)(CWeapon*This, CEntity*, CEntity*, CVector*, CVector*, CColPoint*, CVector2D);
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

void CHooks::InitHooks()
{
	original_CPed__InflictDamage = (char(__thiscall*)(CPed*, CEntity*, eWeaponType, float, ePedPieceTypes, UCHAR))DetourFunction((PBYTE)0x525B20, (PBYTE)CPed__InflictDamage_Hook);
	original_CPed__SetDead = (int(__thiscall*)(CPed*))DetourFunction((PBYTE)0x4F6430, (PBYTE)CPed__SetDead_Hook);
	original_ShowExceptionBox = (signed int(__cdecl*)(DWORD*, int, int))DetourFunction((PBYTE)0x677E40, (PBYTE)ShowExceptionBox_Hook);
	//original_RemoveModel = (char(__cdecl*)(int))DetourFunction((PBYTE)0x40D6E0, (PBYTE)RemoveModel_Hook);
	original_CPlayerPed__ProcessControl = (char(__thiscall*)(CPlayerPed*))DetourFunction((PBYTE)0x537270, (PBYTE)CPlayerPed__ProcessControl_Hook);
	original_CAutomobile__ProcessControl = (char(__thiscall*)(CVehicle*))DetourFunction((PBYTE)0x593030, (PBYTE)CAutomobile__ProcessControl_Hook);
	original_GetPad = (CPad*(__cdecl*)(int))DetourFunction((PBYTE)0x4AB060, (PBYTE)GetPad_Hook);
	//original_CPed__SetIdle = (int(__thiscall*)(CPed*))DetourFunction((PBYTE)0x4FDFD0, (PBYTE)CPed__SetIdle_Hook);
	original_CWeapon__DoBulletImpact = (int(__thiscall*)(CWeapon*This, CEntity*, CEntity*, CVector*, CVector*, CColPoint*, CVector2D))DetourFunction((PBYTE)0x5CEE60, (PBYTE)CWeapon__DoBulletImpact_Hook);

#ifdef VCCOOP_DEBUG_ENGINE
	patch::ReplaceFunction(0x401000, Hooked_DbgPrint);//we overwrite the original func because thats not needed
	RedirectAllCalls(0x401000, 0x67DD05, 0x6F2434, Hooked_DbgPrint);//the original is needed
	RedirectAllCalls(0x401000, 0x67DD05, 0x4A69D0, Hooked_LoadingScreen);//the original is needed
	debugEnabled = true;
#endif

	MakeCall(0x42BE05, Hooked_SpawnPedAfterDeath);

	// Hook script process (so we can spawn a local player)
	MakeCall(0x450245, Hook_CRunningScript__Process);

	//MemWrite<DWORD>(0x694D90, (DWORD)Patched_CPlayerPed__ProcessControl);
	//MemWrite<DWORD>(0x69ADB0, (DWORD)Patched_CAutomobile_ProcessControl);
}