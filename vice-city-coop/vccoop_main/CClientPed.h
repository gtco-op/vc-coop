#pragma once

class CClientPed : public CClientEntity
{
public:
	int networkID;
	CPed * ped;
	PedSyncData syncData;

	CClientPed(int nID);
	~CClientPed();

	void SyncPed(PedSyncData spd);
	PedSyncData BuildSyncData();

	CEntity * GetEntity();
};