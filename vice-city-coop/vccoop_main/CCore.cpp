#include "main.h"

CClientNetwork	*gNetwork;
CConfiguration  *gConfig;
CGame			*gGame;
CRender			*gRender;
CLogger			*gLog;
CChat			*gChat;
#ifdef VCCOOP_DEBUG_ENGINE
CDebugEngine	*gDbgEngine;
CLogger			*gDbgLog;
#endif

CCore::CCore()
{
#ifdef VCCOOP_DEBUG
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
#endif
	srand((int)time(NULL));

#ifdef VCCOOP_DEBUG_ENGINE
	gDbgEngine = new CDebugEngine();
	gDbgLog = new CLogger(true);
#endif
	
	gChat		= new CChat();
	gLog		= new CLogger();
	gLog->Log("[CCore] %s %s loaded.\n", VCCOOP_NAME, VCCOOP_VER);
	gLog->Log("[CCore] Initializing classes.\n");
	gConfig		= new CConfiguration();
	gNetwork	= new CClientNetwork();
	gGame		= new CGame();
	gRender		= new CRender();
	gRender->gGuiContainer.push_back(gChat);

#ifdef VCCOOP_DEBUG_ENGINE
	gRender->gGuiContainer.push_back(gDbgEngine);
#endif

	gLog->Log("[CCore] Main method finished.\n");

	Events::gameProcessEvent += [] {
		gGame->Run();
		gRender->Run();
	};
}
CCore::~CCore()
{
	delete gRender;
	gLog->Log("[CCore] CCore shutting down\n");
	delete gLog;
}
