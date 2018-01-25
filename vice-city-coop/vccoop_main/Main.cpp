/*
Vice City CO-OP Main ASI
Author(s) Zeyad Ahmed
Gamelaster
LemonHaze420
Copyrights (c) 2017-2018 VC:CO-OP Team
*/

#include "VCCoopConfig.h"

//librg
#include "librg/librg.h"

// plugin sdk
#include "plugin_vc.h"
#include "includes.h"
// imgui / directx
#include "imgui\imgui.h"
#include "imgui\imgui_impl_dx9.h"
#include <d3d9.h>
#include "rw/rwd3d9.h"
#include "Logger.h"

using namespace plugin;
using namespace Logger;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall HookedWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

WNDPROC OldWndProc = nullptr;
HWND tWindow = nullptr;
bool DisableGameInput = true, bGUI = true, hasInitialized = false, onceInitHook = false;

void Initialize()
{
	tWindow = FindWindow(0, VC_TITLE);
	if (tWindow) OldWndProc = (WNDPROC)SetWindowLongPtr(tWindow, GWLP_WNDPROC, (LONG_PTR)HookedWndProc);
	Log("[VC CO-OP][ImGui] WndProc hooked!\n");
}
void Set()
{
	SetWindowLongPtr(tWindow, GWLP_WNDPROC, (LONG_PTR)HookedWndProc);
	Log("[VC CO-OP][ImGui] WndProc set!\n");
}
void Restore()
{
	SetWindowLongPtr(tWindow, GWLP_WNDPROC, (LONG_PTR)OldWndProc);
	Log("[VC CO-OP][ImGui] WndProc restored!\n");
}
LRESULT __stdcall HookedWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ImGuiIO& io = ImGui::GetIO();

	io.MouseDrawCursor = true;
	if (DisableGameInput)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return FALSE;
}
void windowThread()
{
	while (FindWindow(NULL, VC_TITLE) == 0) { Sleep(100); }
	while (reinterpret_cast<IDirect3DDevice9*>(RwD3D9GetCurrentD3DDevice()) == NULL) { Sleep(100); }

	tWindow = FindWindow(NULL, VC_TITLE);
	Log("[VC CO-OP][ImGui] VC window found!\n");
}
void on_connect_request(librg_event_t *event) {
	Log("[VC CO-OP][CLIENT] Requesting to connect \n");
}
void on_connect_accepted(librg_event_t *event) {
	Log("[VC CO-OP][CLIENT] Connection Accepted");
}
void on_connect_refused(librg_event_t *event) {
	Log("[VC CO-OP][CLIENT] Connection Refused");
}
void on_entity_create(librg_event_t * event) {
	int entity_id = event->entity->id;
	int entity_type = event->entity->type;
	zplm_vec3_t position = event->entity->position;
	CStreaming::RequestModel(0, 0);
	CPed *ped = new CCivilianPed(ePedType::PEDTYPE_CIVMALE, 0);
	CWorld::Add(ped);
	CVector posn = CVector::CVector(position.x, position.y, position.z);
	ped->Teleport(posn);

}
void on_entity_update(librg_event_t *event) {
	int entity_id = event->entity->id;

}
class vccoop {
public:

	vccoop() {  //When asi attaches
#ifdef VCCOOP_DEBUG
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
		//freopen("CONIN$", "r", stdin);
#endif

		static int keyPressTimeConnect = 0;
		static int keyPressTimeDrawGUI = 0;

		Log("[VC CO-OP] %s %s loaded.\n", VCCOOP_NAME, VCCOOP_VER);
		Log("[VC CO-OP] Executable Directory: %s\n", GetExecutablePath().append("\\"));

		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&windowThread, NULL, 0, NULL);

		Events::drawingEvent += [] { //drawing event loop
			if (RwD3D9Supported())
			{
				if (bGUI)
				{
					if (!hasInitialized)
					{
						if (!onceInitHook) {
							Initialize(); onceInitHook = true;
							Log("[VC CO-OP][ImGui] Constructing ImGUI\n");
						}
						else {
							Set();
							Log("[VC CO-OP][ImGui] Reconstructing ImGUI\n");
						}

						LPDIRECT3DDEVICE9 d3d9Device = (LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice();
						ImGui_ImplDX9_Init(tWindow, d3d9Device);
						ImGui::StyleColorsClassic();
						hasInitialized = true;
					}
					else
					{
						ImGui_ImplDX9_NewFrame();

						ImGui::Text("Welcome to Vice City CO-OP\n This is freaking alpha version");
						char buf[256] = "192.168.1.2";
						ImGui::InputText("IP", buf, 256);
						int portval = 8080;
						int* portvalptr = &portval;
						ImGui::InputInt("Port", portvalptr);
						
						if (ImGui::Button("Connect")) {
							Log("[VC CO-OP][ImGui] Connect button clicked..\n");
						}
						if (ImGui::Button("About VC:CO-OP")) {
							Log("[VC CO-OP][ImGui] About button clicked..\n");
						}

						ImGui::EndFrame();


						ImGui::Render();
					}
				}
			}

		};
		Events::gameProcessEvent += [] {  // game process event loop
			if (KeyPressed(VK_F9) && CTimer::m_snTimeInMilliseconds - keyPressTimeConnect > 500) {
				keyPressTimeConnect = CTimer::m_snTimeInMilliseconds;

				HelpMessageForever("Initalizing Client.");
				librg_ctx_t ctx = { 0 };
				ctx.tick_delay = 45;
				ctx.mode = LIBRG_MODE_CLIENT;
				librg_init(&ctx);
				librg_event_add(&ctx, LIBRG_CONNECTION_REQUEST, on_connect_request);
				librg_event_add(&ctx, LIBRG_CONNECTION_ACCEPT, on_connect_accepted);
				librg_event_add(&ctx, LIBRG_ENTITY_CREATE, on_entity_create);
				HelpMessageForever("Connecting to the server \n IP: 192.168.1.2 PORT: 23546");
				librg_address_t address = { 23546, "192.168.1.2" };
				librg_network_start(&ctx, address);
				bool client_running = true;
				while (client_running) {
						librg_tick(&ctx);
						zpl_sleep_ms(1);
					}

				 //disconnection from the server
				 //and resource disposal
				librg_network_stop(&ctx);
				librg_free(&ctx);
			}
			if (KeyPressed(VK_F8) && CTimer::m_snTimeInMilliseconds - keyPressTimeDrawGUI > 1000) {
				keyPressTimeDrawGUI = CTimer::m_snTimeInMilliseconds;
				
				DisableGameInput = !DisableGameInput;

				if (DisableGameInput)
				{
					Log("[VC CO-OP][ImGui] Handing inputs to ImGui\n");
					Set();

					bGUI = true;
					hasInitialized = false;
				}
				else
				{
					Log("[VC CO-OP][ImGui] Restoring inputs to game\n");
					Restore();

					Log("[VC CO-OP][ImGui] Destroying ImGui instance\n");
					ImGui_ImplDX9_Shutdown();

					bGUI = false;
					hasInitialized = false;
				}
			}
		};
	}
	~vccoop()	{
		Log("[VC CO-OP] Shutting down\n");
	}
} vccoop;
