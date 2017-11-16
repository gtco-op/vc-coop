// VCCOOPDll.h - Contains declaration of Function class  
#pragma once  
#include "plugin.h"
#include "CStreaming.h"
#include "CHud.h"
#include "CWorld.h"
#include "CPopulation.h"

#define GAME_REQUEST 1


#ifdef VCLIB_EXPORTS  
#define VCLIB_API __declspec(dllexport)   
#else  
#define VCLIB_API __declspec(dllimport)   
#endif  

namespace VCLib
{
	// This class is exported from the VCCOOPDll.dll  
	class Functions
	{
	public:
		// === CSTREAMING ===
		static VCLIB_API void CStreamingRequestAllWepModels();
		// === CHUD ===
		static VCLIB_API void ShowHelpMessage(wchar_t* text, bool quickmessage, bool forever, bool showinbrief);
		// === CPED ===
		static VCLIB_API void SpawnPed(unsigned int modelindex, CVector coords, ePedType pedtype);
		static VCLIB_API void CreateCoupleOfPeds(ePedType PedType, unsigned int Model, ePedType PedType2, unsigned int Model2, CVector Position);
		static VCLIB_API void GivePedWeapon(CPed * Ped, eWeaponType WepType, unsigned int Ammo);
		static VCLIB_API void KillPed(CPed * Ped);
		static VCLIB_API void TeleportPed(CPed * Ped, float PosX, float PosY, float PosZ);
		static VCLIB_API void PedSetAimAtEntity(CPed * Ped, CEntity* Entity);
		static VCLIB_API void SetPedHealth(CPed * Ped, float Amount);
		static VCLIB_API void SetPedArmour(CPed * Ped, float Amount);

	};

	


}