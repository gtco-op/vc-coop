#pragma once

#include "main.h"
class CRichPresence {

public:
	CRichPresence();
	~CRichPresence();

	bool threadActive;

	void UpdateRichPresence(bool IsConnected);
	void Shutdown();

	void StartThread();
	void StopThread();

	static void rpThread(LPVOID lParam);
};