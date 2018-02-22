#include "main.h"

CDebugEngine::CDebugEngine()
{
	this->pageSize = 10;

	for (int i = 0; i < 10; i++)
		this->debugBuffer[i][0] = '\0';
}
CDebugEngine::~CDebugEngine()
{

}
void CDebugEngine::Draw()
{
	RECT rect = RECT();

	rect.top = 225;
	rect.left = 30;

	for (int i = 0; i < 10; i++)
	{
		gRender->RenderText(this->debugBuffer[10 - (i + 1)], rect, -1);
		rect.top += gRender->MeasureText("Y").cy + 1;
	}
}
void CDebugEngine::AddDebugMessage(const char * message, ...)
{
	char buffer[256];
	va_list args;
	va_start(args, message);
	vsprintf(buffer, message, args);
	va_end(args);

	for (int i = 9; i > 0; i--)
		sprintf(debugBuffer[i], "%s", debugBuffer[i - 1]);

	sprintf(debugBuffer[0], "%s", buffer);
}