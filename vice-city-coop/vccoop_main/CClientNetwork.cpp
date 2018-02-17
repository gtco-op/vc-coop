#include "main.h"

std::vector<std::pair<CPed*, int>>	CClientNetwork::players;

bool								CClientNetwork::client_connected;
bool								CClientNetwork::client_running;
bool								CClientNetwork::connected;

CClientPlayer*						networkPlayers[MAX_PLAYERS];

CClientNetwork::CClientNetwork()
{
	client_running = false;
	client_connected = false;
	connected = false;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		networkPlayers[i] = NULL;
	}
}
CClientNetwork::~CClientNetwork()
{
	gLog->Log("[CClientNetwork] CClientNetwork shutting down\n");
}