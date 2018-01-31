#include "main.h"

void Hook_CRunningScript__Process();

static bool scriptProcessed = false;
WNDPROC		orig_wndproc;
HWND		orig_wnd;

CGame::CGame()
{
	this->InitPreGamePatches();

	gLog->Log("[CGame] CGame initialized.\n");
}
CGame::~CGame()
{
	gLog->Log("[CGame] CGame shutting down.\n");
}

void CGame::InitPreGamePatches()
{
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

	// Skip splash screen when entering another city
	//MakeRet(0x4A6E80);
	MakeNop(0x48DD7E, 5);
	MakeNop(0x40E157, 5);

	MakeNop(0x62A322, 36);//Dont pause the game when checking for collisions
	MakeNop(0x62A34E, 5);//Dont resume because its not paused


	// Hook script process (so we can spawn a local player)
	MakeCall(0x450245, Hook_CRunningScript__Process);

	//Set fps limit
	MemWrite(0x602D68, 500);
	
	//TODO: fix mouse bug
	// Disable re-initialization of DirectInput mouse device by the game
	//MemWrite<BYTE>(0x49908B, 0xEB);
	//MemWrite<BYTE>(0x498F92, 0xEB);
	//MemWrite<BYTE>(0x499014, 0xEB);

	// Make sure DirectInput mouse device is set non-exclusive (may not be needed?)
	//MemWrite<DWORD>(0x5FFA20, 0x909000B0);

	//MemCpy((void *)0x5EFFE0, "\xC2\x18\x00\x90", 4);
	gLog->Log("[CGame] InitPreGamePatches() finished.\n");
}

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
		FindPlayerPed()->Teleport({ VCCOOP_DEFAULT_SPAWN_POSITION });

		// CStreaming::LoadScene
		CVector scenePosition(VCCOOP_DEFAULT_SPAWN_POSITION);
		Call(0x40AF60, &scenePosition);

		// First tick processed
		scriptProcessed = true;
		
		gLog->Log("[CGame] CRunningScript::Process() hook finished.\n");
	}
}
LRESULT CALLBACK wnd_proc(HWND wnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	ImGuiIO & io = ImGui::GetIO();
	
	switch (umsg)
	{
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			int vkey = (int)wparam;
			if (vkey == VK_F8)
			{
				gRender->ToggleGUI();
				gLog->Log("[CGame] F8 pressed.\n");
			}
			else if (vkey == VK_F9)
			{
				FindPlayerPed()->Teleport({ VCCOOP_DEFAULT_SPAWN_POSITION });
				gLog->Log("[CGame] Teleporting to %.f, %.f, %.f.\n", VCCOOP_DEFAULT_SPAWN_POSITION);
			}
			break;
		}
	}

	if (gRender->bGUI)
	{
		gRender->device->ShowCursor(TRUE);		
		io.MouseDrawCursor = TRUE;
		
		MakeRet(0x5FF290);
		
		ImGui_ImplWin32_WndProcHandler(wnd, umsg, wparam, lparam);
		return DefWindowProc(wnd, umsg, wparam, lparam);
	}
	else
	{
		MemCpy((void*)0x5FF290, "\xA1\xF4\x48\x9B\x00", 5);

		return CallWindowProc(orig_wndproc, wnd, umsg, wparam, lparam);
	}

	return 0;
}
