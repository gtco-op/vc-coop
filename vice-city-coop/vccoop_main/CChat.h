#pragma once
class CChat : public CRenderTemplate
{
public:
	CChat();
	~CChat();

	void Draw();

	int pageSize;
	bool chatToggled;
	bool chatDisplay;

	char chatBuffer[10][256];
	static char chatInputBuffer[256];
	
	void AddChatMessage(const char * message, ...);
	void ToggleChat(bool toggle);
	void ProcessChatInput();
};
