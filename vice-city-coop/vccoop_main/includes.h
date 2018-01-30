#ifndef _INCLUDES_H_DEFINED
#define _INCLUDES_H_DEFINED
#include "VCCoopConfig.h"

//####################################################################################################################################################
//																		DEPENDENCIES
//####################################################################################################################################################
#include "CLogger.h"
#include "CGUI.h"

#include "CNetworking.h"

//##################################
//             IMGUI
//##################################
#include "imgui\imgui.h"
#include "imgui\imgui_impl_dx9.h"

//##################################
//           PLUGIN-SDK
//##################################
#include "plugin_vc.h"
#include "game_vc\CFont.h"
#include "game_vc\CPed.h"
#include "game_vc\CHud.h"
#include "game_vc\CRGBA.h"
#include "game_vc\CHud.h"
#include "game_vc\CPopulation.h"
#include "game_vc\CWorld.h"
#include "game_vc\ePedType.h"
#include "game_vc\CCivilianPed.h"
#include "game_vc\CPad.h"
#include "extensions\ScriptCommands.h"
#include "game_vc\CStreaming.h"
#include "game_vc\CTimer.h"
#include "game_vc\CWorld.h"
#include "game_vc\common.h"

//##################################
//             STD
//##################################
#include <Windows.h>
#include <vector>
#include <cassert>
#include <assert.h>
#include <regex>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ostream>
#include <fstream>
#include <vector>

//##################################
//             D3D
//##################################
#include <d3d9.h>
#include "rw/rwd3d9.h"

//####################################################################################################################################################
//																		CLASSES
//####################################################################################################################################################

struct PlayerInfo {
	float Health;
	float Armour;
	CVector position;
	int iModelIndex;
	float Rotation;
	int iInteriorID;
	int iCurrentAnimID;
	eWeaponType WeaponType;
	unsigned int Ammo;
};

#endif