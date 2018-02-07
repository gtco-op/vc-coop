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