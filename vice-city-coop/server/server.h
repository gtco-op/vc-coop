#pragma once
#include "librg\librg.h"

#include <Windows.h>
#include <iostream>
#include <ostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>

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

#include "..\vccoop_main\config.h"
#include "CLogger.h"
#include "CServerNetwork.h"

extern CLogger			*gLog;
extern CServerNetwork	*gServerNetwork;

using namespace std;