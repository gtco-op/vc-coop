#pragma once

class CClientPed : public CClientEntity
{
public:
	CPed * ped;
	PedSyncData syncData;

	CClientPed(int nID);
	~CClientPed();

	void SyncPed(PedSyncData spd);
	PedSyncData BuildSyncData();

	CEntity * GetEntity();
};