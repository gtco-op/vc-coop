/*
	Vice City CO-OP Server

	Author(s)	LemonHaze
				Zeyad Ahmed

	Copyright (c) 2017-2018 VC:CO-OP Team
*/
#ifdef VCCOOP_LIBRG_DEBUG
#define LIBRG_DEBUG
#endif
#define LIBRG_IMPLEMENTATION
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "server.h"

CConfiguration		*gConfig;
CLogger				*gLog;
CServerNetwork		*gServerNetwork;
CCustomDataManager	*gDataMgr;
CLuaServer			*gLuaServer;

std::string			ServerGamemodePath;

int main(int argc, char const *argv[]) {
	bool console_active = true;
	std::string input;
	
	SetConsoleTitle(TEXT("Vice City CO-OP " VCCOOP_VER " Server"));
	gLog			= new CLogger;

	gDataMgr		= new CCustomDataManager();
	gConfig			= new CConfiguration;
	gServerNetwork	= new CServerNetwork;

	gConfig->PopulateValues();
	
	gLog->Log("[CCore] Server Port: %d\n", gServerNetwork->ServerPort);
	gLog->Log("[CCore] Server Secret: %d\n", gServerNetwork->ServerSecret);

	if (gConfig->IsConfigLoaded() && !gConfig->AutodetectServerGamemode())
		gLog->Log("[CCore] Could not find a game mode. Exiting."); system("PAUSE>NUL"); console_active = false;

	while (console_active)
	{
		getline(cin, input);

		if (strstr(input.c_str(), "exit") || strstr(input.c_str(), "quit"))		{
			console_active = false;
		}
		Sleep(10);
	}
	return 0;
}