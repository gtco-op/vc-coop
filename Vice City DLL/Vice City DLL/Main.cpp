/*
	Vice City DLL (VCCOOPDLL)
	DLL to manage Vice City game
	File Author(s): Zeyad_Ahmed
				    Kewun
	Copyrights (c) 2017 VC:CO-OP Team
	Licence: GPL v3
*/
#include "plugin_vc.h"
#include <game_vc/CHud.h>
#include <game_vc/CPed.h>
#include <game_vc/CPopulation.h>
#include <game_vc\ePedType.h>
#include <game_vc/CStreaming.h>
#include <game_vc\CWorld.h>
#include <game_vc/CPlayerPed.h>
using namespace plugin;
// functions always need to be outside the myplugin class, otherwise game will close

// === CSTREAMING ===
void CStreamingRequestAllWepModels()
{
	CStreaming::RequestModel(269, 1);
	CStreaming::RequestModel(270, 1);
	CStreaming::RequestModel(275, 1);
	CStreaming::RequestModel(278, 1);
	CStreaming::RequestModel(284, 1);
	CStreaming::RequestModel(280, 1);
	CStreaming::RequestModel(286, 1);
	CStreaming::RequestModel(290, 1);
	CStreaming::RequestModel(294, 1);
	CStreaming::RequestModel(259, 1);
	CStreaming::RequestModel(264, 1);
	CStreaming::RequestModel(272, 1);
	CStreaming::RequestModel(274, 1);
	CStreaming::RequestModel(277, 1);
	CStreaming::RequestModel(281, 1);
	CStreaming::RequestModel(276, 1);
	CStreaming::RequestModel(285, 1);
	CStreaming::RequestModel(288, 1);
	CStreaming::RequestModel(268, 1);
	CStreaming::RequestModel(270, 1);
	CStreaming::RequestModel(291, 1);
	CStreaming::RequestModel(275, 1);
	CStreaming::RequestModel(279, 1);
	CStreaming::RequestModel(283, 1);
	CStreaming::RequestModel(280, 1);
	CStreaming::RequestModel(286, 1);
	CStreaming::RequestModel(287, 1);
	CStreaming::RequestModel(269, 1);
	CStreaming::RequestModel(270, 1);
	CStreaming::RequestModel(275, 1);
	CStreaming::RequestModel(278, 1);
	CStreaming::RequestModel(284, 1);
	CStreaming::RequestModel(280, 1);
	CStreaming::RequestModel(286, 1);
	CStreaming::RequestModel(290, 1);
	CStreaming::RequestModel(294, 1);
	CStreaming::LoadAllRequestedModels(0, 0);
}
// === CHUD ===
void ShowHelpMessage(wchar_t text, bool quickmessage, bool forever, bool showinbrief)
{
	CHud::SetHelpMessage((char*)L"Text", quickmessage, forever, showinbrief);
		
}
// === CPED ===
void SpawnPed(unsigned int modelindex, CVector coords, ePedType pedtype)
{
	CPed *ped = CPopulation::AddPed(pedtype, modelindex, coords, 0);
	CWorld::Add(ped);
}
void CreateCoupleOfPeds(ePedType PedType, unsigned int Model, ePedType PedType2, unsigned int Model2, CVector Position)
{
	CPopulation::PlaceCouple(PedType, Model, PedType2, Model2, Position);
}
void GivePedWeapon(CPed * Ped, eWeaponType WepType, unsigned int Ammo)
{
	CPlayerPed*player{ (CPlayerPed*)Ped };
	player->GiveWeapon(WepType,Ammo,false);
	player->SetCurrentWeapon(WepType);
}
void KillPed(CPed * Ped)
{
	CPlayerPed*player{ (CPlayerPed*)Ped };
	player->m_fHealth = 0.0;
}
void TeleportPed(CPed * Ped, float PosX, float PosY, float PosZ)
{
	CPlayerPed*player{ (CPlayerPed*)Ped };
	player->Teleport(CVector(PosX, PosY, PosZ));
}
void PedSetAimAtEntity(CPed * Ped, CEntity* Entity)
{
	CPlayerPed*player{ (CPlayerPed*)Ped };
	player->m_pPointGunAt = Entity;
	player->AimGun();
}
void SetPedHealth(CPed * Ped, float Amount)
{
	CPlayerPed*player{ (CPlayerPed*)Ped };
	player->m_fHealth = Amount;
}
void SetPedArmour(CPed * Ped, float Amount)
{
	CPlayerPed*player{ (CPlayerPed*)Ped };
	player->m_fArmour = Amount;
}


class MyPlugin {
public:		

    MyPlugin() {


    }
} myPlugin;

BOOL WINAPI DllMain(IN HINSTANCE hDllHandle, IN DWORD nReason, IN LPVOID Reserved)
{



	//  Perform global initialization.

	switch (nReason)
	{
	case DLL_PROCESS_ATTACH:

		//  For optimization.

		DisableThreadLibraryCalls(hDllHandle);

		break;

	case DLL_PROCESS_DETACH:

		break;
	}
}
// :v
