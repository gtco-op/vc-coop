#pragma once
class CDebugEngine : public CRenderTemplate
{
public:
	CDebugEngine();
	~CDebugEngine();

	char debugBuffer[10][256];
	int pageSize;

	void AddDebugMessage(const char * message, ...);
	void Draw();
};

