#include "main.h"

CImGui::CImGui() {}
CImGui::~CImGui() {}

int currentPanel = 0;

void CImGui::Draw()
{
	if (gRender->bEscMenu)
	{
		ImGui::SetNextWindowPosCenter();
		ImGui::Begin("Vice City CO-OP " VCCOOP_VER, &gRender->bGUI, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

		if (ImGui::Button("Display options"))currentPanel = 0; ImGui::SameLine();
		if (ImGui::Button("Sound options"))currentPanel = 1; ImGui::SameLine();
		if (ImGui::Button("Keyboard options"))currentPanel = 2;
		ImGui::Separator();
		if (currentPanel == 0)
		{
			static int brigthness = FrontendMenuManager.m_PrefsBrightness;
			ImGui::SliderInt("Brightness", &brigthness, 0, 600);
			FrontendMenuManager.m_PrefsBrightness = brigthness;

			static int drawdistance = MemRead<int>(0x97F2DC);
			ImGui::SliderInt("Draw distance", &drawdistance, 0, 600);
			MemWrite<int>(0x97F2DC, drawdistance);

			static int framelimit = RsGlobal.maxFPS;
			ImGui::SliderInt("Frame limit", &framelimit, 30, 500);
			RsGlobal.maxFPS = framelimit;

			static bool framelimiter = FrontendMenuManager.m_bFrameLimiter;
			ImGui::Checkbox("Frame limiter", &framelimiter); ImGui::SameLine();
			FrontendMenuManager.m_bFrameLimiter = framelimiter;

			static bool widescreen = FrontendMenuManager.m_bWidescreen;
			ImGui::Checkbox("Wide screen", &widescreen);
			FrontendMenuManager.m_bWidescreen = widescreen;

			//gLog->Log("%d, %f", MemRead<int>(0x97F2DC), MemRead<float>(0x97F2DC));

			static int currentitem = 0;
			ImGui::Combo("Screen resolution", &currentitem, " 640x480x32\0 800x600x32\0 1024x768x32\0 1152x864x32\0 1280x600x32\0 1280x720x32\0 1280x7768x32\0 1280x800x32\0 1280x960x32\0 1280x1024x32\0 1360x768x32\0 1366x768x32\0 1400x1050x32\0 1440x900x32\0 1280x800x32\0 1600x900x32\0 1680x1050x32\0 1920x1080x32\0\0");

			/*switch (currentitem) works but eeeeew
			{
			case 0:
			{
			if (RsGlobal.screenWidth != 640 || RsGlobal.screenHeight != 480)
			{
			RsGlobal.screenWidth = 640;
			RsGlobal.screenHeight = 480;
			SetWindowPos(orig_wnd, NULL, 0, 0, RsGlobal.screenWidth, RsGlobal.screenHeight, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
			}
			break;
			}
			case 1:
			{
			if (RsGlobal.screenWidth != 800 || RsGlobal.screenHeight != 600)
			{
			RsGlobal.screenWidth = 800;
			RsGlobal.screenHeight = 600;
			SetWindowPos(orig_wnd, NULL, 0, 0, RsGlobal.screenWidth, RsGlobal.screenHeight, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
			}
			break;
			}
			case 2:
			{
			if (RsGlobal.screenWidth != 800 || RsGlobal.screenHeight != 600)
			{
			RsGlobal.screenWidth = 800;
			RsGlobal.screenHeight = 600;
			SetWindowPos(orig_wnd, NULL, 0, 0, RsGlobal.screenWidth, RsGlobal.screenHeight, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
			}
			break;
			}*/

		}
		else if (currentPanel == 1)
		{
			static int music = 0;
			ImGui::SliderInt("Music volume", &music, 0, 16);
			static int sfx = 0;
			ImGui::SliderInt("SFX volume", &sfx, 0, 16);
		}

		ImGui::End();
	}
	else
	{
		if (gRender->bConnecting && gNetwork->client_running)
		{
			ImGui::SetNextWindowPosCenter();
			ImGui::Begin("Vice City CO-OP " VCCOOP_VER, &gRender->bConnecting, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);
			ImGui::Text("Connecting...");
			ImGui::End();
		}
		else if (gNetwork->client_running == false && gRender->bConnecting == false)
		{
			gRender->bGUI = true;
		}
		if (gRender->bServerView && !gRender->bConnecting)
		{
			ImGui::SetNextWindowPosCenter();
			ImGui::SetNextWindowSize(ImVec2(500, 500));
			ImGui::Begin("Server List", &gRender->bServerView, ImGuiWindowFlags_NoSavedSettings);
			ImGui::AlignTextToFramePadding();
			//---------------------------

			ImGui::Separator();
			for (auto server : serverList) {
				ImGui::Text(server.serverID);
				ImGui::Text(server.serverName);
				ImGui::Text(server.serverHost);
				ImGui::Text(server.serverPort);

				if (ImGui::Button("Connect")) {
					gGame->Name = Nickname;
					sprintf(gNetwork->ServerAddress, server.serverHost);
					gNetwork->ServerPort = atoi(server.serverPort);

					if (strlen(gGame->Name.c_str()) >= 3 && gNetwork->ServerPort != 0 && gNetwork->ServerAddress != "") {
						gNetwork->AttemptConnect(gNetwork->ServerAddress, gNetwork->ServerPort);

						gRender->bConnecting = true;
						gRender->bGUI = false;
						gRender->bAboutWindow = false;
						gRender->bServerView = false;
					}
					else 
					{
						gChat->AddChatMessage("[ERROR] Please ensure all connection settings are valid!");
						gLog->Log("[ERROR] Please ensure all connection settings are valid!\n");
					}
				}
			}
			ImGui::Separator();
			ImGui::End();
			//---------------------------
		}

		if (gStartParams.bWebRequest && !gStartParams.bEmpty && !gRender->bConnecting && !gNetwork->connected && CGame::bLoadingDone)
		{
			gGame->Name = gStartParams.name;
			sprintf(gNetwork->ServerAddress, gStartParams.serveraddress);
			gNetwork->ServerPort = gStartParams.serverport;

			if (strlen(gGame->Name.c_str()) >= 3 && gNetwork->ServerPort != 0 && gNetwork->ServerAddress != "") {
				gNetwork->AttemptConnect(gStartParams.serveraddress, gStartParams.serverport);

				gRender->bConnecting = true;
				gRender->bGUI = false;
				gRender->bAboutWindow = false;
				gRender->bServerView = false;

				gStartParams.bWebRequest = false;
			}
		}

		if (gRender->bGUI && !gRender->bConnecting)
		{
			ImGui::SetNextWindowPosCenter();
			ImGui::Begin("Vice City CO-OP " VCCOOP_VER, &gRender->bGUI, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("\tWelcome to Vice City CO-OP " VCCOOP_VER "\n\t\t  - Alpha Version - ");
			ImGui::Separator();
			if (gRender->pLogoTex != nullptr)
			{
				ImGui::Indent(5.0f);
				ImGui::Image((void*)gRender->pLogoTex, ImVec2(300, 200));
			}
			ImGui::Separator();

			if (ImGui::Button("Server List")) {
				gRender->bServerView = !gRender->bServerView;

			}

			if (!gStartParams.bEmpty)
			{
				ImGui::InputText("Nickname", gStartParams.name, 25, 0, NULL, gStartParams.name);
				ImGui::InputText("IP", gStartParams.serveraddress, 16, 0, NULL, gStartParams.serveraddress);
				ImGui::InputInt("Port", &gStartParams.serverport);
			}
			else
			{
				ImGui::InputText("Nickname", Nickname, 25, 0, NULL, Nickname);
				ImGui::InputText("IP", IP, 16, 0, NULL, IP);
				ImGui::InputInt("Port", &Port);
			}

			if (ImGui::Button("Connect"))
			{
				gGame->Name = Nickname;
				sprintf(gNetwork->ServerAddress, IP);
				gNetwork->ServerPort = Port;

				if (strlen(gGame->Name.c_str()) >= 3 && gNetwork->ServerPort != 0 && gNetwork->ServerAddress != "") {
					gNetwork->AttemptConnect(gNetwork->ServerAddress, gNetwork->ServerPort);

					gRender->bConnecting = true;
					gRender->bGUI = false;
					gRender->bAboutWindow = false;
				}
				else
				{
					gChat->AddChatMessage("[ERROR] Please ensure all connection settings are valid!");
#ifdef VCCOOP_DEBUG
					gRender->gDebugScreen->gDevConsole->AddLog("[ERROR] Please ensure all connection settings are valid!");
#endif
				}
			}

			if (ImGui::Button("Local Server"))
			{
				gGame->Name = Nickname;

				if (strlen(gGame->Name.c_str()) >= 3)
				{
					gNetwork->AttemptConnect("127.0.0.1", VCCOOP_DEFAULT_SERVER_PORT);

					gRender->bConnecting = true;
					gRender->bGUI = false;
					gRender->bAboutWindow = false;
				}
				else
				{
					gChat->AddChatMessage("[ERROR] Please ensure all connection settings are valid!");
				}
			}
			if (ImGui::Button("Back"))
			{
				gLog->Log("[CRender] Back pressed, ending process.\n");
				exit(0);
			}
			if (ImGui::Button("About VC:CO-OP"))
			{
				gRender->bAboutWindow = !gRender->bAboutWindow;
			}
			ImGui::End();
		}
		if (!gRender->bConnecting && gRender->bGUI && gRender->bAboutWindow)
		{
			ImGui::SetNextWindowPosCenter();
			ImGui::Begin("About Vice City CO-OP " VCCOOP_VER, &gRender->bAboutWindow);
			ImGui::Text("WIP");
			ImGui::End();
		}

		// Force connecting window to be displayed until we have connected..
		if (!gNetwork->connected && !gRender->bConnecting && !gRender->bGUI && gNetwork->client_running)
		{
			gRender->bConnecting = true;
		}
	}

	if (gNetwork->connected)
	{
		if (gGame->bShowScoreboard)
		{
			ImGui::SetNextWindowSize(ImVec2(500, 500));
			ImGui::SetNextWindowPosCenter();

			ImGui::Begin("Scoreboard");
			ImGui::Text("ID\t\t\t\t\tName");
			ImGui::Separator();

			ImGui::Text("%d\t\t\t\t\t%s", gNetwork->local_player->id, gGame->Name.c_str());
			ImGui::Separator();

			for (auto it = gNetwork->networkEntities.begin(); it != gNetwork->networkEntities.end(); ++it)
			{
				CClientEntity * networkEntity = (*it);
				if (networkEntity->type == VCOOP_PLAYER)
				{
					CClientPlayer * player = (CClientPlayer*)networkEntity;
					if (player->ped)
					{
						CClientPlayer * player = (CClientPlayer*)networkEntity;
						ImGui::Text("%d\t\t\t\t\t%s", player->networkID, player->szName);
						ImGui::Separator();
					}
				}
			}
			ImGui::End();
		}
	}
}