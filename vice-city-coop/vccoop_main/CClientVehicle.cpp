#include "main.h"

CClientVehicle::CClientVehicle(int nID, int modelID, CVector position, int PrimaryColor, int SecondaryColor)
{
	this->type						= VCOOP_VEHICLE;
	this->model						= modelID;
	this->streamed					= true;
	this->networkID					= nID;

	this->veh						= gGame->CreateVehicle(this->model, position);
	this->veh->m_nPrimaryColor		= PrimaryColor;
	this->veh->m_nSecondaryColor	= SecondaryColor;
	
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
	if (this->streamed)
		return;
		
	if(this->veh == nullptr)
	{
		this->veh = gGame->CreateVehicle(this->model, {VCCOOP_DEFAULT_SPAWN_POSITION});
	}

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
	if (spd.vehicleID == -1)
		return;

	CVehicle* veh		= (CVehicle*)gNetwork->GetEntityFromNetworkID(spd.vehicleID);
	CPed*	  ped		= nullptr;

	if (spd.driver >= 0)
	{
		ped = (CPed*)gNetwork->GetEntityFromNetworkID(spd.driver);

		if (ped && veh)		{
			ped->WarpPedIntoCar(veh);
			veh->SetDriver(ped);
			veh->m_pDriver = ped;
			ped->m_bInVehicle = true;
			ped->m_pVehicle = veh;
		}
	}
	else
	{
		if (veh->m_pDriver && veh->m_pDriver != LocalPlayer())
			veh->m_pDriver->SetExitCar(veh, 0);
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

			gLog->Log("Set and warped driver..\n");
		}

		if(!veh->m_nVehicleFlags.bIsEngineOn)
			veh->m_nVehicleFlags.bIsEngineOn = true;

		float fDif = DistanceBetweenPoints(veh->GetPosition(), spd.vehiclePos);
		if (fDif > 0.1) {
			veh->m_placement.pos.x = spd.vehiclePos.x;
			veh->m_placement.pos.y = spd.vehiclePos.y;
			veh->m_placement.pos.z = spd.vehiclePos.z;
		}

		veh->m_placement.SetOrientation(spd.OrientX, spd.OrientY, spd.OrientZ);

		veh->m_fHealth				= spd.Health;
		veh->m_nModelIndex			= spd.modelID;
		veh->m_nPrimaryColor		= spd.nPrimaryColor;
		veh->m_nSecondaryColor		= spd.nSecondaryColor;
		
		veh->m_placement.at			= spd.vehicleAt;
		veh->m_placement.right		= spd.vehicleRight;
		veh->m_placement.up			= spd.vehicleUp;
	
		for (int i = 0; i < 8; i++) {
			if (spd.passengers[i] >= 0)			{
				CPed* ptr = (CPed*)gNetwork->GetEntityFromNetworkID(spd.passengers[i]);

				if (ptr)
					veh->m_passengers[i] = (CPed*)gNetwork->GetEntityFromNetworkID(spd.passengers[i]); gLog->Log("Set passenger[%d] to %d\n", i, spd.passengers[i]);
			}
		}

		veh->m_nNumPassengers		= spd.NumPassengers;
		veh->m_nNumGettingIn		= spd.NumGettingIn;
		veh->m_nGettingInFlags		= spd.GettingInFlags;
		veh->m_nMaxPassengers		= spd.MaxPassengers;
		
		veh->m_vecMoveSpeed			= spd.moveSpeed;
		veh->ApplyMoveSpeed();

		veh->m_vecTurnSpeed			= spd.turnSpeed;
		veh->ApplyTurnSpeed();
	}
}

VehicleSyncData CClientVehicle::BuildSyncData()
{
	VehicleSyncData spd;

	spd.vehicleID = gNetwork->GetNetworkIDFromEntity(this->veh);
	spd.modelID = this->veh->m_nModelIndex;
	spd.Health = this->veh->m_fHealth;
	spd.vehiclePos = this->veh->GetPosition();
	spd.vehicleAt = this->veh->m_placement.at;
	spd.vehicleRight = this->veh->m_placement.right;
	spd.vehicleUp = this->veh->m_placement.up;
	spd.moveSpeed = this->veh->m_vecMoveSpeed;
	spd.turnSpeed = this->veh->m_vecTurnSpeed;

	spd.nPrimaryColor = this->veh->m_nPrimaryColor;
	spd.nSecondaryColor = this->veh->m_nSecondaryColor;

	spd.NumPassengers		= this->veh->m_nNumPassengers;
	spd.NumGettingIn		= this->veh->m_nNumGettingIn;
	spd.GettingInFlags		= this->veh->m_nGettingInFlags;
	spd.GettingOutFlags		= this->veh->m_nGettingOutFlags;
	spd.MaxPassengers		= this->veh->m_nMaxPassengers;
	
	this->veh->m_placement.GetOrientation(spd.OrientX, spd.OrientY, spd.OrientZ);

	if(this->veh->m_pDriver)
		spd.driver = gNetwork->GetNetworkIDFromEntity(this->veh->m_pDriver);

	return spd;
}

CEntity * CClientVehicle::GetEntity()
{
	return this->veh;
}