#pragma once
class CChat : public CRenderTemplate
{
public:
	CChat();
	~CChat();

	void Draw();

	int pageSize;
	char chatBuffer[10][256];
	void AddChatMessage(const char * message, ...);
};
