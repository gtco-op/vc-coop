#include "main.h"

CClientNetwork	*gNetwork;
CConfiguration  *gConfig;
CGame			*gGame;
CRender			*gRender;
CImGui			*gGUI;
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
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
#endif
	srand((int)time(NULL));

#ifdef VCCOOP_DEBUG_ENGINE
	gDbgEngine	= new CDebugEngine();
	gDbgLog		= new CLogger(true);
#endif
	
	gChat		= new CChat();
	gLog		= new CLogger();
	gLog->Log("[CCore] " VCCOOP_NAME " " VCCOOP_VER " loaded.\n");
	gLog->Log("[CCore] Build: %s\n", GIT_BUILD);

	gConfig		= new CConfiguration();
	gNetwork	= new CClientNetwork();
	gGame		= new CGame();
	gRender		= new CRender();
	gRender->gGuiContainer.push_back(gChat);

	gGUI = new CImGui();
	gRender->gGuiContainer.push_back(gGUI);

#ifdef VCCOOP_DEBUG_ENGINE
	gRender->gGuiContainer.push_back(gDbgEngine);
#endif

	gLog->Log("[CCore] Main method finished.\n");

	Events::gameProcessEvent.Add(CGame::HasGameLoaded, PRIORITY_AFTER);
	Events::gameProcessEvent += [] {
		gGame->Run();
		gRender->Run();
	};
}
CCore::~CCore()
{
	gLog->Log("[CCore] CCore shutting down.\n");
	gNetwork->StopThread();
	gNetwork->StopClientThread();
}
