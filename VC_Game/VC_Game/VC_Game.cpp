#include "VC_Game.h"

using namespace plugin;

namespace VCLib
{

	// === CSTREAMING ===
	void Functions::CStreamingRequestAllWepModels()
	{
			int i;
			CStreaming::RequestModel(259, GAME_REQUEST);
			CStreaming::RequestModel(264, GAME_REQUEST);
			for (i = 268; i < 271; i++) { CStreaming::RequestModel(i, GAME_REQUEST); }
			CStreaming::RequestModel(272, GAME_REQUEST);
			for (i = 274; i < 289; i++) { CStreaming::RequestModel(i, GAME_REQUEST); }
			CStreaming::RequestModel(290, GAME_REQUEST);
			CStreaming::RequestModel(291, GAME_REQUEST);
			CStreaming::RequestModel(294, GAME_REQUEST);
			CStreaming::LoadAllRequestedModels(0, 0);
	}

	// === CHUD ===
	void Functions::ShowHelpMessage(wchar_t* text, bool quickmessage, bool forever, bool showinbrief)
	{
		CHud::SetHelpMessage(text, quickmessage, forever, showinbrief);  
	}

	// === CPED ===
	void Functions::SpawnPed(unsigned int modelindex, CVector coords, ePedType pedtype)
	{
		CPed *ped = CPopulation::AddPed(pedtype, modelindex, coords, 0);
		CWorld::Add(ped);
	}
	void Functions::CreateCoupleOfPeds(ePedType PedType, unsigned int Model, ePedType PedType2, unsigned int Model2, CVector Position)
	{
		CPopulation::PlaceCouple(PedType, Model, PedType2, Model2, Position);
	}
	void Functions::GivePedWeapon(CPed * Ped, eWeaponType WepType, unsigned int Ammo)
	{
		CPlayerPed*player{ (CPlayerPed*)Ped };
		player->GiveWeapon(WepType, Ammo, false);
		player->SetCurrentWeapon(WepType);
	}
	void Functions::KillPed(CPed * Ped)
	{
		CPlayerPed*player{ (CPlayerPed*)Ped };
		player->m_fHealth = 0.0;
	}
	void Functions::TeleportPed(CPed * Ped, float PosX, float PosY, float PosZ)
	{
		CPlayerPed*player{ (CPlayerPed*)Ped };
		player->Teleport(CVector(PosX, PosY, PosZ));
	}
	void Functions::PedSetAimAtEntity(CPed * Ped, CEntity* Entity)
	{
		CPlayerPed*player{ (CPlayerPed*)Ped };
		player->m_pPointGunAt = Entity;
		player->AimGun();
	}
	void Functions::SetPedHealth(CPed * Ped, float Amount)
	{
		CPlayerPed*player{ (CPlayerPed*)Ped };
		player->m_fHealth = Amount;
	}
	void Functions::SetPedArmour(CPed * Ped, float Amount)
	{
		CPlayerPed*player{ (CPlayerPed*)Ped };
		player->m_fArmour = Amount;
	}
}




