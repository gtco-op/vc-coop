#include "main.h"
#include "LogoTextureArray.h"

extern WNDPROC		orig_wndproc;
extern HWND			orig_wnd;
bool   wndHookInited = false;

CRender::CRender()
{
	this->pLogoTex			= NULL;
	this->m_pD3DXFont		= NULL;
	this->PedTags			= true;
	this->bGUI				= false;
	this->bConnecting		= false;
	this->bAboutWindow		= false;
	this->Initialized		= false;

	this->gGuiContainer.push_back(new CNameTags());

#ifdef VCCOOP_DEBUG
	this->bConsole = false;

	this->gDebugScreen = new CDebugScreen();
#endif

	Events::drawingEvent += [] {
		gRender->Draw();
	};
	Events::initRwEvent += [] {
		gRender->InitFont();
	}; 
	Events::shutdownRwEvent += [] {
		gRender->DestroyFont();
	};
	Events::d3dLostEvent += [] {
		gRender->DestroyFont();
	};
	Events::d3dResetEvent += [] {
		gRender->DestroyFont();
		gRender->InitFont();
	};
	gLog->Log("[CRender] CRender initialized\n");
}

CRender::~CRender()
{
	this->DestroyFont();
	gLog->Log("[CRender] CRender shutting down\n");
}
void CRender::Run()
{
	if (!wndHookInited)	{
		HWND  wnd = RsGlobal.ps->window;
		if (wnd)		{
			if (orig_wndproc == NULL || wnd != orig_wnd)			{
				orig_wndproc = (WNDPROC)(UINT_PTR)SetWindowLong(wnd, GWL_WNDPROC, (LONG)(UINT_PTR)wnd_proc);
				orig_wnd = wnd;

				SetWindowText(wnd, VCCOOP_WINDOW_TITLE);
			}
			wndHookInited = true;
			gLog->Log("[CRender] Original WndProc hooked\n");

			ImGui_ImplDX9_Init(orig_wnd, this->device);
			ImGui::GetIO().DisplaySize = { screen::GetScreenWidth(), screen::GetScreenHeight() };
			gLog->Log("[CRender] ImGui initialized\n");

			Initialized = true;
		}
	}
}
void CRender::InitFont()
{
	this->device = reinterpret_cast<IDirect3DDevice9 *>(RwD3D9GetCurrentD3DDevice());

	if (screen::GetScreenWidth() < 1024)
	{
		iFontSize = 14;
	}
	else if (screen::GetScreenWidth() == 1024)
	{
		iFontSize = 16;
	}
	else  if (screen::GetScreenWidth() > 1024 && screen::GetScreenWidth() <= 2048)
	{
		iFontSize = 18;
	}
	else  if (screen::GetScreenWidth() > 2048)
	{
		iFontSize = 20;
	}

	D3DXCreateFont(device, iFontSize, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial", &m_pD3DXFont);
	gLog->Log("[CRender] InitFont() finished\n");

	if (!Initialized)
	{
		ImGui_ImplDX9_Init(orig_wnd, this->device);
		
		ImGuiIO io = ImGui::GetIO();
		io.DisplaySize = { screen::GetScreenWidth(), screen::GetScreenHeight() };

		// Disable INI saving in ImGui (crashfix)
		io.IniFilename = NULL;
		io.LogFilename = NULL;

		gLog->Log("[CRender] ImGui initialized\n");
		gGame->DisableMouseInput();

		if (pLogoTex == nullptr)
		{
			HRESULT res = D3DXCreateTextureFromFileInMemory(device, &logoTexData, sizeof(logoTexData), &pLogoTex);
			if (res != D3D_OK) {
				gLog->Log("[CRender] Logo texture could not be created! (Error: %d)\n", res);
			}
		}

		// Populate GUI variables with retrieved config values
		gConfig->PopulateValues(IP, Port, Nickname);

		Initialized = true;
	}
}

void CRender::DestroyFont()
{
	if (this->m_pD3DXFont)
	{
		this->m_pD3DXFont->Release();
		this->m_pD3DXFont = NULL;
		gLog->Log("[CRender] Font destroyed\n");
	}
	if (this->pLogoTex)
	{
		this->pLogoTex->Release();
		this->pLogoTex = NULL;
		gLog->Log("[CRender] Logo texture destroyed\n");
	}
	if (Initialized)
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
		gLog->Log("[CRender] ImGui destroyed\n");
	}
	this->Initialized = false;
}
void CRender::ToggleGUI()
{
	bGUI = !bGUI;

	if (!gNetwork->connected && !gNetwork->client_running)
	{
		bConnecting = false;
	}
}
void CRender::Draw()
{
	if (this->m_pD3DXFont)
	{
		if (Initialized)
		{
			ImGui_ImplDX9_NewFrame();

#ifdef VCCOOP_DEBUG
			gRender->gDebugScreen->Draw();
#endif

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
			if (gRender->bGUI && !gRender->bConnecting)
			{
				ImGui::SetNextWindowPosCenter();
				ImGui::Begin("Vice City CO-OP " VCCOOP_VER, &gRender->bGUI, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);
				ImGui::AlignTextToFramePadding();
				ImGui::Text("\tWelcome to Vice City CO-OP " VCCOOP_VER "\n\t\t  - Alpha Version - ");
				ImGui::Separator();
				if (pLogoTex != nullptr)
				{
					ImGui::Indent(5.0f);
					ImGui::Image((void*)pLogoTex, ImVec2(300,200));
				}
				ImGui::Separator();
				ImGui::InputText("Nickname", Nickname, 25, 0, NULL, Nickname);
				ImGui::InputText("IP", IP, 16, 0, NULL, IP);
				ImGui::InputInt("Port", &Port);

				if (ImGui::Button("Connect"))
				{
					gGame->Name = Nickname;
					gNetwork->ServerAddress = IP;
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

			// Draw all rendering elements in our GUI container
			for (int i = 0; i < (int)this->gGuiContainer.size(); i++)
			{
				if (this->gGuiContainer[i])
					this->gGuiContainer[i]->Draw();
			}

			ImGui::EndFrame();
			ImGui::Render();
		}
	}

	// should we allow VC mouse input?
	if (Initialized)
	{
		// if chat or GUI is active, then no..
		if (gRender->bGUI || gChat->chatToggled || 
			gRender->bConnecting || gRender->bAboutWindow 
#ifdef VCCOOP_DEBUG
			|| gRender->bConsole
#endif
			)
		{
			gGame->DisableMouseInput();
		}
		else
		{
			gGame->EnableMouseInput();
		}
	}
}
void CRender::RenderText(const char *sz, RECT rect, DWORD dwColor)
{
	//Black outline
	this->m_pD3DXFont->DrawText(NULL, sz, -1, new RECT{ rect.left - 1, rect.top }, DT_NOCLIP | DT_LEFT, 0xFF000000);
	this->m_pD3DXFont->DrawText(NULL, sz, -1, new RECT{ rect.left + 1, rect.top }, DT_NOCLIP | DT_LEFT, 0xFF000000);
	this->m_pD3DXFont->DrawText(NULL, sz, -1, new RECT{ rect.left, rect.top - 1 }, DT_NOCLIP | DT_LEFT, 0xFF000000);
	this->m_pD3DXFont->DrawText(NULL, sz, -1, new RECT{ rect.left, rect.top + 1 }, DT_NOCLIP | DT_LEFT, 0xFF000000);

	this->m_pD3DXFont->DrawText(NULL, sz, -1, &rect, DT_NOCLIP | DT_LEFT, dwColor);
}
SIZE CRender::MeasureText(const char * szString)
{
	RECT rect;
	SIZE ret;

	this->m_pD3DXFont->DrawText(0, szString, -1, &rect, DT_CALCRECT | DT_LEFT, 0xFF000000);
	ret.cx = rect.right - rect.left;
	ret.cy = rect.bottom - rect.top;

	return ret;
}