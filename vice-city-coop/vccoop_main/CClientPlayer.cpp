#include "main.h"

CClientPlayer::CClientPlayer(int nID, int gID)
{
	this->ped = NULL;
	CPlayerPed::SetupPlayerPed(gID);
	CWorld::Players[gID].m_pPed->m_nPedStatus = 2;
	this->ped = CWorld::Players[gID].m_pPed;
	this->ped->Teleport({ VCCOOP_DEFAULT_SPAWN_POSITION });
	this->ped->SetModelIndex(7);

	this->gameID = gID;
	this->networkID = nID;

	gLog->Log("[CClientPlayer]GameID: %d Network ID: %d Ped pointer: 0x%X\n\n", gID, nID, ped);

	gGame->remotePlayerPeds[gID] = this->ped;
	gGame->remotePlayers++;
}


void CClientPlayer::Respawn()
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

	CPlayerPed::SetupPlayerPed(this->gameID);
	CWorld::Players[this->gameID].m_pPed->m_nPedStatus = 2;
	this->ped = CWorld::Players[this->gameID].m_pPed;
	this->ped->Teleport({ VCCOOP_DEFAULT_SPAWN_POSITION });
	this->ped->SetModelIndex(7);
	this->ped->m_fHealth = 100.0f;


	gLog->Log("[CClientPlayer]GameID: %d Network ID: %d Ped pointer: 0x%X\n\n", this->gameID, this->networkID, ped);

	gGame->remotePlayerPeds[this->gameID] = this->ped;
}


CClientPlayer::CClientPlayer(int nID)
{
	this->gameID = 0;
	this->networkID = nID;
	this->ped = FindPlayerPed();
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

	this->gameID = -1;
	this->networkID = -1;
}

void CClientPlayer::SyncPlayer(PlayerSyncData spd)
{
	this->syncData = spd;

	ped->m_nModelIndex = spd.iModelIndex;
	//ped->m_dwAnimGroupId = spd.iCurrentAnimID;
	ped->m_fHealth = spd.Health;
	ped->m_fRotationCur = spd.Rotation;
	ped->m_fRotationDest = spd.Rotation;
	ped->m_fArmour = spd.Armour;

	if (spd.WepModelIndex > 0 && spd.CurrWep > 0)
	{
		if (CStreaming::ms_aInfoForModel[spd.WepModelIndex].m_nLoadState != LOADSTATE_LOADED)
		{
			CStreaming::RequestModel(spd.WepModelIndex, 22);
			CStreaming::LoadAllRequestedModels(false);
		}

		ped->GiveWeapon((eWeaponType)spd.CurrWep, 1000, true);
		ped->SetCurrentWeapon((eWeaponType)spd.CurrWep);
		ped->SetAmmo((eWeaponType)spd.CurrWep, 1000);
	}
	else
	{
		ped->SetCurrentWeapon(eWeaponType::WEAPONTYPE_UNARMED);
	}

	gGame->remotePlayerKeys[this->gameID] = spd.playerKeys;
	gGame->remotePlayerLookFrontX[this->gameID] = spd.playerLook;
}

PlayerSyncData CClientPlayer::BuildSyncData()
{
	PlayerSyncData spd;
	spd.Health = ped->m_fHealth;
	spd.iCurrentAnimID = ped->m_dwAnimGroupId;
	spd.Armour = ped->m_fArmour;
	spd.iModelIndex = ped->m_nModelIndex;
	spd.Rotation = ped->m_fRotationCur;

	spd.CurrWep = ped->m_aWeapons[ped->m_nWepSlot].m_nType;
	spd.WepModelIndex = ped->m_dwWepModelID;
	spd.Ammo = 0;


	spd.iInteriorID = 0;

	spd.playerKeys = *(GTA_CONTROLSET*)0x7DBCB0;
	spd.playerLook = *(CAMERA_AIM*)0x7E4978;
	
	this->syncData = spd;

	return spd;
}