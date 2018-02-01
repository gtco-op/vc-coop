#include "main.h"

CImGui::CImGui() {}
CImGui::~CImGui() {}

void CImGui::Draw()
{
	if (gRender->bInitializedImGui && gRender->bGUI)
	{
		ImGui_ImplDX9_NewFrame();
		ImGui::Begin("Vice City CO-OP " VCCOOP_VER);
		ImGui::Text("Welcome to Vice City CO-OP " VCCOOP_VER "\nThis is freaking alpha version");

		ImGui::InputText("IP", IP, sizeof(IP));
		ImGui::InputInt("Port", &Port);

		if (ImGui::Button("Connect"))
		{
			gLog->Log("[CRender] Connect button clicked..\n");
			gRender->bGUI = false;
			gRender->bInitializedImGui = false;

			gNetwork->AttemptConnect(IP, Port);
		}
		if (ImGui::Button("About VC:CO-OP"))
		{
			gLog->Log("[CRender] About button clicked..\n");
		}
		ImGui::End();
		ImGui::EndFrame();
		ImGui::Render();
	}
}