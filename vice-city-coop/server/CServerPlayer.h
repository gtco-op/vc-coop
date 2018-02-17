#pragma once
class CServerPlayer
{
public:
	string name;
	SystemAddress address;
	int id;

	PlayerSyncData syncData;

	CServerPlayer(string connectname, SystemAddress connectaddress, int playerid);
	~CServerPlayer();
};