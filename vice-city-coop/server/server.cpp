/*
	Vice City CO-OP Server

	Author(s)	LemonHaze
				Zeyad Ahmed

	Copyright (c) 2017-2018 VC:CO-OP Team
*/
#ifdef VCCOOP_LIBRG_DEBUG
#define LIBRG_DEBUG
#endif
#define HAS_SOCKLEN_T 1
#define LIBRG_IMPLEMENTATION
#define LIBRG_DEBUG
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "server.h"

CConfiguration		*gConfig;
CLogger				*gLog;
CServerNetwork		*gServerNetwork;
CCustomDataManager	*gDataMgr;

std::string			ServerGamemodePath;

int main(int argc, char const *argv[]) {
	bool console_active = true;
	std::string input;
	
#if defined(_MSC_VER)
	SetConsoleTitle(TEXT(VCCOOP_SERVER_WINDOW_TITLE));
#endif
	
	gLog			= new CLogger;
	gDataMgr		= new CCustomDataManager();
	gConfig			= new CConfiguration;

	gConfig->PopulateValues();
	if (!gConfig->IsConfigLoaded())	{
#if defined(_MSC_VER)
		MessageBoxA(NULL, "An error occurred when populating the server configuration.\nTry running the server with elevated permissions.", VCCOOP_NAME " " VCCOOP_VER, MB_OK | MB_ICONERROR);
#endif
		gLog->Log("[CConfiguration][ERROR] An error occurred when populating the server configuration. Try running the server with elevated permissions.\n");

		console_active = false;
#if defined(_MSC_VER)
		system("PAUSE>NUL");
#endif
	} else {
		if (!gConfig->AutodetectServerGamemode()) {
			gLog->Log("[CCore][ERROR] Could not find a game mode. Exiting.\n"); 

			console_active = false;
#if defined(_MSC_VER)
			system("PAUSE>NUL"); 
#endif
		} else {
			gLog->Log("[CCore][INFO] Server Port: %d\n",	gServerNetwork->ServerPort);
			gLog->Log("[CCore][INFO] Server Secret: %d\n",	gServerNetwork->ServerSecret);
		}
	}

	if (!console_active)
		return 0;

	gServerNetwork = new CServerNetwork;

	while (console_active && gConfig->IsConfigLoaded())	{
		getline(cin, input);

		if (strstr(input.c_str(), "exit") || strstr(input.c_str(), "quit"))		{
			console_active = false;
		}
	}
	return 0;
}