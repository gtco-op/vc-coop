#pragma once

#include "main.h"
class CRichPresence {

public:
	CRichPresence();
	~CRichPresence();

	void UpdateRichPresence(bool IsConnected);
	void Shutdown();

	void StartThread();
	void StopThread();

	static void rpThread(LPVOID lParam);
};