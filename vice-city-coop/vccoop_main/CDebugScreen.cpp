#include "main.h"

char   vccoop_string[600];

CDebugScreen::CDebugScreen() {}
CDebugScreen::~CDebugScreen() {}

void CDebugScreen::Draw()
{
	if (!gNetwork->connected)
	{
		sprintf(vccoop_string, "%s %s", VCCOOP_NAME, VCCOOP_VER);
	}
	else
	{
		sprintf(vccoop_string, "%s %s     Server: %s:%d   Press F7 to disconnect", VCCOOP_NAME, VCCOOP_VER, IP, Port);
	}

	SIZE textSize = gRender->MeasureText(vccoop_string);

	gRender->RenderText(vccoop_string, { 10, (LONG)screen::GetScreenHeight() - textSize.cy - 5 }, (gNetwork->connected ? 0xFF00FF00 : 0xFFFFFFFF));

	sprintf(vccoop_string, "Ped pool: %d/%d\nVehicle pool: %d/%d", CPools::ms_pPedPool->GetNoOfUsedSpaces(), CPools::ms_pPedPool->m_nSize, CPools::ms_pVehiclePool->GetNoOfUsedSpaces(), CPools::ms_pVehiclePool->m_nSize);
	textSize = gRender->MeasureText(vccoop_string);
	gRender->RenderText(vccoop_string, { (LONG)screen::GetScreenWidth() - textSize.cx - 5, (LONG)screen::GetScreenHeight() -textSize.cy - 5 }, 0xFFFFFFFF);
}