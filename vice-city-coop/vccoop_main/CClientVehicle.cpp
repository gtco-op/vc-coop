#include "main.h"
#include "CModelInfo.h"
#include "CBike.h"

CVehicle * CreateVehicle(unsigned int modelIndex, CVector position) 
{
	CStreaming::RequestModel(modelIndex, 22);
	CStreaming::LoadAllRequestedModels(false);
	if (CStreaming::ms_aInfoForModel[modelIndex].m_nLoadState == LOADSTATE_LOADED) 
	{
		CVehicle *vehicle = nullptr;
		switch (reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[modelIndex])->m_nVehicleType) 
		{
		case VEHICLE_HELI:
			vehicle = new CHeli(modelIndex, 1);
			break;
		case VEHICLE_PLANE:
			vehicle = new CPlane(modelIndex, 1);
			break;
		case VEHICLE_BIKE:
			vehicle = new CBike(modelIndex, 1);
			break;
		case VEHICLE_BOAT:
			vehicle = new CBoat(modelIndex, 1);
			break;
		default:
			vehicle = new CAutomobile(modelIndex, 1);
			break;
		}
		if (vehicle) 
		{
			vehicle->Teleport(position);
			CWorld::Add(vehicle);
			return vehicle;
		}
	}
	return nullptr;
}