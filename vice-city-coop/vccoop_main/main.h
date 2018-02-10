#pragma once

//librg
#include "vendor\librg\librg.h"

//imgui
#include "vendor\imgui\imgui.h"
#include "vendor\imgui\imgui_impl_dx9.h"

//inih
#include "vendor\inih\INIReader.h"

//hooks
#include "hooklibrary.h"

//common
#include <Windows.h>
#include <fstream>
#include <iostream>
#include <cstdarg>
#include <string>
#include <ctime>
#include <time.h>
#include <vector>
#include <map>
#include <utility>

//plugin sdk
#include "plugin.h"
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

//vccoop
#include "config.h"

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


#include "CConfiguration.h"
#include "CClientNetwork.h"
#include "CLogger.h"
#include "CRenderTemplate.h"
#include "CNameTags.h"
#include "CDebugScreen.h"
#include "CImGui.h"
#include "CChat.h"
#include "CGame.h"

#include "CRender.h"
#include "CClientPlayer.h"

#define SAFE_RELEASE(p)          { if (p) { (p)->Release(); (p)=NULL; } }
#define LocalPlayer				 FindPlayerPed

using namespace std;
using namespace plugin;
using namespace Hook;

extern WNDPROC		orig_wndproc;
extern HWND			orig_wnd;

extern CClientNetwork	*gNetwork;
extern CConfiguration	*gConfig;
extern CGame			*gGame;
extern CRender			*gRender;
extern CLogger			*gLog;
extern CChat			*gChat;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

#include "CCore.h"