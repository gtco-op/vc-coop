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

class MyPlugin {
public:		
	//======================== CHUD ==========================
	int ShowHelpMessage(wchar_t text, bool quickmessage, bool forever, bool showinbrief)
		{
			CHud::SetHelpMessage((unsigned short*)text, quickmessage, forever, showinbrief);
			return 1;
			
		}
	//====================== CPED =============================
	int SpawnPed(unsigned int modelindex, CVector coords, ePedType pedtype)
	{
		int id;
		CPed *ped = CPopulation::AddPed(pedtype, modelindex, coords, 0);
		CWorld::Add(ped);
		return id;
	}
		int SpawnPed(unsigned int modelindex, CVector coords, ePedType pedtype)
	{
		int id;
		CPed *ped = CPopulation::AddPed(pedtype, modelindex, coords, 0);
		CWorld::Add(ped);
		
	}
	int CreateCoupleOfPeds(ePedType PedType, unsigned int Model, ePedType PedType2, unsigned int Model2, CVector Position)
	{
		CPopulation::PlaceCouple(PedType, Model, PedType2, Model2, Position);
	}
	int GivePedrWeapon(CPed Ped, eWeaponType WepType, unsigned int Ammo)
	{
	 // this needs some load model work
		Ped.GiveWeapon(WepType, Ammo, false);
		Ped.SetCurrentWeapon(WepType);
	}
	int KillPed(CPed Ped)
	{
		Ped.m_fHealth = 0.0;
	}
	int TeleportPed(CPed Ped, float PosX, float PosY, float PosZ)
	{
		Ped.Teleport(CVector(PosX, PosY, PosZ));
	}
	int PedSetAimAtEntity(CPed Ped, CEntity Entity)
	{
		Ped.m_pPointGunAt = &Entity;
		Ped.AimGun();
	}
	int SetPedHealth(CPed Ped, float Amount)
	{
		Ped.m_fHealth = Amount;
	}
	int SetPedArmour(CPed Ped, float Amount)
	{
		Ped.m_fArmour = Amount;
	}
    MyPlugin() {


    }
} myPlugin;
