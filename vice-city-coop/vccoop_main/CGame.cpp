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
void CGame::Run()
{

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

	gLog->Log("[CGame] InitPreGamePatches() finished.\n");
}
void CGame::EnableMouseInput()
{
	//Enable CPad:UpdateMouse
	MemCpy((void*)0x4AD820, "\x53", 1);
	//Enable camera movement update
	MemCpy((void*)0x48351A, "\xD9\x05\xD8\xAD\x68\x00", 6);
	//CControllerConfigManager::AffectPadFromKeyBoard restore
	MemCpy((void*)0x4AB6E6, "\xE8\x45\xCE\x16\x00", 5);
	//CControllerConfigManager::AffectPadFromMouse restore
	MemCpy((void*)0x4AB6F0, "\xE8\x9B\xCD\x16\x00", 5);
}
void CGame::DisableMouseInput()
{
	//Disable CPad:UpdateMouse
	MakeRet(0x4AD820);
	//Disable camera movement update
	MakeRet(0x48351A);
	//CControllerConfigManager::AffectPadFromKeyBoard nop
	MakeNop(0x4AB6E6, 5);
	//CControllerConfigManager::AffectPadFromMouse nop
	MakeNop(0x4AB6F0, 5);
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

			if (vkey == VK_F7 && gNetwork->client_connected)
			{
				gNetwork->StopClientThread();
				gLog->Log("[CGame] Disconnecting from server.\n");
			}
			else if (vkey == VK_F8)
			{
				gRender->ToggleGUI();
				gLog->Log("[CGame] Toggling GUI.\n");
			}
			else if (vkey == VK_F9)
			{
				FindPlayerPed()->Teleport({ VCCOOP_DEFAULT_SPAWN_POSITION });
				gLog->Log("[CGame] Teleporting to X: %.f Y: %.f Z: %.f\n", VCCOOP_DEFAULT_SPAWN_POSITION);
			}
			else if (vkey == VK_F10)
			{
				gNetwork->AttemptConnect("127.0.0.1", VCCOOP_DEFAULT_SERVER_PORT);
				gLog->Log("[CGame] Attempting to connect to local server\n");
			}
			break;
		}
	}

	io.MouseDrawCursor = gRender->bGUI;
	if(gRender->device != nullptr)
		gRender->device->ShowCursor(gRender->bGUI);

	if (gRender->bGUI)
	{
		gGame->DisableMouseInput();
		ImGui_ImplWin32_WndProcHandler(wnd, umsg, wparam, lparam);
		return DefWindowProc(wnd, umsg, wparam, lparam);
	}
	else
	{
		gGame->EnableMouseInput();
		return CallWindowProc(orig_wndproc, wnd, umsg, wparam, lparam);
	}
}