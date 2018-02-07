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

#include "CConfiguration.h"
#include "CClientNetwork.h"
#include "CLogger.h"
#include "CGame.h"
#include "CRenderTemplate.h"
#include "CRender.h"
#include "CNameTags.h"
#include "CDebugScreen.h"
#include "CImGui.h"
#include "CChat.h"
#include "CClientPlayer.h"

#define LocalPlayer FindPlayerPed

using namespace std;
using namespace plugin;
using namespace Hook;

extern CClientNetwork	*gNetwork;
extern CConfiguration	*gConfig;
extern CGame			*gGame;
extern CRender			*gRender;
extern CLogger			*gLog;
extern CChat			*gChat;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);