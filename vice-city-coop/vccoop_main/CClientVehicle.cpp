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

void CClientVehicle::SyncVehicle(VehicleSyncData spd)
{
	if (spd.vehicleID == -1)return;
	CVehicle * veh = (CVehicle*)gNetwork->GetEntityFromNetworkID(spd.vehicleID);
	CPed * ped = NULL;
	
	if(spd.driver)ped = (CPed*)gNetwork->GetEntityFromNetworkID(spd.driver);
	else
	{
		if (veh->m_pDriver && veh->m_pDriver != LocalPlayer())ped->SetExitCar(veh, 0);
	}

	//gLog->Log("remote ped is in vehicle %d 0x%X", spd.vehicleID, veh);
	if (veh)
	{
		if (ped && (!ped->m_bInVehicle || ped->m_pVehicle != veh))
		{
			//gLog->Log("ped wasn't in the car so warped in");
			ped->WarpPedIntoCar(veh); 
		}
		//gLog->Log("teleporting vehicle");
		//veh->Teleport(spd.vehiclePos);
		//gLog->Log("Setting matrix");
		veh->m_placement.at = spd.vehicleAt;
		veh->m_placement.right = spd.vehicleRight;
		veh->m_placement.up = spd.vehicleUp;

		veh->m_fHealth = spd.Health;

		veh->m_vecMoveSpeed = spd.speed;
		veh->ApplyMoveSpeed();
		//gLog->Log("Vehicle sync done");
	}
}

VehicleSyncData CClientVehicle::BuildSyncData()
{
	VehicleSyncData spd;

	spd.vehicleID = gNetwork->GetNetworkIDFromEntity(this->veh);
	spd.Health = this->veh->m_fHealth;
	spd.vehiclePos = this->veh->GetPosition();
	spd.vehicleAt = this->veh->m_placement.at;
	spd.vehicleRight = this->veh->m_placement.right;
	spd.vehicleUp = this->veh->m_placement.up;
	spd.speed = this->veh->m_vecMoveSpeed;

	if(this->veh->m_pDriver)spd.driver = gNetwork->GetNetworkIDFromEntity(this->veh->m_pDriver);
	else spd.driver = -1;

	return spd;
}