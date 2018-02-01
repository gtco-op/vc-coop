#include "main.h"

extern WNDPROC		orig_wndproc;
extern HWND			orig_wnd;
bool   wndHookInited = false;

CRender::CRender()
{
	this->m_pD3DXFont		= NULL;
	this->PedTags			= true;
	this->bGUI				= false;
	this->bInitializedImGui = false;

	this->gGuiContainer.push_back(new CNameTags());
	this->gGuiContainer.push_back(new CDebugScreen());
	this->gGuiContainer.push_back(new CImGui());

	Events::drawingEvent += [] {
		gRender->Draw();
	};
	Events::initRwEvent += [] {
		gRender->InitFont();
	}; 
	Events::shutdownRwEvent += [] {
		gRender->DestroyFont();
		gRender->ShutdownGUI();
	};
	Events::d3dLostEvent += [] {
		gRender->DestroyFont();
		gRender->ShutdownGUI();
	};
	Events::d3dResetEvent += [] {
		gRender->InitFont();
	};

	gLog->Log("[CRender] CRender initialized\n");
}

CRender::~CRender()
{
	this->DestroyFont();

	if(this->bGUI || this->bInitializedImGui)
		this->ShutdownGUI();

	gLog->Log("[CRender] CRender shutting down\n");
}
void CRender::Run()
{
	if (!wndHookInited)
	{
		//pluginsdk wins
		HWND  wnd = RsGlobal.ps->window;
		if (wnd)
		{
			if (orig_wndproc == NULL || wnd != orig_wnd)
			{
				orig_wndproc = (WNDPROC)(UINT_PTR)SetWindowLong(wnd, GWL_WNDPROC, (LONG)(UINT_PTR)wnd_proc);
				orig_wnd = wnd;
			}

			wndHookInited = true;
			gLog->Log("[CRender] Original WndProc hooked\n");


			if (!gRender->bInitializedImGui)
			{
				ImGui_ImplDX9_Init(orig_wnd, gRender->device);

				ImGui::StyleColorsClassic();

				ImGui::GetIO().DisplaySize = { screen::GetScreenWidth(), screen::GetScreenHeight() };

				gLog->Log("[CRender] ImGui initialized\n");
				gRender->bInitializedImGui = true;
			}
		}
	}
}
void CRender::InitFont()
{//All dx and drawing related initialization comes here
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

	if (gRender->bInitializedImGui && orig_wnd)
	{
		ImGui_ImplDX9_Init(orig_wnd, gRender->device);

		ImGui::StyleColorsClassic();

		ImGui::GetIO().DisplaySize = { screen::GetScreenWidth(), screen::GetScreenHeight() };

		gLog->Log("[CRender] ImGui initialized\n");
		gRender->bInitializedImGui = true;
	}
	
	gLog->Log("[CRender] InitFont() finished\n");
}

void CRender::DestroyFont()
{
	if (this->m_pD3DXFont)
	{
		this->m_pD3DXFont->Release();
		this->m_pD3DXFont = NULL;
		gLog->Log("[CRender] Font destroyed\n");
	}
}
void CRender::ToggleGUI()
{
	if (!bGUI)gGame->DisableMouseInput();
	else gGame->EnableMouseInput();
	bGUI = !bGUI;
	ImGui::GetIO().MouseDrawCursor = bGUI;
	gRender->device->ShowCursor(bGUI);
}
void CRender::ShutdownGUI()
{
	bGUI = false; 
	bInitializedImGui = false; 
	ImGui_ImplDX9_Shutdown(); 
}
void CRender::Draw()
{
	if (this->m_pD3DXFont)
	{
		for (int i = 0; i < this->gGuiContainer.size(); i++)
		{
			if (this->gGuiContainer[i])this->gGuiContainer[i]->Draw();
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