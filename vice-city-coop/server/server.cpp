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

std::string			ServerGamemodePath;

int main(int argc, char const *argv[]) {
	bool console_active = true;
	std::string input;
	
	SetConsoleTitle(TEXT("Vice City CO-OP " VCCOOP_VER " Server"));
	
	gLog			= new CLogger;
	gDataMgr		= new CCustomDataManager();
	gConfig			= new CConfiguration;

	gConfig->PopulateValues();
	if (!gConfig->IsConfigLoaded())	{
		MessageBoxA(NULL, "An error occurred when populating the server configuration.\nTry running the server with elevated permissions.", VCCOOP_NAME " " VCCOOP_VER, MB_OK | MB_ICONERROR);
		gLog->Log("[CConfiguration][ERROR] An error occurred when populating the server configuration. Try running the server with elevated permissions.\n");

		console_active = false;
		system("PAUSE>NUL");
	} else {
		if (!gConfig->AutodetectServerGamemode()) {
			gLog->Log("[CCore][ERROR] Could not find a game mode. Exiting.\n"); 

			console_active = false;
			system("PAUSE>NUL"); 
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