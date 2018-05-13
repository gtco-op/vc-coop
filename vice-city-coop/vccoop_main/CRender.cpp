#include "main.h"
#include "LogoTextureArray.h"

extern WNDPROC		orig_wndproc;
extern HWND			orig_wnd;
bool   wndHookInited = false;

std::vector<serverInfo> serverList;

CRender::CRender()
{
	this->pLogoTex			= NULL;
	this->m_pD3DXFont		= NULL;

	this->PedTags			= true;
	
	this->bGUI				= false;
	this->bConnecting		= false;
	this->bAboutWindow		= false;
	this->Initialized		= false;
	this->bEscMenu			= false;
	this->bServerView		= false;

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

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}
bool DownloadFile(char* URL, char* Filename = "tmp.xml")
{
	CURL *curl;
	FILE *fp;
	CURLcode res;
	curl = curl_easy_init();

	if (curl) {
		fp = fopen(Filename, "wb");
		curl_easy_setopt(curl, CURLOPT_URL, URL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);

		if (res != CURLE_OK)
		{
			gLog->Log("[CCore] Could not retrieve '%s' (error=%s).\n", URL, curl_easy_strerror(res));
			fclose(fp);
			curl_easy_cleanup(curl);
			return false;
		}
		else
		{
			gLog->Log("[CCore] Retrieved '%s' successfully.\n", URL);
			fclose(fp);
			curl_easy_cleanup(curl);
			return true;
		}
	}
	return false;
}

std::vector<serverInfo> GetServersFromURL(char* URL = VCCOOP_DEFAULT_MASTER_LIST_URL)
{
	std::vector<serverInfo> result;

	if (DownloadFile(URL))	{
		tinyxml2::XMLDocument doc;
		doc.LoadFile("tmp.xml");
		tinyxml2::XMLHandle docHandle(&doc);
		tinyxml2::XMLElement *entry = docHandle.FirstChildElement("servers").ToElement();

		if (entry) {
			for (tinyxml2::XMLNode *node = entry->FirstChildElement(); node; node = node->NextSibling()) {
				serverInfo temp;
				tinyxml2::XMLElement *e = node->ToElement();

				const char* serverID	= e->Attribute("id");
				const char* serverName	= e->FirstChildElement("name")->GetText();
				const char* serverHost	= e->FirstChildElement("host")->GetText();
				const char* serverPort	= e->FirstChildElement("port")->GetText();

				sprintf(temp.serverID, serverID);
				sprintf(temp.serverName, serverName);
				sprintf(temp.serverHost, serverHost);
				sprintf(temp.serverPort, serverPort);
				result.push_back(temp);
			}
		}
		gLog->Log("[CCore] Retrieved %d servers\n", result.size());
		remove("tmp.xml");
	}
	return result;
}


CRender::~CRender()
{
	this->DestroyFont();
	gLog->Log("[CRender] CRender shutting down\n");
}
void CRender::Run()
{
	if (!wndHookInited)	
	{
		HWND  wnd = RsGlobal.ps->window;
		if (wnd)		
		{
			if (orig_wndproc == NULL || wnd != orig_wnd)			
			{
				orig_wndproc = (WNDPROC)(UINT_PTR)SetWindowLong(wnd, GWL_WNDPROC, (LONG)(UINT_PTR)wnd_proc);
				orig_wnd = wnd;

				SetWindowText(wnd, VCCOOP_WINDOW_TITLE);
				ImmAssociateContext(wnd, 0);
			}
			RECT rect;
			GetWindowRect(wnd, &rect);
			
			gLog->Log("[CRender] rright: %d\n", rect.right);
			gLog->Log("[CRender] Display Mode: %d\n", MemRead<s32>(0x9B6CBC));

			MemWrite<s32>(0x9B6CBC, rect.right/2);

			wndHookInited = true;
			gLog->Log("[CRender] Original WndProc hooked\n");

			ImGui_ImplDX9_Init(orig_wnd, this->device);
			ImGui::GetIO().DisplaySize = { screen::GetScreenWidth(), screen::GetScreenHeight() };
			gLog->Log("[CRender] ImGui initialized\n");

			Initialized = true;

			serverList = GetServersFromURL();
			if (serverList.size() == 0)			{
				serverList = GetServersFromURL(VCCOOP_DEFAULT_MASTER_LIST_BACKUP_URL);
			}
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
		if (gStartParams.bEmpty)
		{
			gConfig->PopulateValues(IP, Port, Nickname);
			if (!gConfig->IsDataDirValid() || !gConfig->IsConfigLoaded()) {
				//MessageBoxA(orig_wnd, "An error occurred when populating the game configuration.\nTry running Vice City with elevated permissions.", VCCOOP_NAME " " VCCOOP_VER, MB_OK | MB_ICONERROR);
				gLog->Log("[CConfiguration] An error occurred when populating the game configuration. Try running Vice City with elevated permissions.\n");
				//gGame->Exit();
			}
		}
		else
		{

			if (gStartParams.bConnect && !gStartParams.bEmpty && gGame->Name.empty()) {
				std::string name = gConfig->GetReader()->Get("Client", "Nickname", VCCOOP_DEFAULT_NICKNAME);
				strcpy(gStartParams.name, name.c_str());

				gGame->Name = gStartParams.name;
				sprintf(gConfig->Nickname, "%s", gStartParams.name);
			}

			strcpy(gConfig->ServerAddress, gStartParams.serveraddress);
			gConfig->ServerPort = gStartParams.serverport;
			strcpy(gConfig->Nickname, gStartParams.name);

			strcpy(IP, gStartParams.serveraddress);
			Port = gStartParams.serverport;
			strcpy(Nickname, gStartParams.name);
		}
		gRender->Initialized = true;
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
	if (!this)
		return;

	bGUI = !bGUI;

	if (bGUI)
	{
		gGame->DisableHUD();
	}
	else
	{
		gGame->EnableHUD();
	}

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
			|| gRender->bEscMenu || gRender->bServerView
			)
		{
			gGame->DisableMouseInput();
			gRender->device->ShowCursor(TRUE);
			ImGui::GetIO().MouseDrawCursor = TRUE;
		}
		else if(gGame->IsWindowActive())
		{
			gGame->EnableMouseInput();
			gRender->device->ShowCursor(FALSE);
			ImGui::GetIO().MouseDrawCursor = FALSE;
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
void CRender::TakeScreenshot()
{
	std::string fName("screenshot_");
	fName.append(time_stamp(DEFAULT_TIME_FORMAT));
	fName.append(".bmp");

	LPDIRECT3DSURFACE9 pBackBuffer;
	gRender->device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	D3DXSaveSurfaceToFileA(fName.c_str(), D3DXIFF_BMP, pBackBuffer, NULL, NULL);
	SAFE_RELEASE(pBackBuffer);

	gLog->Log("[CRender] Screenshot saved to %s\n", fName.c_str());
}
void CRender::DrawProgressBar(CRect size, int value, CRGBA color, float max)
{
	CRGBA darkbg = color;
	if (color.red < 65)darkbg.red = 0;
	else darkbg.red = color.red - 65;
	if (color.green < 65)darkbg.green = 0;
	else darkbg.green = color.green - 65;
	if (color.blue < 65)darkbg.blue = 0;
	else darkbg.blue = color.blue - 65;

	CSprite2d::DrawRect(CRect(size.left, size.top, size.right, size.bottom), CRGBA(0, 0, 0, 255));
	CSprite2d::DrawRect(CRect(size.left + 1, size.top + 1, size.right - 1, size.bottom - 1), darkbg);

	int len = (size.right - size.left);
	CSprite2d::DrawRect(CRect(size.left + 1, size.top + 1, size.left + ceil((len / max)*value) - 1, size.bottom - 1), color);
}
