#include "main.h"

char CChat::chatInputBuffer[256];

CChat::CChat() 
{
	this->pageSize = 10;
	this->chatToggled = false; 
	this->chatDisplay = true;

	memset(this->chatInputBuffer, 0, sizeof(this->chatInputBuffer));
	for (int i = 0; i < 10; i++)
	{
		this->chatBuffer[i][0] = '\0';
	}
}
CChat::~CChat() 
{
	gLog->Log("[CChat] Shutting down\n");
}
void CChat::Draw()
{
	//todo: proper pagination implementation
	RECT rect = RECT();

	rect.top = 10;
	rect.left = 30;

#ifdef VCCOOP_DEBUG	
	if (gRender->bConsole && gRender->gDebugScreen->gDevConsole) {
		gRender->gDebugScreen->gDevConsole->Draw(&gRender->bConsole);

		gChat->chatDisplay = false;
		gChat->chatToggled = false;
	}
	else if (gRender->bConsole && !gRender->gDebugScreen->gDevConsole)
	{
		gRender->gDebugScreen->gDevConsole = new DeveloperConsole();
	}
	else
	{
		gChat->chatDisplay = true;
	}
#endif

#ifndef VCCOOP_DEBUG
	if (!this->chatDisplay && gNetwork->connected)
	{
		this->chatDisplay = true;
	}
#endif
	if (this->chatDisplay 
#ifdef VCCOOP_DEBUG
		|| !gRender->gDebugScreen->gDevConsole->Collapsed
#endif
		&& gNetwork->connected
		)
	{
		for (int i = 0; i < 10; i++)
		{
			gRender->RenderText(this->chatBuffer[10 - (i + 1)], rect, -1);
			rect.top += gRender->MeasureText("Y").cy + 1;
		}
	}
	if (this->chatToggled && this->chatDisplay)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::SetNextWindowFocus();
		ImGui::SetNextWindowPos(ImVec2(30, (float)gRender->MeasureText("Y").cy * 10 + 20));
		ImGui::SetNextWindowBgAlpha(-1.0f);
		style.WindowBorderSize = 0.0f;
		ImGui::Begin("A", &this->chatToggled, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		ImGui::PushItemWidth(350.0f);
		
		if (ImGui::InputText("", this->chatInputBuffer, 256, ImGuiInputTextFlags_EnterReturnsTrue, NULL, this->chatInputBuffer))
		{
			char* input_end = this->chatInputBuffer + strlen(this->chatInputBuffer);
			while (input_end > this->chatInputBuffer && input_end[-1] == ' ') { input_end--; } *input_end = 0;
			if (this->chatInputBuffer[0])
			{
				ProcessChatInput();
			}
			strcpy(this->chatInputBuffer, "");
			this->chatToggled = false;
		}

		ImGui::SetKeyboardFocusHere();
		ImGui::PopItemWidth();
		ImGui::End();
		style.WindowBorderSize = 1.0f;
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
void CChat::ToggleChat(bool toggle)
{
	this->chatToggled = toggle;
}
void CChat::ProcessChatInput()
{
	this->ToggleChat(false);
	if (strlen(this->chatInputBuffer) > 0 && this->chatInputBuffer[0] != '\0')
	{
		if (strstr(this->chatInputBuffer, "/quit") || strstr(this->chatInputBuffer, "/exit") || strstr(this->chatInputBuffer, "!quit") || strstr(this->chatInputBuffer, "!exit")
			|| strstr(this->chatInputBuffer, "/q") || strstr(this->chatInputBuffer, "!q"))
		{
			gLog->Log("[CChat] Shutting down\n");
			gGame->Exit();
		}
		else if (strstr(this->chatInputBuffer, "/campos"))
		{
			CVector pos = CCameraStuff::GetCameraPos();
			gLog->Log("[CChat] X: %f\tY: %f\tZ: %f\n", pos.x, pos.y, pos.z);
		}
		else if (strstr(this->chatInputBuffer, "/pos"))
		{
			CVector pos = LocalPlayer()->GetPosition();
			gLog->Log("[CChat] X: %f\tY: %f\tZ: %f\n", pos.x, pos.y, pos.z);
		}
#ifdef VCCOOP_DEBUG
		else if (strstr(this->chatInputBuffer, "/collapse"))
		{
			gRender->gDebugScreen->gDevConsole->Collapsed = !gRender->gDebugScreen->gDevConsole->Collapsed;
		}
		else if (strstr(this->chatInputBuffer, "/debug"))
		{
			debugEnabled = !debugEnabled;
		}
#endif
		else
		{
			char buffer[256];
			sprintf(buffer, "%s: %s", gGame->Name.c_str(), this->chatInputBuffer);

			librg_message_send_all(&gNetwork->ctx, VCOOP_SEND_MESSAGE, buffer, sizeof(buffer));

		}
	}
}