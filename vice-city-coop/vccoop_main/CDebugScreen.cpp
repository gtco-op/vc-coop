#include "main.h"

bool debugEnabled = true;
char   vccoop_string[600];

#ifdef VCCOOP_DEBUG
DeveloperConsole* CDebugScreen::gDevConsole;

CDebugScreen::CDebugScreen() 
{
	debugEnabled = true;
}
CDebugScreen::~CDebugScreen() 
{

}
void CDebugScreen::Draw()
{
	if (!gNetwork->connected)
	{
		sprintf(vccoop_string, "VC CO-OP Build: " "%s", GIT_BUILD);
	}
	else
	{
		sprintf(vccoop_string, "VC CO-OP Build: " "%s     Server: %s:%d   Press F7 to disconnect", GIT_BUILD, gNetwork->addr.host, gNetwork->addr.port);
	}

	SIZE textSize = gRender->MeasureText(vccoop_string);

	gRender->RenderText(vccoop_string, { 10, (LONG)screen::GetScreenHeight() - textSize.cy - 5 }, (gNetwork->connected ? 0xFF00FF00 : 0xFFFFFFFF));

	sprintf(vccoop_string, "Ped pool: %d/%d\nVehicle pool: %d/%d\nmsTime: %d\n", CPools::ms_pPedPool->GetNoOfFreeSpaces(), CPools::ms_pPedPool->m_nSize, CPools::ms_pVehiclePool->GetNoOfFreeSpaces(), CPools::ms_pVehiclePool->m_nSize, CTimer::m_snTimeInMilliseconds);
	textSize = gRender->MeasureText(vccoop_string);
	gRender->RenderText(vccoop_string, { (LONG)screen::GetScreenWidth() - textSize.cx - 5, (LONG)screen::GetScreenHeight() -textSize.cy - 5 }, 0xFFFFFFFF);
}
#endif
