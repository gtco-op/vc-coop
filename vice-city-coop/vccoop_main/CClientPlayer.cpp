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
	//gLog->Log("GameID: %d\nNetwork ID: %d\nPed pointer: 0x%X\n\n", gID, nID, ped);

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
