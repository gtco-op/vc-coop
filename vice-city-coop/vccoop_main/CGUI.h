#ifndef _CGUI_H_DEFINED
#define _CGUI_H_DEFINED
#include "VCCoopConfig.h"
#include "CLogger.h"

#include "plugin_vc.h"
#include "rw\rwd3d9.h"

#include "imgui\imgui.h"
#include "imgui\imgui_impl_dx9.h"

#include <Windows.h>
#include <d3d9.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class CGUI
{
public:
	static bool DisableGameInput;
	bool bGUI = false;
	bool hasInitialized = false;
	bool onceInitHook = false;

	void Shutdown();
	void DrawEvent();

	void ToggleInput(bool Enabled);
	void ToggleGUI();
	static void SetVCHandle();

	HWND tWindow;

	CLogger* l;
	void* n;
	static CGUI* ptr;
public:
	CGUI(CLogger*, void*);
	~CGUI();
};

#endif