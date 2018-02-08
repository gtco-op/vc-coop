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
	SIZE MeasureText(const char * szString);

	void InitFont();

	IDirect3DDevice9 *device;

	std::vector<CRenderTemplate*> gGuiContainer;
private:
	LPDIRECT3DTEXTURE9 pLogoTex;

	ID3DXFont * m_pD3DXFont;
	int iFontSize;

	void Draw();
	void DestroyFont();
};