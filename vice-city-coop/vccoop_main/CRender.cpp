#include "main.h"

extern WNDPROC		orig_wndproc;
extern HWND			orig_wnd;
bool   wndHookInited = false;
char   vccoop_string[600];

CRender::CRender()
{
	this->m_pD3DXFont		= NULL;
	this->PedTags			= true;
	this->bGUI				= true;
	this->bInitializedImGui = false;

	Events::drawingEvent += [] {
		if (!gRender->bInitializedImGui && gRender->bGUI && orig_wnd != nullptr)		{
			gRender->device = (LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice();
			ImGui_ImplDX9_Init(orig_wnd, gRender->device);
			ImGui::StyleColorsClassic();

			gLog->Log("[CRender] ImGui initialized\n");
			gRender->bInitializedImGui = true;
		}

		if (gRender->bInitializedImGui && gRender->bGUI && orig_wnd != nullptr)		{
			ImGui_ImplDX9_NewFrame();
			ImGui::Begin("Vice City CO-OP " VCCOOP_VER);
			ImGui::Text("Welcome to Vice City CO-OP " VCCOOP_VER "\nThis is freaking alpha version");

			ImGui::InputText("IP", IP, sizeof(IP));
			ImGui::InputInt("Port", &Port);

			if (ImGui::Button("Connect")) {
				gLog->Log("[CRender] Connect button clicked..\n");
				gRender->bGUI = false;
				gRender->bInitializedImGui = false;

				gNetwork->AttemptConnect(IP, Port);
			}
			if (ImGui::Button("About VC:CO-OP")) {
				gLog->Log("[CRender] About button clicked..\n");
			}
			ImGui::End();
			ImGui::EndFrame();
			ImGui::Render();
		}

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

	//Lets initialize the wnd hook when the game already loaded it, if font loads hwnd should be OK
	if (!wndHookInited)
	{
		//pPresentParam = reinterpret_cast<D3DPRESENT_PARAMETERS *>(0xC9C040); this shit doesnt exists in vc xD
		HWND*  wnd = (HWND*)(0xA0FD1C);

		if (orig_wndproc == NULL || *wnd != orig_wnd)
		{
			orig_wndproc = (WNDPROC)(UINT_PTR)SetWindowLong(*wnd, GWL_WNDPROC, (LONG)(UINT_PTR)wnd_proc);
			orig_wnd = *wnd;
		}

		wndHookInited = true;
		gLog->Log("[CRender] Original WndProc hooked\n");
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
	bGUI = !bGUI;
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
		if (!gNetwork->connected)		{
			sprintf(vccoop_string, "%s %s", VCCOOP_NAME, VCCOOP_VER);
		} else	{
			sprintf(vccoop_string, "%s %s     Server: %s:%d   Press F7 to disconnect", VCCOOP_NAME, VCCOOP_VER, IP, Port);
		}

		this->RenderText(vccoop_string, { 10, 10, 10 }, (gNetwork->connected? 0xFF00FF00 : 0xFFFFFFFF));

		for (int i = 0; i < CPools::ms_pPedPool->m_nSize; i++)
		{
			CPed *ped = CPools::ms_pPedPool->GetAt(i);
			if (ped)
			{
				CVector posn = ped->GetPosition();
				RwV3d screenCoors; float w, h;
				if (CSprite::CalcScreenCoors({ posn.x, posn.y, posn.z + 1.3f }, &screenCoors, &w, &h, true))
				{
					char text[600];

					sprintf(text, "Ped: %d", i);

					SIZE size = this->MeasureText(text);

					this->RenderText(text, { (LONG)screenCoors.x - (LONG)ceil(size.cx / 2.0), (LONG)screenCoors.y }, 0xFFFFFFFF);

					CSprite2d::DrawRect(CRect(screenCoors.x - 60, screenCoors.y - 5 + 30, screenCoors.x + 60, screenCoors.y + 5 + 30), CRGBA(0, 0, 0, 255));
					CSprite2d::DrawRect(CRect(screenCoors.x - 60 + 3, screenCoors.y - 5 + 30 + 1, screenCoors.x + 60 - 3, screenCoors.y + 5 + 30 - 1), CRGBA(145, 0, 0, 255));
					CSprite2d::DrawRect(CRect(screenCoors.x - 60 + 3, screenCoors.y - 5 + 30 + 1, (screenCoors.x - 60 + 3) + (((screenCoors.x + 60 - 3) - (screenCoors.x - 60 + 3)) / 100.0f)*ped->m_fHealth, screenCoors.y + 5 + 30 - 1), CRGBA(255, 0, 0, 255));
				}
			}
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