#pragma once

#define MAX_PED_MODEL_ID 195
#define MAX_VEH_MODEL_ID 104

class CModelIDs
{
public:
	CModelIDs() {}
	~CModelIDs() {}

	static bool IsValidPedModel(int modelID);
	static bool IsValidVehicleModel(int modelID);

	static int GetRandomModel(bool ped = true);

	static const int pedModelIDs[MAX_PED_MODEL_ID];
	static const int vehModelIDs[MAX_VEH_MODEL_ID];
};

