#pragma once
// Dependencies
#include "vendor\librg\librg.h"
#include "vendor\inih\INIReader.h"
//lua
#ifndef __cplusplus
extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#else
#include <lua.hpp>
#endif

// Standard includes
#include <Windows.h>
#include <iostream>
#include <ostream>
#include <fstream>
#include <stdlib.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>
#include <experimental/filesystem>

// Plugin-SDK
#include "plugin.h"
#include "game_vc\common.h"
#include "game_vc\eWeaponFire.h"
#include "game_vc\CSprite.h"
#include "game_vc\CSprite2d.h"
#include "game_vc\CCamera.h"
#include "game_vc\CWorld.h"
#include "game_vc\CStreaming.h"
#include "game_vc\CCivilianPed.h"
#include "game_vc\CPlayerInfo.h"
#include "game_vc\CPopulation.h"

#define VCCOOP_DEBUG

// VCCOOP
#include "..\vccoop_main\config.h"
#include "CConfiguration.h"
#include "CLogger.h"
#include "CServerNetwork.h"

#include "CCustomData.h"
#include "CCustomDataManager.h"

#include "CLuaScript.h"
#include "CLuaDumper.h"

//------------------------------------------------------------------------------------------------

extern CConfiguration		*gConfig;
extern CLogger				*gLog;
extern CServerNetwork		*gServerNetwork;

extern CCustomDataManager	*gDataMgr;

extern std::string			ServerGamemodePath;
extern CLuaScript			*gGamemodeScript;

using namespace std;