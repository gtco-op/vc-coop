#pragma once

//librg
#include "librg\librg.h"

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
#include "rw\rwd3d9.h"
#include "hooklibrary.h"
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

#define MAX_PLAYERS 500

#pragma pack(1)
typedef struct _CAMERA_AIM
{
	float f1x, f1y, f1z;
	float pos1x, pos1y, pos1z;
	float pos2x, pos2y, pos2z;
	float f2x, f2y, f2z;

} CAMERA_AIM;

#pragma pack(1)
typedef struct _GTA_CONTROLSET
{
	DWORD dwFrontPad;
	WORD wKeys1[19];
	DWORD dwFrontPad2;
	WORD wKeys2[19];
	WORD wTurnLeftRightAnalog[10];
	BYTE bytePadding1[138];
	BYTE byteCrouchAnalog[5];
} GTA_CONTROLSET;


//vccoop
#include "config.h"

#include "CClientNetwork.h"
#include "CLogger.h"
#include "CGame.h"
#include "CRenderTemplate.h"
#include "CRender.h"
#include "CNameTags.h"
#include "CDebugScreen.h"
#include "CImGui.h"
#include "CChat.h"

#include "imgui\imgui.h"
#include "imgui\imgui_impl_dx9.h"

#define LocalPlayer FindPlayerPed

using namespace std;
using namespace plugin;
using namespace Hook;

extern CClientNetwork	*gNetwork;
extern CGame			*gGame;
extern CRender			*gRender;
extern CLogger			*gLog;
extern CChat			*gChat;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);