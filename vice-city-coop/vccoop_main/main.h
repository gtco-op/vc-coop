#pragma once

//common
#include <Windows.h>
#include <fstream>
#include <iostream>
#include <cstdarg>
#include <string>
#include <ctime>
#include <time.h>

//plugin sdk
#include "plugin.h"
#include "RenderWare.h"
#include "d3dx9.h"
#include "CSprite.h"
#include "rw\rwd3d9.h"
#include "hooklibrary.h"
#include "common.h"
#include "CSprite2d.h"
#include "CCamera.h"

//vccoop
#include "config.h"

#include "CLogger.h"
#include "CGame.h"
#include "CRender.h"

#include "imgui\imgui.h"
#include "imgui\imgui_impl_dx9.h"

using namespace plugin;
using namespace Hook;

extern CGame *gGame;
extern CRender *gRender;
extern CLogger *gLog;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);