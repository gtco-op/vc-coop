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
		static int keyPressTime = 0;

#ifdef VCCOOP_DEBUG
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
#endif
		gChat = new CChat();
		gLog = new CLogger();

		gLog->Log("[CCore] %s %s loaded.\n", VCCOOP_NAME, VCCOOP_VER);
		gLog->Log("[CCore] Initializing classes.\n");
		gConfig = new CConfiguration();
		gNetwork = new CClientNetwork();
		gGame = new CGame();
		gRender = new CRender();
		gRender->gGuiContainer.push_back(gChat);

		gLog->Log("[CCore] Main method finished.\n");

		Events::gameProcessEvent += [] {
#ifdef VCCOOP_DEBUG
			if (KeyPressed(223) && CTimer::m_snTimeInMilliseconds - keyPressTime > 500)
			{
				keyPressTime = CTimer::m_snTimeInMilliseconds;
				gRender->bConsole = !gRender->bConsole;
				ImGui::GetIO().ClearInputCharacters();
			}
#endif
			if (KeyPressed('T') && CTimer::m_snTimeInMilliseconds - keyPressTime > 500 && !gChat->chatToggled)
			{
				keyPressTime = CTimer::m_snTimeInMilliseconds;
				if (!gChat->chatToggled && !gRender->bGUI && !gRender->bConnecting && !gRender->bAboutWindow)
					gChat->ToggleChat(true); ImGui::GetIO().ClearInputCharacters();
			}
			gGame->Run();
			gRender->Run();
		};
	}
	~VCCoop()
	{
		delete gRender;

		gLog->Log("[CCore] CCore shutting down\n");

		delete gLog;
	}
} vcCoop;