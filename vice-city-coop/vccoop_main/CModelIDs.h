#pragma once

#define MAX_OBJ_MODEL_ID 288
#define MAX_PED_MODEL_ID 195
#define MAX_VEH_MODEL_ID 104

enum eModelType {
	MODEL_OBJ,
	MODEL_PED,
	MODEL_VEH
};

class CModelIDs
{
public:
	CModelIDs() {}
	~CModelIDs() {}

	static bool IsValidObjectModel(int modelID);
	static bool IsValidPedModel(int modelID);
	static bool IsValidVehicleModel(int modelID);
	
	static int GetRandomModel(eModelType type = MODEL_PED);

	static const int objModelIDs[MAX_OBJ_MODEL_ID];
	static const int pedModelIDs[MAX_PED_MODEL_ID];
	static const int vehModelIDs[MAX_VEH_MODEL_ID];
};

