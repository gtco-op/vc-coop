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
    MyPlugin() {


    }
} myPlugin;