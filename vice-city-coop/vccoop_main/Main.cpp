/*
	Vice City CO-OP Main ASI
	Author(s) Zeyad Ahmed
	Copyrights (c) 2017-2018 VC:CO-OP Team
*/
#include "plugin_vc.h"
#include "includes.h"

using namespace plugin;
class vccoop {
public:
    vccoop() {
		
		Events::gameProcessEvent += [] { 
		
		};
    }
} myPlugin;