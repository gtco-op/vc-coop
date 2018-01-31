#include "main.h"

CNetwork *gNetwork;
CGame *gGame;
CRender *gRender;
CLogger *gLog;

class VCCoop 
{
public:
	VCCoop() 
	{
#ifdef VCCOOP_DEBUG
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
#endif

		gLog = new CLogger();
		gLog->Log("[CCore] Initializing classes.\n");

		gNetwork = new CNetwork();
		gGame = new CGame();
		
		gRender = new CRender();

		gLog->Log("[CCore] Main method finished.\n");

		Events::gameProcessEvent += [] {
			gGame->Run();
			gRender->Run();
		};
	}
} vcCoop;