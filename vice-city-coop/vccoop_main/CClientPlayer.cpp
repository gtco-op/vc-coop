#include "main.h"

CClientPlayer::CClientPlayer(int nID, int gID)
{
	ped = NULL;
	int dwPlayerID = 0;
	Command<0x0053>(gID, 10.0f, 5.0f, 25.0f);
	Command<0x01F5>(gID, &dwPlayerID);
	ped = gGame->GamePool_Ped_GetAt(dwPlayerID);
	gGame->players[gID] = (DWORD)ped;
	ped->Teleport({ VCCOOP_DEFAULT_SPAWN_POSITION });
	ped->SetModelIndex(7);
	this->gameID = gID;
	this->networkID = nID;

	gLog->Log("GameID: %d\nNetwork ID: %d\n", gID, nID);
}
CClientPlayer::CClientPlayer(int nID)
{
	ped = FindPlayerPed();
	this->gameID = 0;
	this->networkID = nID;
}
CClientPlayer::~CClientPlayer()
{
	CWorld::Remove(ped);
	ped = NULL;
	gGame->players[this->gameID] = 0;
	this->gameID = -1;
	this->networkID = -1;
}

WORD CClientPlayer::GetKeys()
{
	WORD wRet = 0;

	GTA_CONTROLSET *pInternalKeys = (GTA_CONTROLSET*)0x7DBCB0;

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


GTA_CONTROLSET CClientPlayer::SetKeys(WORD wKeys)
{
	GTA_CONTROLSET pPlayerKeys = gGame->remotePlayerKeys[this->gameID];

	if (wKeys & 1) {
		pPlayerKeys.wKeys2[KEY_INCAR_HANDBRAKE] = 0xFF;
		pPlayerKeys.wKeys1[KEY_INCAR_HANDBRAKE] = 0xFF;
	}
	else {
		pPlayerKeys.wKeys2[KEY_INCAR_HANDBRAKE] = 0x00;
		pPlayerKeys.wKeys1[KEY_INCAR_HANDBRAKE] = 0x00;
	}
	wKeys >>= 1; // 1

	if (wKeys & 1) {
		pPlayerKeys.wKeys2[KEY_INCAR_LOOKR] = 0xFF;
		pPlayerKeys.wKeys1[KEY_INCAR_LOOKR] = 0xFF;
	}
	else {
		pPlayerKeys.wKeys2[KEY_INCAR_LOOKR] = 0x00;
		pPlayerKeys.wKeys1[KEY_INCAR_LOOKR] = 0x00;
	}
	wKeys >>= 1; // 2

	if (wKeys & 1) {
		pPlayerKeys.wKeys2[KEY_INCAR_LOOKL] = 0xFF;
		pPlayerKeys.wKeys1[KEY_INCAR_LOOKL] = 0xFF;
	}
	else {
		pPlayerKeys.wKeys2[KEY_INCAR_LOOKL] = 0x00;
		pPlayerKeys.wKeys1[KEY_INCAR_LOOKL] = 0x00;
	}
	wKeys >>= 1; // 3

	if (wKeys & 1) {
		pPlayerKeys.wKeys1[KEY_INCAR_TURRETUD] = 0xFF80;
		pPlayerKeys.wKeys2[KEY_INCAR_TURRETUD] = 0xFF80;
	}
	else {
		pPlayerKeys.wKeys1[KEY_INCAR_TURRETUD] = 0x0;
		pPlayerKeys.wKeys2[KEY_INCAR_TURRETUD] = 0x0;
	}
	wKeys >>= 1; // 4

	if (wKeys & 1) {
		pPlayerKeys.wKeys1[KEY_INCAR_TURRETUD] = 0x80;
		pPlayerKeys.wKeys2[KEY_INCAR_TURRETUD] = 0x80;
	}
	else {
		pPlayerKeys.wKeys1[KEY_INCAR_TURRETUD] = 0x0;
		pPlayerKeys.wKeys2[KEY_INCAR_TURRETUD] = 0x0;
	}
	wKeys >>= 1; // 5

	if (wKeys & 1)
		pPlayerKeys.wKeys2[KEY_ONFOOT_CROUCH] = 0xFF;
	else
		pPlayerKeys.wKeys2[KEY_ONFOOT_CROUCH] = 0x00;
	wKeys >>= 1; // 6

	if (wKeys & 1)
		pPlayerKeys.wKeys2[KEY_ONFOOT_FIRE] = 0xFF;
	else
		pPlayerKeys.wKeys2[KEY_ONFOOT_FIRE] = 0x00;
	wKeys >>= 1; // 7

	if (wKeys & 1)
		pPlayerKeys.wKeys2[KEY_ONFOOT_JUMP] = 0xFF;
	else
		pPlayerKeys.wKeys2[KEY_ONFOOT_JUMP] = 0x00;
	wKeys >>= 1; // 8

	if (wKeys & 1)
		pPlayerKeys.wKeys1[KEY_ONFOOT_CROUCH] = 0xFF;
	else
		pPlayerKeys.wKeys1[KEY_ONFOOT_CROUCH] = 0x00;
	wKeys >>= 1; // 9

	if (wKeys & 1)
		pPlayerKeys.wKeys1[KEY_ONFOOT_FIRE] = 0xFF;
	else
		pPlayerKeys.wKeys1[KEY_ONFOOT_FIRE] = 0x00;
	wKeys >>= 1; // 10

	if (wKeys & 1) {
		pPlayerKeys.wKeys1[KEY_ONFOOT_SPRINT] = 0xFF;
		pPlayerKeys.wKeys2[KEY_ONFOOT_SPRINT] = 0xFF;
	}
	else {
		pPlayerKeys.wKeys1[KEY_ONFOOT_SPRINT] = 0x00;
		pPlayerKeys.wKeys2[KEY_ONFOOT_SPRINT] = 0x00;
	}
	wKeys >>= 1; // 11

	if (wKeys & 1)
		pPlayerKeys.wKeys1[KEY_ONFOOT_JUMP] = 0xFF;
	else
		pPlayerKeys.wKeys1[KEY_ONFOOT_JUMP] = 0x00;
	wKeys >>= 1; // 12

	if (wKeys & 1) {
		pPlayerKeys.wKeys1[KEY_ONFOOT_RIGHT] = 0xFF;
		pPlayerKeys.wKeys2[KEY_ONFOOT_RIGHT] = 0xFF;
	}
	else {
		pPlayerKeys.wKeys1[KEY_ONFOOT_RIGHT] = 0x00;
		pPlayerKeys.wKeys2[KEY_ONFOOT_RIGHT] = 0x00;
	}
	wKeys >>= 1; // 13

	if (wKeys & 1) {
		pPlayerKeys.wKeys1[KEY_ONFOOT_LEFT] = 0xFF;
		pPlayerKeys.wKeys2[KEY_ONFOOT_LEFT] = 0xFF;
	}
	else {
		pPlayerKeys.wKeys1[KEY_ONFOOT_LEFT] = 0x00;
		pPlayerKeys.wKeys2[KEY_ONFOOT_LEFT] = 0x00;
	}
	wKeys >>= 1; // 14

	if (wKeys & 1) {
		pPlayerKeys.wKeys1[KEY_ONFOOT_BACKWARD] = 0xFF;
		pPlayerKeys.wKeys2[KEY_ONFOOT_BACKWARD] = 0xFF;
	}
	else {
		pPlayerKeys.wKeys1[KEY_ONFOOT_BACKWARD] = 0x00;
		pPlayerKeys.wKeys2[KEY_ONFOOT_BACKWARD] = 0x00;
	}
	wKeys >>= 1; // 15

	if (wKeys & 1) {
		pPlayerKeys.wKeys1[KEY_ONFOOT_FORWARD] = 0xFF;
		pPlayerKeys.wKeys2[KEY_ONFOOT_FORWARD] = 0xFF;
	}
	else {
		pPlayerKeys.wKeys1[KEY_ONFOOT_FORWARD] = 0x00;
		pPlayerKeys.wKeys2[KEY_ONFOOT_FORWARD] = 0x00;
	}

	return pPlayerKeys;
}