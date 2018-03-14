#pragma once

#if defined( _MSC_VER )
#pragma warning (disable : 4244)
#pragma warning (disable : 4348)
#pragma warning (disable : 4060)
#pragma warning (disable : 4018)
#endif

// librg
#include "vendor\librg\librg.h"

// inih
#include "vendor\inih\INIReader.h"

// Lua
extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

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
#include <filesystem>
#include <experimental/filesystem>

// Plugin-SDK
/*#include "plugin_vc.h"
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
#include "game_vc\CMatrix.h"*/
#include "game_vc\CPad.h"
#include "game_vc\CVector.h"
#include "game_vc\CVector2D.h"
#include "game_vc\CPathNode.h"
#include "game_vc\CColPoint.h"

// VCCOOP
#include "..\vccoop_main\config.h"
#include "..\vccoop_main\CModelIDs.h"
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