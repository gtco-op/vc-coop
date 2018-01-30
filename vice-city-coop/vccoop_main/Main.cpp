/*
		Vice City CO-OP Main ASI

		Author(s)	Zeyad Ahmed
					Gamelaster
					LemonHaze420
					Vektor

		Special Thanks for	Plugin-SDK Community
							Librg Community
							
		Copyrights (c) 2017-2018 VC:CO-OP Team
*/
#include "CNetworking.h"
#include "Hooks.h"
using namespace plugin;
using namespace Hook;
CLogger* l;
CGUI* g;
CNetworking* n;


static bool scriptProcessed = false;
void Hook_CRunningScript__Process()
{
	if (!scriptProcessed)
	{
		// Change player model ID
		MemWrite<u8>(0x5384FA + 1, 7);

		// CPlayerPed::SetupPlayerPed
		Call(0x5383E0, 0);

		// Set player position
		//ThisCall(0x4F5690, FindPlayerPed(), );
		FindPlayerPed()->Teleport({ 0.0f, 0.0f, 10.0f });

		// CStreaming::LoadScene
		CVector scenePosition(0.0f, 0.0f, 10.0f);
		Call(0x40AF60, &scenePosition);

		// First tick processed
		scriptProcessed = true;
	}
}
class vccoop {
public:

	vccoop()	{
		//disable gamestate initialize
		MakeNop(0x601B3B, 10);

		//Set game state to loaded
		MemWrite<u32>(0x9B5F08, 5);

		//Set bStartGame to 1
		MemWrite<u8>(0x869641, 1);

		//Set bMenuVisible = 0
		MemWrite<u8>(0x869668, 0);

		//Set bGameLoaded = 1
		MemWrite<u8>(0x86969C, 1);

		// fix CPedStats::GetPesStatType crash
		Call(0x530260, 0);//initialize probably

		// Don't load the SCM Script
		MakeShortJmp(0x4506D6, 0x45070E);

		// Skip loading screen
		MakeRet(0x4A69D0);

		// Hook script process (so we can spawn a local player)
		MakeCall(0x450245, Hook_CRunningScript__Process);
#ifdef VCCOOP_DEBUG
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
#endif
		static int keyPressTimeConnect = 0;
		static int keyPressTimeDrawGUI = 0;
				
		l = new CLogger();
		g = new CGUI(l);
		n = new CNetworking(l);
				
		Events::drawingEvent += [] {
			g->DrawEvent();
		};

		Events::gameProcessEvent += [] {
			if (KeyPressed(VK_F8) && CTimer::m_snTimeInMilliseconds - keyPressTimeDrawGUI > 1000) {
				keyPressTimeDrawGUI = CTimer::m_snTimeInMilliseconds;
				
				g->ToggleGUI();
			}
			if (KeyPressed(VK_F9) && CTimer::m_snTimeInMilliseconds - keyPressTimeConnect > 1000) {
				keyPressTimeConnect = CTimer::m_snTimeInMilliseconds;

				n->AttemptConnect();
			}
		};
	}
	~vccoop()	{
		l->Log("[VC CO-OP] Shutting down\n");
	}
} vccoop;