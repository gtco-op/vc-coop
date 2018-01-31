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
	bool bInitializedImGui;

	void ToggleGUI();
	void ShutdownGUI();
	
	/*		 D3D9		*/
public:
	bool PedTags;

	void RenderText(const char *sz, RECT rect, DWORD dwColor); 
	SIZE MeasureText(const char * szString);

	void InitFont();

	IDirect3DDevice9 *device;
private:
	ID3DXFont * m_pD3DXFont;
	int iFontSize;

	void Draw();
	void DestroyFont();
};