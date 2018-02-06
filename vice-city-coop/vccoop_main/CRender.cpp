#include "main.h"

extern WNDPROC		orig_wndproc;
extern HWND			orig_wnd;
bool   wndHookInited = false;

CRender::CRender()
{
	this->m_pD3DXFont		= NULL;
	this->PedTags			= true;
	this->bGUI				= false;

	this->Initialized = false;

	this->gGuiContainer.push_back(new CNameTags());
	this->gGuiContainer.push_back(new CDebugScreen());

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
		ImGui::GetIO().DisplaySize = { screen::GetScreenWidth(), screen::GetScreenHeight() };
		gLog->Log("[CRender] ImGui initialized\n");
		gGame->DisableMouseInput();
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
	if (Initialized)
	{
		ImGui_ImplDX9_Shutdown();
	}
	this->Initialized = false;
}
void CRender::ToggleGUI()
{
	bGUI = !bGUI;
	ImGui::GetIO().MouseDrawCursor = bGUI;
	gRender->device->ShowCursor(bGUI);
	if(bGUI) gGame->DisableMouseInput();
	else gGame->EnableMouseInput();
}
void CRender::Draw()
{
	if (this->m_pD3DXFont)
	{
		for (int i = 0; i < (int)this->gGuiContainer.size(); i++)
		{
			if (this->gGuiContainer[i])this->gGuiContainer[i]->Draw();
		}
	}
	if (Initialized && gRender->bGUI)
	{
		ImGui_ImplDX9_NewFrame();
		ImGui::Begin("Vice City CO-OP " VCCOOP_VER, &gRender->bGUI);
		ImGui::Text("Welcome to Vice City CO-OP " VCCOOP_VER "\n - Alpha Version - ");

		ImGui::InputText("Nickname", Nickname, 25, 0, NULL, Nickname);
		ImGui::InputText("IP", IP, 16, 0, NULL, IP);
		ImGui::InputInt("Port", &Port);

		if (ImGui::Button("Connect"))
		{
			gRender->bGUI			= false;

			gGame->Name				= Nickname;

			gNetwork->ServerAddress = IP;
			gNetwork->ServerPort	= Port;

			if (strlen(gGame->Name.c_str()) >= 3) {
				gNetwork->AttemptConnect(gNetwork->ServerAddress, gNetwork->ServerPort);
			}
		}
		if (ImGui::Button("About VC:CO-OP"))
		{
			gLog->Log("[CRender] About button clicked\n");
		}
		ImGui::End();
		ImGui::EndFrame();
		ImGui::Render();
	}

	// should we allow VC mouse input?
	if (Initialized)
	{
		// if chat or GUI is active, then no..
		if (gRender->bGUI || gChat->chatToggled)
		{
			gGame->DisableMouseInput();
			gRender->device->ShowCursor(TRUE);
		}
		else
		{
			gGame->EnableMouseInput();
			gRender->device->ShowCursor(FALSE);
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