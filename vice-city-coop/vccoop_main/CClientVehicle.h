#pragma once
class CClientVehicle
{
public:
	CVehicle *veh;
	bool streamed;
	int networkID;
	int model;

	CClientVehicle(int nID);
	~CClientVehicle();

	void Respawn();
	void StreamIn();
	void StreamOut();
};