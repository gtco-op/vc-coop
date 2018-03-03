#pragma once

class CClientPlayer : public CClientEntity
{
public:
	char szName[25];
	int gameID;
	CPed * ped;
	PlayerSyncData syncData;
	bool streamed;

	CClientPlayer(int nID);
	CClientPlayer(int nID, int gID);
	~CClientPlayer();

	void SyncPlayer(PlayerSyncData spd);
	PlayerSyncData BuildSyncData();

	void Respawn();
	void StreamIn();
	void StreamOut();

	CEntity * GetEntity();
};