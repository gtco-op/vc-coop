#pragma once
class CRender
{
public:
	CRender();
	~CRender();

	void Run();

	/*		ImGui		*/
public:
	bool bGUI;
	bool bConnecting;
	bool bAboutWindow;
	bool bEscMenu;
	bool bServerView;

#ifdef VCCOOP_DEBUG
	bool bConsole;

	CDebugScreen *gDebugScreen;
#endif

	bool Initialized;
	void ToggleGUI();
	void TakeScreenshot();
	
	/*		 D3D9		*/
public:
	bool PedTags;

	void RenderText(const char *sz, RECT rect, DWORD dwColor); 
	void DrawProgressBar(CRect size, int value, CRGBA color, float max = 100.0f);
	SIZE MeasureText(const char * szString);

	void InitFont();

	IDirect3DDevice9 *device;

	std::vector<CRenderTemplate*> gGuiContainer;

	LPDIRECT3DTEXTURE9 pLogoTex;
private:

	ID3DXFont * m_pD3DXFont;
	int iFontSize;

	void Draw();
	void DestroyFont();
};