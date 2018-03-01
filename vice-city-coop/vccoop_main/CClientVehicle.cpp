#include "main.h"

CClientVehicle::CClientVehicle(int nID)
{
	this->model = 130;
	this->veh = gGame->CreateVehicle(this->model, { VCCOOP_DEFAULT_SPAWN_POSITION });
	this->streamed = true;
	this->networkID = nID;

	gLog->Log("[CClientVehicle]Network ID: %d Veh pointer: 0x%X\n\n", nID, this->veh);
}

CClientVehicle::~CClientVehicle()
{
	if (this->veh)
	{
		CWorld::Remove(this->veh);
		if (this->veh)
		{
			this->veh->Remove();
			CVehicle::operator delete(this->veh);
			this->veh = NULL;
		}
	}
	this->networkID = -1;
}

void CClientVehicle::StreamIn()
{
	if (this->streamed)return;

	this->veh = gGame->CreateVehicle(this->model, { VCCOOP_DEFAULT_SPAWN_POSITION });
	this->streamed = true;
}

void CClientVehicle::StreamOut()
{
	if (this->streamed == false)return;
	this->streamed = false;

	if (this->veh)
	{
		CWorld::Remove(this->veh);
		if (this->veh)
		{
			this->veh->Remove();
			CVehicle::operator delete(this->veh);
			this->veh = NULL;
		}
	}
}