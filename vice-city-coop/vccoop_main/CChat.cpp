#include "main.h"

CChat::CChat() 
{
	this->pageSize = 10;

	for (int i = 0; i < 10; i++)
	{
		this->chatBuffer[i][0] = '\0';
	}
}
CChat::~CChat() {}

void CChat::Draw()
{
	//todo: proper pagination implementation
	RECT rect = RECT();

	rect.top = 10;
	rect.left = 30;

	for (int i = 0; i < 10; i++)
	{
		gRender->RenderText(this->chatBuffer[10 - (i + 1)], rect, -1);
		
		SIZE textSize = gRender->MeasureText("Y");

		rect.top += textSize.cy + 1;
	}
}

void CChat::AddChatMessage(const char * message, ...)
{
	char buffer[256];
	va_list args;
	va_start(args, message);
	vsprintf(buffer, message, args);
	va_end(args);

	for (int i = 9; i > 0; i--)
	{
		sprintf(chatBuffer[i], "%s", chatBuffer[i - 1]);
	}

	sprintf(chatBuffer[0], "%s", buffer);
}