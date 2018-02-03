#pragma once
#include "..\vccoop_main\config.h"

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

#include "CLogger.h"
#include "CServerNetwork.h"

extern CLogger			*gLog;
extern CServerNetwork	*gServerNetwork;

using namespace std;