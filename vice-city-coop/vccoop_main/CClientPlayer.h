#pragma once

class CClientPlayer
{
public:
	char szName[25];
	int networkID;
	int gameID;
	CPed * ped;
	PlayerSyncData syncData;
	bool streamed;

	CClientPlayer(int nID);
	CClientPlayer(int nID, int gID);
	~CClientPlayer();

	void Respawn();

	void SyncPlayer(PlayerSyncData spd);
	PlayerSyncData CClientPlayer::BuildSyncData();

	void StreamIn();
	void StreamOut();
};