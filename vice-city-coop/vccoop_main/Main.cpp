#include "main.h"

CClientNetwork	*gNetwork;
CConfiguration  *gConfig;
CGame			*gGame;
CRender			*gRender;
CLogger			*gLog;
CChat			*gChat;

class VCCoop 
{
public:
	VCCoop() 
	{
#ifdef VCCOOP_DEBUG
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
#endif
		gChat = new CChat();
		gLog = new CLogger();

		gLog->Log("[CCore] Initializing classes.\n");
		gConfig = new CConfiguration();
		gNetwork = new CClientNetwork();
		gGame = new CGame();
		gRender = new CRender();
		gRender->gGuiContainer.push_back(gChat);
		
		gLog->Log("[CCore] Main method finished.\n");

		Events::gameProcessEvent += [] {
			if (KeyPressed('T'))
			{
				if (!gChat->chatToggled && !gRender->bGUI && !gRender->bConnecting && !gRender->bAboutWindow)
					gChat->ToggleChat(true); ImGui::GetIO().ClearInputCharacters();
			}
			gGame->Run();
			gRender->Run();
		};
	}
} vcCoop;