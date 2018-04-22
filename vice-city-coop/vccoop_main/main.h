#pragma once
#include "..\\versioninfo.h"

//librg
#include "vendor\librg\librg.h"

//imgui
#include "vendor\imgui\imgui.h"
#include "vendor\imgui\imgui_impl_dx9.h"

//inih
#include "vendor\inih\INIReader.h"

//hooks
#include "hooklibrary.h"

// discord rich presence
#include "vendor\discord-rich-presence\discord-rpc.h"
//lua
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

// TinyXML
#include "..\server\vendor\tinyxml2-6.2.0\tinyxml2.h"

// libcurl
#include "..\vendor\curl-7.59.0\include\curl\curl.h"

//common
#include <Windows.h> 
#include <tchar.h>           
#include <shellapi.h>
#include <fstream>
#include <iostream>
#include <cstdarg>
#include <string>
#include <ctime>
#include <time.h>
#include <vector>
#include <map>
#include <utility>
#include <iterator>
#include <experimental/filesystem>

//detours
#include "detours.h"

//plugin sdk
#include "plugin_vc.h"
#include "RenderWare.h"
#include "d3dx9.h"
#include "vendor\rw\rwd3d9.h"
#include "common.h"

#include "eWeaponFire.h"

#include "CSprite.h"
#include "CSprite2d.h"
#include "CCamera.h"
#include "CWorld.h"
#include "CStreaming.h"
#include "CCivilianPed.h"
#include "CPlayerInfo.h"
#include "CPopulation.h"
#include "CModelInfo.h"
#include "CHeli.h"
#include "CPlane.h"
#include "CBoat.h"
#include "CWorld.h"
#include "CTheScripts.h"
#include "CTimer.h"
#include "CTheZones.h"
#include "extensions\ScriptCommands.h"
#include "CMenuManager.h"
#include "CPathFind.h"
#include "CModelInfo.h"
#include "CBike.h"
#include "CHud.h"
#include "CMessages.h"
#include "CPlayerPed.h"
#include "CPools.h"
#include "CPool.h"
#include "CEntity.h"

//vccoop
#include "config.h"

#if !defined(VCCOOP_VERBOSE_LOG) && defined(VCCOOP_DEBUG)
	#define VCCOOP_VERBOSE_LOG
#else
	#undef VCCOOP_VERBOSE_LOG
#endif

#if !defined(_DEBUG) || !defined(VCCOOP_DEBUG)
	#undef VCCOOP_DEBUG_ENGINE
	#undef VCCOOP_VERBOSE_LOG
#endif 

#define GetKey(a)		(((GetAsyncKeyState(a)&0x8000)) && IsWindowActive() && (CTimer::m_snTimeInMilliseconds - keyPressTime >= 500))
#define GetTwoKeys(a,b) ((GetAsyncKeyState(a)&0x8000) && (GetAsyncKeyState(b)&0x8000) && IsWindowActive() && (CTimer::m_snTimeInMilliseconds - keyPressTime >= 500))

//keys
#define	KEY_INCAR_TURRETLR			0
#define	KEY_INCAR_TURRETUD			1
#define	KEY_INCAR_RADIO				2
#define	KEY_INCAR_LOOKL				3
#define	KEY_INCAR_HANDBRAKE			4
#define	KEY_INCAR_LOOKR				5
#define	KEY_INCAR_TURNL				8
#define	KEY_INCAR_TURNR				9
#define	KEY_INCAR_CAMERA			11
#define	KEY_INCAR_BACKWARD			12
#define	KEY_INCAR_EXITVEHICLE		13
#define	KEY_INCAR_FORWARD			14
#define	KEY_INCAR_FIRE				15
#define	KEY_INCAR_HORN				16
#define	KEY_ONFOOT_TURNLR			0
#define	KEY_ONFOOT_ACTION			2
#define	KEY_ONFOOT_NEXTWEAPON		3
#define	KEY_ONFOOT_TARGET			4
#define	KEY_ONFOOT_PREVWEAPON		5
#define	KEY_ONFOOT_FORWARD			6
#define	KEY_ONFOOT_BACKWARD			7
#define	KEY_ONFOOT_LEFT				8
#define	KEY_ONFOOT_RIGHT			9
#define	KEY_ONFOOT_JUMP				12
#define	KEY_ONFOOT_ENTERVEHICLE		13
#define	KEY_ONFOOT_SPRINT			14
#define	KEY_ONFOOT_FIRE				15
#define	KEY_ONFOOT_CROUCH			16
#define	KEY_ONFOOT_LOOKBEHIND		17

#include "CLogger.h"
#include "CConfiguration.h"

#include "CModelIDs.h"

#include "CHooks.h"
#include "CClientEntity.h"
#include "CClientPlayer.h"
#include "CClientVehicle.h"
#include "CClientPed.h"

#include "CRichPresence.h"
#include "CClientNetwork.h"

#include "CRenderTemplate.h"
#include "CNameTags.h"
#include "CImGui.h"
#include "CChat.h"
#include "CGame.h"
#include "CCameraStuff.h"

extern CGame			*gGame;

#ifdef VCCOOP_DEBUG
#include "CDebugScreen.h"
#include "CDebugEngine.h"
#endif

#include "CRender.h"

#include "CCustomData.h"

#include "CLua.h"

#define SAFE_RELEASE(p)          { if (p) { (p)->Release(); (p)=NULL; } }
#define LocalPlayer				 gGame->FindLocalPed

using namespace std;
using namespace plugin;
using namespace Hook;

extern WNDPROC		orig_wndproc;
extern HWND			orig_wnd;

extern CClientNetwork	*gNetwork;
extern CConfiguration	*gConfig;
extern CRender			*gRender;
extern CImGui			*gGUI;
extern CLogger			*gLog;
extern CChat			*gChat;

#ifdef VCCOOP_DEBUG_ENGINE
extern CDebugEngine		*gDbgEngine;
extern CLogger			*gDbgLog;
#endif

extern CPad localPlayerKeys;

extern sStartParams gStartParams;

extern std::vector<serverInfo> serverList;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

#include "CCore.h"