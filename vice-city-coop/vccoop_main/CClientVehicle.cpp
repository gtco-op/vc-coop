#include "main.h"

CClientVehicle::CClientVehicle(int nID, int modelID)
{
	this->model = modelID;
	this->veh = gGame->CreateVehicle(this->model, { VCCOOP_DEFAULT_SPAWN_POSITION });
	this->streamed = true;
	this->networkID = nID;
	this->type = VCOOP_VEHICLE;

	gLog->Log("[CClientVehicle] Network ID: %d Veh pointer: 0x%X ModelID: %d\n", nID, this->veh, modelID);
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

	if (veh)
	{
		if (ped && (!ped->m_bInVehicle || ped->m_pVehicle != veh))
		{
			gLog->Log("[CClientVehicle] Ped wasn't in the car so warped in\n");
			ped->SetObjective(eObjective::OBJECTIVE_ENTER_CAR_AS_DRIVER, (void*)veh);
			ped->WarpPedIntoCar(veh); 
			veh->SetDriver(ped);
			veh->m_pDriver = ped;
			ped->m_bInVehicle = true;
			ped->m_pVehicle = veh;
		}
		if(!veh->m_nVehicleFlags.bIsEngineOn)veh->m_nVehicleFlags.bIsEngineOn = true;

		float fDif = DistanceBetweenPoints(veh->GetPosition(), spd.vehiclePos);
		if (fDif > 0.1) {
			veh->m_placement.pos.x = spd.vehiclePos.x;
			veh->m_placement.pos.y = spd.vehiclePos.y;
			veh->m_placement.pos.z = spd.vehiclePos.z;
		}

		veh->m_placement.at = spd.vehicleAt;
		veh->m_placement.right = spd.vehicleRight;
		veh->m_placement.up = spd.vehicleUp;

		veh->m_fHealth = spd.Health;

		veh->m_vecMoveSpeed = spd.moveSpeed;
		veh->ApplyMoveSpeed();

		veh->m_vecTurnSpeed = spd.turnSpeed;
		veh->ApplyTurnSpeed();
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
	spd.moveSpeed = this->veh->m_vecMoveSpeed;
	spd.turnSpeed = this->veh->m_vecTurnSpeed;

	if(this->veh->m_pDriver)spd.driver = gNetwork->GetNetworkIDFromEntity(this->veh->m_pDriver);
	else spd.driver = -1;

	return spd;
}

CEntity * CClientVehicle::GetEntity()
{
	return this->veh;
}