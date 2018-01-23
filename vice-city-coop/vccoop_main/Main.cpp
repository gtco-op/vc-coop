/*
Vice City CO-OP Main ASI
Author(s) Zeyad Ahmed
Gamelaster
Copyrights (c) 2017-2018 VC:CO-OP Team
*/
#define SERVER_SECRET 12345
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



using namespace plugin;
void on_connect_request(librg_event_t *event) {
	printf("[CLIENT] Requesting to connect \n");
	
}
void on_connect_accepted(librg_event_t *event) {
	printf("[CLIENT] Connection Accepted");
}
void on_connect_refused(librg_event_t *event) {
	printf("[CLIENT] Connection Refused");
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
			if (KeyPressed(VK_F9)) {
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
		};
	}
} myPlugin;
