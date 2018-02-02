#include "main.h"

CChat::CChat() 
{
	this->pageSize = 10;
	this->chatToggled = false; 
	memset(this->chatInputBuffer, 0, sizeof(this->chatInputBuffer));
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

		rect.top += gRender->MeasureText("Y").cy + 1;
	}
	
	if (this->chatToggled)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImGui_ImplDX9_NewFrame();
		ImGui::SetNextWindowFocus();
		ImGui::SetNextWindowPos(ImVec2(30, (float)gRender->MeasureText("Y").cy * 10 + 20));
		ImGui::SetNextWindowBgAlpha(-1.0f);
		style.WindowBorderSize = 0.0f;
		ImGui::Begin("A", (bool*)1, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		ImGui::PushItemWidth(350.0f);
		ImGui::SetKeyboardFocusHere(); 
		ImGui::InputText("", this->chatInputBuffer, IM_ARRAYSIZE(this->chatInputBuffer));
		ImGui::PopItemWidth();
		ImGui::End();
		ImGui::EndFrame();
		ImGui::Render();
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
	if (!toggle)gGame->EnableMouseInput();
	else gGame->DisableMouseInput();

	ImGui::GetIO().MouseDrawCursor = toggle;
	gRender->device->ShowCursor(toggle);
	this->chatToggled = toggle;
}

void CChat::ProcessChatInput()
{
	this->ToggleChat(false);
	if (strlen(this->chatInputBuffer) > 0 && this->chatInputBuffer[0] != '\0')
	{
		this->AddChatMessage(this->chatInputBuffer);
		this->chatInputBuffer[0] = 0;
	}
}