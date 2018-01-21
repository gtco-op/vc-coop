/*
	Vice City CO-OP Main ASI
	Author(s) Zeyad Ahmed
			  Gamelaster
	Copyrights (c) 2017-2018 VC:CO-OP Team
*/
#define LIBRG_DEBUG
#define LIBRG_IMPLEMENTATION
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
librg_entity_id player;


using namespace plugin;
void on_connect_request(librg_event_t *event) {
	librg_log("[CLIENT] Requesting to connect");
}
void on_connect_accepted(librg_event_t *event){
	librg_log("[CLIENT] Connection Accepted");
	
}
void on_connect_refused(librg_event_t *event) {
	librg_log("[CLIENT] Connection Refused");
}
void on_entity_create(librg_event_t * event) {
	player = event->entity->id;

}
class vccoop {
public:
	
    vccoop() {  //When asi attaches
		static HWND hwnd = GetForegroundWindow();
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
		//freopen("CONIN$", "r", stdin);
		if (hwnd == NULL)
		{
			printf("HWND not working :c\n");
		}
		static int keyPressTime = 0;
		static bool isInitialized = false;
		Events::drawingEvent += [] { //drawing event loop
			if (RwD3D9Supported())
			{
				if (isInitialized == false)
				{
					isInitialized = true;
					printf("Initializing ImGUI\n");
					LPDIRECT3DDEVICE9 d3d9Device = (LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice();
					ImGui_ImplDX9_Init(hwnd, d3d9Device);
					ImGui::StyleColorsClassic();
				}
				else
				{
					

					ImGui_ImplDX9_NewFrame();
					
					ImGui::Text("Welcome to Vice City CO-OP \n That is freaking alpha version");
					char buf[256] = "192.168.1.2";
					ImGui::InputText("IP", buf, 256);
					int portval = 8080;
					int* portvalptr = &portval;
					ImGui::InputInt("Port", portvalptr);
					if (ImGui::Button("Connect")) {
						// connect to the server by using buf as ip and portvalptr as port


					}
					if (ImGui::Button("About VC:CO-OP")) {
						// lazy to do
					}
					ImGui::EndFrame();
					ImGui::Render();
				}
			}

		};
		Events::gameProcessEvent += [] {  // game process event loop

				
		};
    }
} myPlugin;