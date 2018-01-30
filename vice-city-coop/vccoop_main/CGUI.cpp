#pragma message("Compiling GUI class")
#include "CGUI.h"

bool CGUI::DisableGameInput = false;
CGUI* CGUI::ptr = nullptr;

CGUI::CGUI(CLogger* Log, void* Network)
{
	l = Log;

	while (FindWindow(0, VC_TITLE) == 0) { Sleep(1000); }

	l->Log("[CGUI] Initialized\n");

	ptr = this;
}
CGUI::~CGUI()
{

}
void CGUI::Shutdown()
{
	ImGui_ImplDX9_Shutdown();
}
void CGUI::SetVCHandle()
{
	if(ptr!=nullptr)
		ptr->tWindow = FindWindow(NULL, VC_TITLE);
}
void CGUI::DrawEvent()
{
	if (RwD3D9Supported())
	{
		if (bGUI)
		{
			if (!hasInitialized)
			{
				if (!onceInitHook) {
					SetVCHandle(); onceInitHook = true;
					l->Log("[CGUI] Constructing ImGUI\n");
				}
				else {
					l->Log("[CGUI] Reconstructing ImGUI\n");
				}

				LPDIRECT3DDEVICE9 d3d9Device = (LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice();
				ImGui_ImplDX9_Init(tWindow, d3d9Device);
				ImGui::StyleColorsClassic();
				hasInitialized = true;
			}
			else
			{
				ImGui_ImplDX9_NewFrame();

				ImGui::Begin("Vice City CO-OP");
				ImGui::Text("Welcome to Vice City CO-OP\nThis is freaking alpha version");
				ImGui::InputText("IP", "127.0.0.1", 256);
				int portval = 23546;
				int* portvalptr = &portval;
				ImGui::InputInt("Port", portvalptr);

				if (ImGui::Button("Connect")) {
					l->Log("[CGUI] Connect button clicked..\n");
					//AttemptConnect();

					bGUI = false;
					hasInitialized = false;
					DisableGameInput = false;
				}
				if (ImGui::Button("About VC:CO-OP")) {
					l->Log("[CGUI] About button clicked..\n");
				}
				ImGui::End();

				ImGui::EndFrame();


				ImGui::Render();
			}
		}
	}
}
void CGUI::ToggleInput(bool Enabled)
{
	DisableGameInput = Enabled;
}
void CGUI::ToggleGUI()
{
	DisableGameInput = !DisableGameInput;

	if (DisableGameInput)
	{
		bGUI = true;
		hasInitialized = false;

		l->Log("[CGUI] Creating CGUI instance\n");
	}
	else
	{
		bGUI = false;
		hasInitialized = false;

		l->Log("[CGUI] Destroying CGUI instance\n");
		ImGui_ImplDX9_Shutdown();
	}
}