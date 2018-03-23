#pragma once
class CClientVehicle : public CClientEntity
{
public:
	CVehicle *veh;
	bool streamed;
	int model;
	VehicleSyncData syncData;

	CClientVehicle(int nID, int modelID, CVector position, int PrimaryColor, int SecondaryColor);
	~CClientVehicle();

	void Respawn();
	void StreamIn();
	void StreamOut();

	void SyncVehicle(VehicleSyncData spd);
	VehicleSyncData BuildSyncData();

	CEntity * GetEntity();
};