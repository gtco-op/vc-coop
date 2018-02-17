/*
	Vice City CO-OP Server

	Author(s)	LemonHaze
				Zeyad Ahmed

	Copyright (c) 2017-2018 VC:CO-OP Team
*/
#include "server.h"

CConfiguration  *gConfig;
CLogger			*gLog;
CServerNetwork	*gServerNetwork;

int main(int argc, char const *argv[]) 
{
	bool console_active = true;
	std::string input;
	
	SetConsoleTitle(TEXT("Vice City CO-OP " VCCOOP_VER " Server"));
	gLog			= new CLogger;
	gConfig			= new CConfiguration;
	gServerNetwork	= new CServerNetwork;

	gConfig->PopulateValues();

	gLog->Log("[CCore] Server Port: %d\n", gServerNetwork->ServerPort);
	gLog->Log("[CCore] Server Secret: %d\n", gServerNetwork->ServerSecret);

	gServerNetwork->InitializeServer();
	gServerNetwork->CreateServerThread();

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