#include "CMemoryEdits.h"


static bool scriptProcessed = false;

void CMemoryEdits::Hook_CRunningScript__Process()
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
void CMemoryEdits::Patch()
{
	l->Log("[CMemoryEdits] Patching GameState.\n");

	//disable gamestate initialize
	MakeNop(0x601B3B, 10);

	l->Log("[CMemoryEdits] State set to loaded.\n");
	//Set game state to loaded
	MemWrite<u32>(0x9B5F08, 5);

	//Set bStartGame to 1
	MemWrite<u8>(0x869641, 1);

	//Set bMenuVisible = 0
	MemWrite<u8>(0x869668, 0);

	l->Log("[CMemoryEdits] Game loaded.\n");
	//Set bGameLoaded = 1
	MemWrite<u8>(0x86969C, 1);

	// fix CPedStats::GetPesStatType crash
	Call(0x530260, 0);//initialize probably

					  // Don't load the SCM Script
	MakeShortJmp(0x4506D6, 0x45070E);

	// Skip loading screen
	MakeRet(0x4A69D0);

	// Hook script process (so we can spawn a local player)
	MakeCall(0x450245, &Hook_CRunningScript__Process);

	l->Log("[CMemoryEdits] CRunningScript::Process() hooked.\n");
}