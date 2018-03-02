#pragma once
class CClientVehicle
{
public:
	CVehicle *veh;
	bool streamed;
	int networkID;
	int model;
	VehicleSyncData syncData;

	CClientVehicle(int nID);
	~CClientVehicle();

	void Respawn();
	void StreamIn();
	void StreamOut();

	void SyncVehicle(VehicleSyncData spd);
	VehicleSyncData BuildSyncData();
};