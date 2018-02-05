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