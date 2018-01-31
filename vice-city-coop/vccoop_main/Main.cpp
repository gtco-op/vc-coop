#include "main.h"

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

		gGame = new CGame();
		
		gRender = new CRender();
		gRender->InitFont();

		gLog->Log("[CCore] Main method finished.\n");
	}
} vcCoop;