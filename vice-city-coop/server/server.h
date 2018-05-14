#pragma once
#include "..\\versioninfo.h"

#if defined( _MSC_VER )
#pragma warning (disable : 4244)
#pragma warning (disable : 4348)
#pragma warning (disable : 4060)
#pragma warning (disable : 4018)
#pragma warning (disable : 4267)

// librg
#include "vendor\\librg\\librg.h"

// inih
#include "vendor\\inih\\INIReader.h"

// AngelScript
#include <angelscript.h>
#include <scriptstdstring/scriptstdstring.h>
#include <scriptbuilder/scriptbuilder.h>

// Lua
extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

// TinyXML
#include "vendor\\tinyxml2-6.2.0\\tinyxml2.h"

// Mongoose
#include "mongoose.h"

#include <Windows.h>
#include <experimental/filesystem>
#include <filesystem>
#include <sstream>
#include <iterator>

// Plugin-SDK
#include "game_vc/CPad.h"
#include "game_vc/CVector.h"
#include "game_vc/CVector2D.h"
#include "game_vc/CPathNode.h"
#include "game_vc/CColPoint.h"

#else
#define HAS_SOCKLEN_T 1

#include <pthread.h>
#include <experimental/filesystem>
	
// librg
#include "./vendor/librg/librg.h"

// inih
#include "./vendor/inih/INIReader.h"

// Lua
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

// AngelScript
#include "angelscript.h"

#endif

// Standard includes
#include <iostream>
#include <ostream>
#include <fstream>
#include <stdlib.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>

// VCCOOP
#include "../vccoop_main/config.h"

#if !defined(VCCOOP_VERBOSE_LOG) && defined(VCCOOP_DEBUG)
	#define VCCOOP_VERBOSE_LOG
#else
	#undef VCCOOP_VERBOSE_LOG
#endif

#ifndef _DEBUG
	#undef VCCOOP_DEBUG_ENGINE
	#undef VCCOOP_VERBOSE_LOG
#endif 


#include "CModelIDs.h"
#include "CConfiguration.h"

#include "CLogger.h"
#include "CServerNetwork.h"

#include "CCustomData.h"
#include "CCustomDataManager.h"

#include "CLuaScript.h"
#include "CLuaDumper.h"

#include "CWebManagementServer.h"

#include "CAngelScriptInterface.h"

//------------------------------------------------------------------------------------------------

extern CConfiguration		*gConfig;
extern CLogger				*gLog;
extern CServerNetwork		*gServerNetwork;

extern CCustomDataManager	*gDataMgr;

extern CWebManagementServer *gWebServer;

extern std::string			ServerGamemodePath;
extern CLuaScript			*gGamemodeScript;

extern int					gActivePlayers;

using namespace std;