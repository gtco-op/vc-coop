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
	
	gLog->Log("GameID: %d\nNetwork ID: %d\nPed pointer: 0x%X\n\n", gID, nID, ped);

	gGame->remotePlayerPeds[gID] = this->ped;
	gGame->remotePlayers++;
}

CClientPlayer::CClientPlayer(int nID)
{
	this->gameID = 0;
	this->networkID = nID;
	this->ped = FindPlayerPed();
}

CClientPlayer::~CClientPlayer()
{

}

void CClientPlayer::SyncPlayer(PlayerSyncData spd)
{
	this->syncData = spd;

	ped->m_nModelIndex = spd.iModelIndex;
	ped->m_dwAnimGroupId = spd.iCurrentAnimID;
	ped->m_fHealth = spd.Health;
	ped->m_fRotationCur = spd.Rotation;
	ped->m_fRotationDest = spd.Rotation;
	ped->m_fArmour = spd.Armour;

	//this->ConvertKeys(spd.playerKeys);
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

	spd.CurrWep = 0;
	spd.WepModelIndex = 0;
	spd.Ammo = 0;

	spd.iInteriorID = 0;

	spd.playerKeys = *(GTA_CONTROLSET*)0x7DBCB0;
	spd.playerLook = *(CAMERA_AIM*)0x7E4978;
	
	this->syncData = spd;

	return spd;
}

void CClientPlayer::ConvertKeys(WORD wKeys)
{
	GTA_CONTROLSET * pPlayerKeys = &gGame->remotePlayerKeys[this->gameID];

	if (wKeys & 1) 
	{
		pPlayerKeys->wKeys2[KEY_INCAR_HANDBRAKE] = 0xFF;
		pPlayerKeys->wKeys1[KEY_INCAR_HANDBRAKE] = 0xFF;
	}
	else 
	{
		pPlayerKeys->wKeys2[KEY_INCAR_HANDBRAKE] = 0x00;
		pPlayerKeys->wKeys1[KEY_INCAR_HANDBRAKE] = 0x00;
	}
	wKeys >>= 1; // 1

	if (wKeys & 1) {
		pPlayerKeys->wKeys2[KEY_INCAR_LOOKR] = 0xFF;
		pPlayerKeys->wKeys1[KEY_INCAR_LOOKR] = 0xFF;
	}
	else {
		pPlayerKeys->wKeys2[KEY_INCAR_LOOKR] = 0x00;
		pPlayerKeys->wKeys1[KEY_INCAR_LOOKR] = 0x00;
	}
	wKeys >>= 1; // 2

	if (wKeys & 1) {
		pPlayerKeys->wKeys2[KEY_INCAR_LOOKL] = 0xFF;
		pPlayerKeys->wKeys1[KEY_INCAR_LOOKL] = 0xFF;
	}
	else {
		pPlayerKeys->wKeys2[KEY_INCAR_LOOKL] = 0x00;
		pPlayerKeys->wKeys1[KEY_INCAR_LOOKL] = 0x00;
	}
	wKeys >>= 1; // 3

	if (wKeys & 1) {
		pPlayerKeys->wKeys1[KEY_INCAR_TURRETUD] = 0xFF80;
		pPlayerKeys->wKeys2[KEY_INCAR_TURRETUD] = 0xFF80;
	}
	else {
		pPlayerKeys->wKeys1[KEY_INCAR_TURRETUD] = 0x0;
		pPlayerKeys->wKeys2[KEY_INCAR_TURRETUD] = 0x0;
	}
	wKeys >>= 1; // 4

	if (wKeys & 1) {
		pPlayerKeys->wKeys1[KEY_INCAR_TURRETUD] = 0x80;
		pPlayerKeys->wKeys2[KEY_INCAR_TURRETUD] = 0x80;
	}
	else {
		pPlayerKeys->wKeys1[KEY_INCAR_TURRETUD] = 0x0;
		pPlayerKeys->wKeys2[KEY_INCAR_TURRETUD] = 0x0;
	}
	wKeys >>= 1; // 5

	if (wKeys & 1)
		pPlayerKeys->wKeys2[KEY_ONFOOT_CROUCH] = 0xFF;
	else
		pPlayerKeys->wKeys2[KEY_ONFOOT_CROUCH] = 0x00;
	wKeys >>= 1; // 6

	if (wKeys & 1)
		pPlayerKeys->wKeys2[KEY_ONFOOT_FIRE] = 0xFF;
	else
		pPlayerKeys->wKeys2[KEY_ONFOOT_FIRE] = 0x00;
	wKeys >>= 1; // 7

	if (wKeys & 1)
		pPlayerKeys->wKeys2[KEY_ONFOOT_JUMP] = 0xFF;
	else
		pPlayerKeys->wKeys2[KEY_ONFOOT_JUMP] = 0x00;
	wKeys >>= 1; // 8

	if (wKeys & 1)
		pPlayerKeys->wKeys1[KEY_ONFOOT_CROUCH] = 0xFF;
	else
		pPlayerKeys->wKeys1[KEY_ONFOOT_CROUCH] = 0x00;
	wKeys >>= 1; // 9

	if (wKeys & 1)
		pPlayerKeys->wKeys1[KEY_ONFOOT_FIRE] = 0xFF;
	else
		pPlayerKeys->wKeys1[KEY_ONFOOT_FIRE] = 0x00;
	wKeys >>= 1; // 10

	if (wKeys & 1) {
		pPlayerKeys->wKeys1[KEY_ONFOOT_SPRINT] = 0xFF;
		pPlayerKeys->wKeys2[KEY_ONFOOT_SPRINT] = 0xFF;
	}
	else {
		pPlayerKeys->wKeys1[KEY_ONFOOT_SPRINT] = 0x00;
		pPlayerKeys->wKeys2[KEY_ONFOOT_SPRINT] = 0x00;
	}
	wKeys >>= 1; // 11

	if (wKeys & 1)
		pPlayerKeys->wKeys1[KEY_ONFOOT_JUMP] = 0xFF;
	else
		pPlayerKeys->wKeys1[KEY_ONFOOT_JUMP] = 0x00;
	wKeys >>= 1; // 12

	if (wKeys & 1) {
		pPlayerKeys->wKeys1[KEY_ONFOOT_RIGHT] = 0xFF;
		pPlayerKeys->wKeys2[KEY_ONFOOT_RIGHT] = 0xFF;
	}
	else {
		pPlayerKeys->wKeys1[KEY_ONFOOT_RIGHT] = 0x00;
		pPlayerKeys->wKeys2[KEY_ONFOOT_RIGHT] = 0x00;
	}
	wKeys >>= 1; // 13

	if (wKeys & 1) {
		pPlayerKeys->wKeys1[KEY_ONFOOT_LEFT] = 0xFF;
		pPlayerKeys->wKeys2[KEY_ONFOOT_LEFT] = 0xFF;
	}
	else {
		pPlayerKeys->wKeys1[KEY_ONFOOT_LEFT] = 0x00;
		pPlayerKeys->wKeys2[KEY_ONFOOT_LEFT] = 0x00;
	}
	wKeys >>= 1; // 14

	if (wKeys & 1) {
		pPlayerKeys->wKeys1[KEY_ONFOOT_BACKWARD] = 0xFF;
		pPlayerKeys->wKeys2[KEY_ONFOOT_BACKWARD] = 0xFF;
	}
	else {
		pPlayerKeys->wKeys1[KEY_ONFOOT_BACKWARD] = 0x00;
		pPlayerKeys->wKeys2[KEY_ONFOOT_BACKWARD] = 0x00;
	}
	wKeys >>= 1; // 15

	if (wKeys & 1) {
		pPlayerKeys->wKeys1[KEY_ONFOOT_FORWARD] = 0xFF; 
		pPlayerKeys->wKeys2[KEY_ONFOOT_FORWARD] = 0xFF;
	}
	else {
		pPlayerKeys->wKeys1[KEY_ONFOOT_FORWARD] = 0x00;
		pPlayerKeys->wKeys2[KEY_ONFOOT_FORWARD] = 0x00;
	}

	gGame->remotePlayerKeys[this->gameID] = *pPlayerKeys;
	return;
}

//-----------------------------------------------------------

WORD CClientPlayer::GetKeys()
{
	WORD wRet = 0;

	GTA_CONTROLSET * pInternalKeys = (GTA_CONTROLSET*)0x7DBCB0;

	// Just down keys
	if (pInternalKeys->wKeys1[KEY_ONFOOT_FORWARD]) wRet |= 1;
	wRet <<= 1;

	if (pInternalKeys->wKeys1[KEY_ONFOOT_BACKWARD]) wRet |= 1;
	wRet <<= 1;

	if (pInternalKeys->wKeys1[KEY_ONFOOT_LEFT]) wRet |= 1;
	wRet <<= 1;

	if (pInternalKeys->wKeys1[KEY_ONFOOT_RIGHT]) wRet |= 1;
	wRet <<= 1;

	if (pInternalKeys->wKeys1[KEY_ONFOOT_JUMP]) wRet |= 1;
	wRet <<= 1;

	if (pInternalKeys->wKeys1[KEY_ONFOOT_SPRINT]) wRet |= 1;
	wRet <<= 1;

	if (pInternalKeys->wKeys1[KEY_ONFOOT_FIRE]) wRet |= 1;
	wRet <<= 1;

	if (pInternalKeys->wKeys1[KEY_ONFOOT_CROUCH]) wRet |= 1;
	wRet <<= 1;

	// Jump held
	if (pInternalKeys->wKeys2[KEY_ONFOOT_JUMP]) wRet |= 1;
	wRet <<= 1;

	// Fire held
	if (pInternalKeys->wKeys2[KEY_ONFOOT_FIRE]) wRet |= 1;
	wRet <<= 1;

	// Crouch held
	if (pInternalKeys->wKeys2[KEY_ONFOOT_CROUCH]) wRet |= 1;
	wRet <<= 1;

	if (pInternalKeys->wKeys1[KEY_INCAR_TURRETUD] == 0x80) wRet |= 1;
	wRet <<= 1;

	if (pInternalKeys->wKeys1[KEY_INCAR_TURRETUD] == 0xFF80) wRet |= 1;
	wRet <<= 1;

	if (pInternalKeys->wKeys2[KEY_INCAR_LOOKL]) wRet |= 1;
	wRet <<= 1;

	if (pInternalKeys->wKeys2[KEY_INCAR_LOOKR]) wRet |= 1;
	wRet <<= 1;

	if (pInternalKeys->wKeys1[KEY_INCAR_HANDBRAKE]) wRet |= 1;

	return wRet;
}