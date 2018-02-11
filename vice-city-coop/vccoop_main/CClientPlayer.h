#pragma once

class CClientPlayer
{
public:
	char szName[25];
	int networkID;
	int gameID;
	CPed * ped;
	PlayerSyncData syncData;

	CClientPlayer(int nID);
	CClientPlayer(int nID, int gID);
	~CClientPlayer();

	void SyncPlayer(PlayerSyncData spd);
	PlayerSyncData CClientPlayer::BuildSyncData();
};