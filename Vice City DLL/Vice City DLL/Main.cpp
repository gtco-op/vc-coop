/*
	Vice City DLL (VCCOOPDLL)
	DLL to manage Vice City game
	File Author(s): Zeyad_Ahmed
	Copyrights (c) 2017 VC:CO-OP Team
	Licence: GPL v3
*/
#include "plugin_vc.h"
#include <game_vc/CHud.h>
#include <game_vc/CPed.h>
#include <game_vc/CPopulation.h>
#include <game_vc\ePedType.h>
#include <game_vc\CWorld.h>
using namespace plugin;
// functions always need to be outside the myplugin class, otherwise game will close
void ShowHelpMessage(wchar_t text, bool quickmessage, bool forever, bool showinbrief)
{
	CHud::SetHelpMessage((unsigned short*)text, quickmessage, forever, showinbrief);
		
}
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
 // this needs some load model work
	CPlayerPed*player{ (CPlayerPed*)Ped };
	player.GiveWeapon(WepType,Ammo,false);
	
	player.SetCurrentWeapon(WepType);
}
void KillPed(CPed * Ped)
{
	CPlayerPed*player{ (CPlayerPed*)Ped };
	player->m_fHealth = 0.0;
}
void TeleportPed(CPed * Ped, float PosX, float PosY, float PosZ)
{
	CPlayerPed*player{ (CPlayerPed*)Ped };
	player.Teleport(CVector(PosX, PosY, PosZ));
}
void PedSetAimAtEntity(CPed * Ped, CEntity Entity)
{
	CPlayerPed*player{ (CPlayerPed*)Ped };
	player.m_pPointGunAt = &Entity;
	player.AimGun();
}
void SetPedHealth(CPed * Ped, float Amount)
{
	CPlayerPed*player{ (CPlayerPed*)Ped };
	player.m_fHealth = Amount;
}
void SetPedArmour(CPed * Ped, float Amount)
{
	CPlayerPed*player{ (CPlayerPed*)Ped };
	player.m_fArmour = Amount;
}


class MyPlugin {
public:		

    MyPlugin() {


    }
} myPlugin;
// t
