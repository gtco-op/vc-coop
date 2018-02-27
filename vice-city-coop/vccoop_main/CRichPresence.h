#pragma once

#include "main.h"
class CRichPresence {

public:
	//    Discord_Initialize(APPLICATION_ID, &handlers, 1, STEAM_APP_ID);
	CRichPresence();
	~CRichPresence();
	static void InitRichPresence();
	static void UpdateRichPresence(bool IsConnected);
	static void Shutdown();

};