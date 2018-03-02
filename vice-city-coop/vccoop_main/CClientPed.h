#pragma once

class CClientPed
{
public:
	int networkID;
	CPed * ped;
	PedSyncData syncData;

	CClientPed(int nID);
	~CClientPed();

	void SyncPed(PedSyncData spd);
	PedSyncData BuildSyncData();
};