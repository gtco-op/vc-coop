#include "main.h"

CClientPlayer::CClientPlayer(int nID, int gID)
{
	if (gID == -1)return;
	this->ped = NULL;
	CPlayerPed::SetupPlayerPed(gID);
	CWorld::Players[gID].m_pPed->m_nPedStatus = 1;
	this->ped = CWorld::Players[gID].m_pPed;

	this->ped->m_placement.pos = { VCCOOP_DEFAULT_SPAWN_POSITION };

	gGame->CustomModelLoad(7);
	if(CStreaming::ms_aInfoForModel[7].m_nLoadState == LOADSTATE_LOADED)	{
		this->ped->SetModelIndex(7);
		Sleep(1000);
	}
	
	CWorld::Players[gID].m_bNeverGetsTired = true;

	this->gameID = gID;
	this->networkID = nID;

	this->streamed = true;

	gLog->Log("[CClientPlayer]GameID: %d Network ID: %d Ped pointer: 0x%X\n\n", gID, nID, ped);

	gGame->remotePlayerPeds[gID] = this->ped;
	gGame->remotePlayers++;

	this->type = VCOOP_PLAYER;
}

void CClientPlayer::StreamIn()
{
	if (this->streamed)return;
	this->streamed = true;

	this->ped = NULL;
	CPlayerPed::SetupPlayerPed(this->gameID);
	CWorld::Players[this->gameID].m_pPed->m_nPedStatus = 2;
	this->ped = CWorld::Players[this->gameID].m_pPed;
	this->ped->m_placement.pos = { VCCOOP_DEFAULT_SPAWN_POSITION };

	if (this->syncData.iModelIndex >= 0) {
		if (CStreaming::ms_aInfoForModel[this->syncData.iModelIndex].m_nLoadState != LOADSTATE_LOADED) {
			gGame->CustomModelLoad(this->syncData.iModelIndex);
		}
		this->ped->SetModelIndex(this->syncData.iModelIndex);
	}
	else
	{
		if (CStreaming::ms_aInfoForModel[7].m_nLoadState != LOADSTATE_LOADED) {
			while (CStreaming::ms_aInfoForModel[7].m_nLoadState != LOADSTATE_LOADED)
			{
				Sleep(100);
				gGame->CustomModelLoad(7);
			}
		}
		this->ped->SetModelIndex(7);
	}

	gGame->remotePlayerPeds[this->gameID] = this->ped;
}

void CClientPlayer::StreamOut()
{
	if (this->streamed == false)return;
	this->streamed = false;

	if (this->ped)
	{
		CWorld::Remove(this->ped);
		if (this->ped)
		{
			this->ped->Remove();
			CPed::operator delete(this->ped);
			this->ped = NULL;
		}
	}
}

void CClientPlayer::Respawn()
{
	if (!this->streamed)return;
	if (this->ped)
	{
		CWorld::Remove(this->ped);
		if (this->ped)
		{
			this->ped->Remove();
			CPed::operator delete(this->ped);
			this->ped = NULL;
		}
	}

	CPlayerPed::SetupPlayerPed(this->gameID);
	CWorld::Players[this->gameID].m_pPed->m_nPedStatus = 2;
	this->ped = CWorld::Players[this->gameID].m_pPed;
	this->ped->m_placement.pos = { VCCOOP_DEFAULT_SPAWN_POSITION };

	if (this->syncData.iModelIndex >= 0) {
		if (CStreaming::ms_aInfoForModel[this->syncData.iModelIndex].m_nLoadState != LOADSTATE_LOADED) {
			gGame->CustomModelLoad(this->syncData.iModelIndex);

			while (CStreaming::ms_aInfoForModel[this->syncData.iModelIndex].m_nLoadState != LOADSTATE_LOADED)
			{
				Sleep(100);
			}
		}
		this->ped->SetModelIndex(this->syncData.iModelIndex);
	}
	else
	{
		if (CStreaming::ms_aInfoForModel[7].m_nLoadState != LOADSTATE_LOADED) {
			gGame->CustomModelLoad(7);

			while (CStreaming::ms_aInfoForModel[7].m_nLoadState != LOADSTATE_LOADED)			{
				Sleep(100);
			}
		}
		this->ped->SetModelIndex(7);
	}

	this->ped->m_fHealth = 100.0f;


	gLog->Log("[CClientPlayer]GameID: %d Network ID: %d Ped pointer: 0x%X\n\n", this->gameID, this->networkID, ped);

	gGame->remotePlayerPeds[this->gameID] = this->ped;
}


CClientPlayer::CClientPlayer(int nID)
{
	this->gameID = 0;
	this->networkID = nID;
	this->ped = LocalPlayer();
}

CClientPlayer::~CClientPlayer()
{
	if (this->ped)
	{
		CWorld::Remove(this->ped);
		if (this->ped)
		{
			this->ped->Remove();
			CPed::operator delete(this->ped);
			this->ped = NULL;
		}
	}

	gGame->remotePlayerPeds[this->gameID] = NULL;
	this->gameID = -1;
	this->networkID = -1;
}

void CClientPlayer::SyncPlayer(PlayerSyncData spd)
{
	this->syncData = spd;

	if (spd.isInVehicle) {
		if (gNetwork->GetEntityFromNetworkID(spd.vehicleID)) {
			if (reinterpret_cast<CVehicle*>(gNetwork->GetEntityFromNetworkID(spd.vehicleID)) != nullptr)
				ped->SetEnterCar(reinterpret_cast<CVehicle*>(gNetwork->GetEntityFromNetworkID(spd.vehicleID)), 0);
		}
	}
	if (spd.objective == OBJECTIVE_LEAVE_CAR) {
		if(reinterpret_cast<CVehicle*>(gNetwork->GetEntityFromNetworkID(spd.vehicleID)) != nullptr)
			ped->SetExitCar(reinterpret_cast<CVehicle*>(gNetwork->GetEntityFromNetworkID(spd.vehicleID)), 0);
	}

	ped->m_dwObjective = spd.objective;
	
	if (CModelIDs::IsValidPedModel(spd.iModelIndex) && ped->m_nModelIndex != spd.iModelIndex) {
		if (CStreaming::ms_aInfoForModel[spd.iModelIndex].m_nLoadState != LOADSTATE_LOADED) {
			gGame->CustomModelLoad(spd.iModelIndex);
		}
		
		while (CStreaming::ms_aInfoForModel[spd.iModelIndex].m_nLoadState != LOADSTATE_LOADED)
			Sleep(100);

		if(CStreaming::ms_aInfoForModel[spd.iModelIndex].m_nLoadState == LOADSTATE_LOADED)
			ped->SetModelIndex(spd.iModelIndex);
	}

	//ped->m_dwAnimGroupId = spd.iCurrentAnimID;
	ped->m_fHealth = spd.Health;
	ped->m_fRotationCur = spd.Rotation;
	ped->m_fRotationDest = spd.Rotation;
	ped->m_fArmour = spd.Armour;

	ped->m_placement.SetOrientation(spd.OrientX, spd.OrientY, spd.OrientZ);

	if (spd.WepModelIndex != ped->m_dwWepModelID && spd.CurrWep != ped->m_aWeapons[ped->m_nWepSlot].m_nType)
	{
		if (spd.CurrWep == 0)
		{
			ped->SetCurrentWeapon(eWeaponType::WEAPONTYPE_UNARMED);
		}
		else
		{
			if (CStreaming::ms_aInfoForModel[spd.WepModelIndex].m_nLoadState != LOADSTATE_LOADED) {
				gGame->CustomModelLoad(spd.WepModelIndex);
				while(CStreaming::ms_aInfoForModel[spd.WepModelIndex].m_nLoadState != LOADSTATE_LOADED) {
					Sleep(100);
				}
			}

			switch ((eWeaponType)spd.CurrWep)
			{
				case eWeaponType::WEAPONTYPE_CHAINSAW:
				case eWeaponType::WEAPONTYPE_BASEBALLBAT:
				case eWeaponType::WEAPONTYPE_BRASSKNUCKLE:
				case eWeaponType::WEAPONTYPE_CAMERA:
				case eWeaponType::WEAPONTYPE_CLEAVER:
				case eWeaponType::WEAPONTYPE_DETONATOR:
				case eWeaponType::WEAPONTYPE_GOLFCLUB:
				case eWeaponType::WEAPONTYPE_HAMMER:
				case eWeaponType::WEAPONTYPE_KATANA:
				case eWeaponType::WEAPONTYPE_KNIFE:
				case eWeaponType::WEAPONTYPE_MACHETE:
				case eWeaponType::WEAPONTYPE_NIGHTSTICK:
				case eWeaponType::WEAPONTYPE_SCREWDRIVER:
				{
					ped->GiveWeapon((eWeaponType)spd.CurrWep, 1, true);
					ped->SetAmmo((eWeaponType)spd.CurrWep, 1);
					break;
				}
				default:
				{
					ped->GiveWeapon((eWeaponType)spd.CurrWep, spd.Ammo, true);
					ped->SetAmmo((eWeaponType)spd.CurrWep, spd.Ammo);
					break;
				}
			}
			if (ped->m_dwWepModelID != spd.WepModelIndex) 
				ped->SetCurrentWeapon((eWeaponType)spd.CurrWep);
		}
	}

	//gGame->remotePlayerKeys[this->gameID] = spd.playerKeys;
	CPad * pad = &gGame->remotePlayerKeys[this->gameID];
	pad->NewState = spd.newPlayerKeys;
	pad->OldState = spd.oldPlayerKeys;

	//below: important
	pad->WORD_EA = 0;
	pad->Mode = 0;
	pad->ShakeDur = 0;
	pad->DisablePlayerControls = 0;

	gGame->remotePlayerKeys[this->gameID] = *pad;

	gGame->remotePlayerLookFrontX[this->gameID] = spd.playerLook;

	ped->m_nPedFlags.bIsStanding = spd.m_nPedFlags.bIsStanding;
	ped->m_nPedFlags.bWasStanding = spd.m_nPedFlags.bWasStanding;
	ped->m_nPedFlags.b03 = spd.m_nPedFlags.b03;
	ped->m_nPedFlags.bIsPointingGunAt = spd.m_nPedFlags.bIsPointingGunAt;
	ped->m_nPedFlags.bIsLooking = spd.m_nPedFlags.bIsLooking;
	ped->m_nPedFlags.b06 = spd.m_nPedFlags.b06;
	ped->m_nPedFlags.bIsRestoringLook = spd.m_nPedFlags.bIsRestoringLook;
	ped->m_nPedFlags.bIsAimingGun = spd.m_nPedFlags.bIsAimingGun;

	ped->m_nPedFlags.bIsRestoringGun = spd.m_nPedFlags.bIsRestoringGun;
	ped->m_nPedFlags.bCanPointGunAtTarget = spd.m_nPedFlags.bCanPointGunAtTarget;
	ped->m_nPedFlags.bIsTalking = spd.m_nPedFlags.bIsTalking;
	ped->m_nPedFlags.bIsInTheAir = spd.m_nPedFlags.bIsInTheAir;
	ped->m_nPedFlags.bIsLanding = spd.m_nPedFlags.bIsLanding;
	ped->m_nPedFlags.bIsRunning = spd.m_nPedFlags.bIsRunning;
	ped->m_nPedFlags.b15 = spd.m_nPedFlags.b15;
	ped->m_nPedFlags.b16 = spd.m_nPedFlags.b16;

	ped->m_nPedFlags.bCanPedEnterSeekedCar = spd.m_nPedFlags.bCanPedEnterSeekedCar;
	ped->m_nPedFlags.bRespondsToThreats = spd.m_nPedFlags.bRespondsToThreats;
	ped->m_nPedFlags.bRenderPedInCar = spd.m_nPedFlags.bRenderPedInCar;
	ped->m_nPedFlags.b20 = spd.m_nPedFlags.b20;
	ped->m_nPedFlags.bUpdateAnimHeading = spd.m_nPedFlags.bUpdateAnimHeading;
	ped->m_nPedFlags.bRemoveHead = spd.m_nPedFlags.bRemoveHead;
	ped->m_nPedFlags.bFiringWeapon = spd.m_nPedFlags.bFiringWeapon;
	ped->m_nPedFlags.b24 = spd.m_nPedFlags.b24;

	ped->m_nPedFlags.b25a = spd.m_nPedFlags.b25a;
	ped->m_nPedFlags.b25b = spd.m_nPedFlags.b25b;
	ped->m_nPedFlags.bPedIsBleeding = spd.m_nPedFlags.bPedIsBleeding;
	ped->m_nPedFlags.bStopAndShoot = spd.m_nPedFlags.bStopAndShoot;
	ped->m_nPedFlags.bIsPedDieAnimPlaying = spd.m_nPedFlags.bIsPedDieAnimPlaying;
	ped->m_nPedFlags.b30 = spd.m_nPedFlags.b30;
	ped->m_nPedFlags.b31 = spd.m_nPedFlags.b31;
	ped->m_nPedFlags.b32 = spd.m_nPedFlags.b32;

	ped->m_nPedFlags.bKindaStayInSamePlace = spd.m_nPedFlags.bKindaStayInSamePlace;
	ped->m_nPedFlags.bBeingChasedByPolice = spd.m_nPedFlags.bBeingChasedByPolice;
	ped->m_nPedFlags.bNotAllowedToDuck = spd.m_nPedFlags.bNotAllowedToDuck;
	ped->m_nPedFlags.bCrouchWhenShooting = spd.m_nPedFlags.bCrouchWhenShooting;
	ped->m_nPedFlags.bIsDucking = spd.m_nPedFlags.bIsDucking;
	ped->m_nPedFlags.bGetUpAnimStarted = spd.m_nPedFlags.bGetUpAnimStarted;
	ped->m_nPedFlags.bDoBloodyFootprints = spd.m_nPedFlags.bDoBloodyFootprints;
	ped->m_nPedFlags.b40 = spd.m_nPedFlags.b40;

	ped->m_nPedFlags.b41 = spd.m_nPedFlags.b41;
	ped->m_nPedFlags.b42 = spd.m_nPedFlags.b42;
	ped->m_nPedFlags.b43 = spd.m_nPedFlags.b43;
	ped->m_nPedFlags.b44 = spd.m_nPedFlags.b44;
	ped->m_nPedFlags.b45 = spd.m_nPedFlags.b45;
	ped->m_nPedFlags.b46 = spd.m_nPedFlags.b46;
	ped->m_nPedFlags.b47 = spd.m_nPedFlags.b47;
	ped->m_nPedFlags.b48 = spd.m_nPedFlags.b48;

	ped->m_nPedFlags.b49 = spd.m_nPedFlags.b49;
	ped->m_nPedFlags.b50 = spd.m_nPedFlags.b50;
	ped->m_nPedFlags.b51 = spd.m_nPedFlags.b51;
	ped->m_nPedFlags.b52 = spd.m_nPedFlags.b52;
	ped->m_nPedFlags.b53 = spd.m_nPedFlags.b53;
	ped->m_nPedFlags.b54 = spd.m_nPedFlags.b54;
	ped->m_nPedFlags.bIsBeingJacked = spd.m_nPedFlags.bIsBeingJacked;
	ped->m_nPedFlags.bFadeOut = spd.m_nPedFlags.bFadeOut;

	ped->m_nPedFlags.b57 = spd.m_nPedFlags.b57;
	ped->m_nPedFlags.b58 = spd.m_nPedFlags.b58;
	ped->m_nPedFlags.b59 = spd.m_nPedFlags.b59;
	ped->m_nPedFlags.bClearObjective = spd.m_nPedFlags.bClearObjective;
	ped->m_nPedFlags.b61 = spd.m_nPedFlags.b61;
	ped->m_nPedFlags.b62 = spd.m_nPedFlags.b62;
	ped->m_nPedFlags.b63 = spd.m_nPedFlags.b63;
	ped->m_nPedFlags.b64 = spd.m_nPedFlags.b64;

	ped->m_nPedFlags.bShakeFist = spd.m_nPedFlags.bShakeFist;
	ped->m_nPedFlags.bNoCriticalHits = spd.m_nPedFlags.bNoCriticalHits;
	ped->m_nPedFlags.b67 = spd.m_nPedFlags.b67;
	ped->m_nPedFlags.bHasAlreadyBeenRecorded = spd.m_nPedFlags.bHasAlreadyBeenRecorded;
	ped->m_nPedFlags.bFallenDown = spd.m_nPedFlags.bFallenDown;
	ped->m_nPedFlags.bUpdateMatricesRequired = spd.m_nPedFlags.bUpdateMatricesRequired;
	ped->m_nPedFlags.b71 = spd.m_nPedFlags.b71;
	ped->m_nPedFlags.b72 = spd.m_nPedFlags.b72;

	ped->m_nPedFlags.b73 = spd.m_nPedFlags.b73;
	ped->m_nPedFlags.b74 = spd.m_nPedFlags.b74;
	ped->m_nPedFlags.b75 = spd.m_nPedFlags.b75;
	ped->m_nPedFlags.b76 = spd.m_nPedFlags.b76;
	ped->m_nPedFlags.b77 = spd.m_nPedFlags.b77;
	ped->m_nPedFlags.bMiamiViceCop = spd.m_nPedFlags.bMiamiViceCop;
	ped->m_nPedFlags.bMoneyHasBeenGivenByScript = spd.m_nPedFlags.bMoneyHasBeenGivenByScript;
	ped->m_nPedFlags.bHasBeenPhotographed = spd.m_nPedFlags.bHasBeenPhotographed;

	ped->m_nPedFlags.bIsDrowning = spd.m_nPedFlags.bIsDrowning;
	ped->m_nPedFlags.bDrownsInWater = spd.m_nPedFlags.bDrownsInWater;
	ped->m_nPedFlags.bStayBehindIfLeaderTooFar = spd.m_nPedFlags.bStayBehindIfLeaderTooFar;
	ped->m_nPedFlags.bUsingExitCarTimer = spd.m_nPedFlags.bUsingExitCarTimer;
	ped->m_nPedFlags.bScriptPedIsPlayerAlly = spd.m_nPedFlags.bScriptPedIsPlayerAlly;
	ped->m_nPedFlags.bKnockedUpIntoAir = spd.m_nPedFlags.bKnockedUpIntoAir;
	ped->m_nPedFlags.bDeadPedInFrontOfCar = spd.m_nPedFlags.bDeadPedInFrontOfCar;
	ped->m_nPedFlags.bStayInCarOnJack = spd.m_nPedFlags.bStayInCarOnJack;

	ped->m_nPedFlags.b89 = spd.m_nPedFlags.b89;
	ped->m_nPedFlags.bDoomAim = spd.m_nPedFlags.bDoomAim;
	ped->m_nPedFlags.bCanBeShotInVehicle = spd.m_nPedFlags.bCanBeShotInVehicle;
	ped->m_nPedFlags.b92 = spd.m_nPedFlags.b92;
	ped->m_nPedFlags.b93 = spd.m_nPedFlags.b93;
	ped->m_nPedFlags.b94 = spd.m_nPedFlags.b94;
	ped->m_nPedFlags.b95 = spd.m_nPedFlags.b95;
	ped->m_nPedFlags.b96 = spd.m_nPedFlags.b96;

	ped->m_nPedFlags.b97 = spd.m_nPedFlags.b97;
	ped->m_nPedFlags.bCrouchWhenScared = spd.m_nPedFlags.bCrouchWhenScared;
	ped->m_nPedFlags.b99 = spd.m_nPedFlags.b99;
	ped->m_nPedFlags.b100 = spd.m_nPedFlags.b100;
	ped->m_nPedFlags.b101 = spd.m_nPedFlags.b101;
	ped->m_nPedFlags.b102 = spd.m_nPedFlags.b102;
	ped->m_nPedFlags.bPedWasSetOutOfCollision = spd.m_nPedFlags.bPedWasSetOutOfCollision;
	ped->m_nPedFlags.bGangMemberReturnsFire = spd.m_nPedFlags.bGangMemberReturnsFire;
}

PlayerSyncData CClientPlayer::BuildSyncData()
{
	PlayerSyncData spd;

	spd.Health = ped->m_fHealth;
	spd.iCurrentAnimID = ped->m_dwAnimGroupId;
	spd.Armour = ped->m_fArmour;
	spd.iModelIndex = ped->m_nModelIndex;
	spd.Rotation = ped->m_fRotationCur;

	spd.objective = ped->m_dwObjective;

	ped->m_placement.GetOrientation(spd.OrientX, spd.OrientY, spd.OrientZ);

	spd.m_nPedFlags.bIsStanding = ped->m_nPedFlags.bIsStanding;
	spd.m_nPedFlags.bWasStanding = ped->m_nPedFlags.bWasStanding;
	spd.m_nPedFlags.b03 = ped->m_nPedFlags.b03;
	spd.m_nPedFlags.bIsPointingGunAt = ped->m_nPedFlags.bIsPointingGunAt;
	spd.m_nPedFlags.bIsLooking = ped->m_nPedFlags.bIsLooking;
	spd.m_nPedFlags.b06 = ped->m_nPedFlags.b06;
	spd.m_nPedFlags.bIsRestoringLook = ped->m_nPedFlags.bIsRestoringLook;
	spd.m_nPedFlags.bIsAimingGun = ped->m_nPedFlags.bIsAimingGun;

	spd.m_nPedFlags.bIsRestoringGun = ped->m_nPedFlags.bIsRestoringGun;
	spd.m_nPedFlags.bCanPointGunAtTarget = ped->m_nPedFlags.bCanPointGunAtTarget;
	spd.m_nPedFlags.bIsTalking = ped->m_nPedFlags.bIsTalking;
	spd.m_nPedFlags.bIsInTheAir = ped->m_nPedFlags.bIsInTheAir;
	spd.m_nPedFlags.bIsLanding = ped->m_nPedFlags.bIsLanding;
	spd.m_nPedFlags.bIsRunning = ped->m_nPedFlags.bIsRunning;
	spd.m_nPedFlags.b15 = ped->m_nPedFlags.b15;
	spd.m_nPedFlags.b16 = ped->m_nPedFlags.b16;

	spd.m_nPedFlags.bCanPedEnterSeekedCar = ped->m_nPedFlags.bCanPedEnterSeekedCar;
	spd.m_nPedFlags.bRespondsToThreats = ped->m_nPedFlags.bRespondsToThreats;
	spd.m_nPedFlags.bRenderPedInCar = ped->m_nPedFlags.bRenderPedInCar;
	spd.m_nPedFlags.b20 = ped->m_nPedFlags.b20;
	spd.m_nPedFlags.bUpdateAnimHeading = ped->m_nPedFlags.bUpdateAnimHeading;
	spd.m_nPedFlags.bRemoveHead = ped->m_nPedFlags.bRemoveHead;
	spd.m_nPedFlags.bFiringWeapon = ped->m_nPedFlags.bFiringWeapon;
	spd.m_nPedFlags.b24 = ped->m_nPedFlags.b24;

	spd.m_nPedFlags.b25a = ped->m_nPedFlags.b25a;
	spd.m_nPedFlags.b25b = ped->m_nPedFlags.b25b;
	spd.m_nPedFlags.bPedIsBleeding = ped->m_nPedFlags.bPedIsBleeding;
	spd.m_nPedFlags.bStopAndShoot = ped->m_nPedFlags.bStopAndShoot;
	spd.m_nPedFlags.bIsPedDieAnimPlaying = ped->m_nPedFlags.bIsPedDieAnimPlaying;
	spd.m_nPedFlags.b30 = ped->m_nPedFlags.b30;
	spd.m_nPedFlags.b31 = ped->m_nPedFlags.b31;
	spd.m_nPedFlags.b32 = ped->m_nPedFlags.b32;

	spd.m_nPedFlags.bKindaStayInSamePlace = ped->m_nPedFlags.bKindaStayInSamePlace;
	spd.m_nPedFlags.bBeingChasedByPolice = ped->m_nPedFlags.bBeingChasedByPolice;
	spd.m_nPedFlags.bNotAllowedToDuck = ped->m_nPedFlags.bNotAllowedToDuck;
	spd.m_nPedFlags.bCrouchWhenShooting = ped->m_nPedFlags.bCrouchWhenShooting;
	spd.m_nPedFlags.bIsDucking = ped->m_nPedFlags.bIsDucking;
	spd.m_nPedFlags.bGetUpAnimStarted = ped->m_nPedFlags.bGetUpAnimStarted;
	spd.m_nPedFlags.bDoBloodyFootprints = ped->m_nPedFlags.bDoBloodyFootprints;
	spd.m_nPedFlags.b40 = ped->m_nPedFlags.b40;

	spd.m_nPedFlags.b41 = ped->m_nPedFlags.b41;
	spd.m_nPedFlags.b42 = ped->m_nPedFlags.b42;
	spd.m_nPedFlags.b43 = ped->m_nPedFlags.b43;
	spd.m_nPedFlags.b44 = ped->m_nPedFlags.b44;
	spd.m_nPedFlags.b45 = ped->m_nPedFlags.b45;
	spd.m_nPedFlags.b46 = ped->m_nPedFlags.b46;
	spd.m_nPedFlags.b47 = ped->m_nPedFlags.b47;
	spd.m_nPedFlags.b48 = ped->m_nPedFlags.b48;

	spd.m_nPedFlags.b49 = ped->m_nPedFlags.b49;
	spd.m_nPedFlags.b50 = ped->m_nPedFlags.b50;
	spd.m_nPedFlags.b51 = ped->m_nPedFlags.b51;
	spd.m_nPedFlags.b52 = ped->m_nPedFlags.b52;
	spd.m_nPedFlags.b53 = ped->m_nPedFlags.b53;
	spd.m_nPedFlags.b54 = ped->m_nPedFlags.b54;
	spd.m_nPedFlags.bIsBeingJacked = ped->m_nPedFlags.bIsBeingJacked;
	spd.m_nPedFlags.bFadeOut = ped->m_nPedFlags.bFadeOut;

	spd.m_nPedFlags.b57 = ped->m_nPedFlags.b57;
	spd.m_nPedFlags.b58 = ped->m_nPedFlags.b58;
	spd.m_nPedFlags.b59 = ped->m_nPedFlags.b59;
	spd.m_nPedFlags.bClearObjective = ped->m_nPedFlags.bClearObjective;
	spd.m_nPedFlags.b61 = ped->m_nPedFlags.b61;
	spd.m_nPedFlags.b62 = ped->m_nPedFlags.b62;
	spd.m_nPedFlags.b63 = ped->m_nPedFlags.b63;
	spd.m_nPedFlags.b64 = ped->m_nPedFlags.b64;

	spd.m_nPedFlags.bShakeFist = ped->m_nPedFlags.bShakeFist;
	spd.m_nPedFlags.bNoCriticalHits = ped->m_nPedFlags.bNoCriticalHits;
	spd.m_nPedFlags.b67 = ped->m_nPedFlags.b67;
	spd.m_nPedFlags.bHasAlreadyBeenRecorded = ped->m_nPedFlags.bHasAlreadyBeenRecorded;
	spd.m_nPedFlags.bFallenDown = ped->m_nPedFlags.bFallenDown;
	spd.m_nPedFlags.bUpdateMatricesRequired = ped->m_nPedFlags.bUpdateMatricesRequired;
	spd.m_nPedFlags.b71 = ped->m_nPedFlags.b71;
	spd.m_nPedFlags.b72 = ped->m_nPedFlags.b72;

	spd.m_nPedFlags.b73 = ped->m_nPedFlags.b73;
	spd.m_nPedFlags.b74 = ped->m_nPedFlags.b74;
	spd.m_nPedFlags.b75 = ped->m_nPedFlags.b75;
	spd.m_nPedFlags.b76 = ped->m_nPedFlags.b76;
	spd.m_nPedFlags.b77 = ped->m_nPedFlags.b77;
	spd.m_nPedFlags.bMiamiViceCop = ped->m_nPedFlags.bMiamiViceCop;
	spd.m_nPedFlags.bMoneyHasBeenGivenByScript = ped->m_nPedFlags.bMoneyHasBeenGivenByScript;
	spd.m_nPedFlags.bHasBeenPhotographed = ped->m_nPedFlags.bHasBeenPhotographed;

	spd.m_nPedFlags.bIsDrowning = ped->m_nPedFlags.bIsDrowning;
	spd.m_nPedFlags.bDrownsInWater = ped->m_nPedFlags.bDrownsInWater;
	spd.m_nPedFlags.bStayBehindIfLeaderTooFar = ped->m_nPedFlags.bStayBehindIfLeaderTooFar;
	spd.m_nPedFlags.bUsingExitCarTimer = ped->m_nPedFlags.bUsingExitCarTimer;
	spd.m_nPedFlags.bScriptPedIsPlayerAlly = ped->m_nPedFlags.bScriptPedIsPlayerAlly;
	spd.m_nPedFlags.bKnockedUpIntoAir = ped->m_nPedFlags.bKnockedUpIntoAir;
	spd.m_nPedFlags.bDeadPedInFrontOfCar = ped->m_nPedFlags.bDeadPedInFrontOfCar;
	spd.m_nPedFlags.bStayInCarOnJack = ped->m_nPedFlags.bStayInCarOnJack;

	spd.m_nPedFlags.b89 = ped->m_nPedFlags.b89;
	spd.m_nPedFlags.bDoomAim = ped->m_nPedFlags.bDoomAim;
	spd.m_nPedFlags.bCanBeShotInVehicle = ped->m_nPedFlags.bCanBeShotInVehicle;
	spd.m_nPedFlags.b92 = ped->m_nPedFlags.b92;
	spd.m_nPedFlags.b93 = ped->m_nPedFlags.b93;
	spd.m_nPedFlags.b94 = ped->m_nPedFlags.b94;
	spd.m_nPedFlags.b95 = ped->m_nPedFlags.b95;
	spd.m_nPedFlags.b96 = ped->m_nPedFlags.b96;

	spd.m_nPedFlags.b97 = ped->m_nPedFlags.b97;
	spd.m_nPedFlags.bCrouchWhenScared = ped->m_nPedFlags.bCrouchWhenScared;
	spd.m_nPedFlags.b99 = ped->m_nPedFlags.b99;
	spd.m_nPedFlags.b100 = ped->m_nPedFlags.b100;
	spd.m_nPedFlags.b101 = ped->m_nPedFlags.b101;
	spd.m_nPedFlags.b102 = ped->m_nPedFlags.b102;
	spd.m_nPedFlags.bPedWasSetOutOfCollision = ped->m_nPedFlags.bPedWasSetOutOfCollision;
	spd.m_nPedFlags.bGangMemberReturnsFire = ped->m_nPedFlags.bGangMemberReturnsFire;

	spd.CurrWep = ped->m_aWeapons[ped->m_nWepSlot].m_nType;
	spd.WepModelIndex = ped->m_dwWepModelID;
	spd.Ammo = ped->m_aWeapons[ped->m_nWepSlot].m_nTotalAmmo;

	spd.isInVehicle = ped->m_bInVehicle;
	spd.vehicleID = gNetwork->GetNetworkIDFromEntity(ped->m_pVehicle);

	spd.iInteriorID = 0;
	
	CPad * pad = CPad::GetPad(0);
	if (CWorld::PlayerInFocus != 0) pad = &localPlayerKeys;

	spd.oldPlayerKeys	= pad->OldState;
	spd.newPlayerKeys	= pad->NewState;

	if (this->ped->m_aWeapons[this->ped->m_nWepSlot].m_nAmmoInClip < 1 && this->ped->m_nWepSlot > 0)
	{
		//dont send fire key if there is no ammo in the clip
		spd.newPlayerKeys.ButtonCircle = 0;
		spd.oldPlayerKeys.ButtonCircle = 0;
	}

	spd.playerLook = *(CAMERA_AIM*)&TheCamera.Cams[TheCamera.ActiveCam].Front;
	
	this->syncData = spd;

	return spd;
}

CEntity * CClientPlayer::GetEntity()
{
	return this->ped;
}