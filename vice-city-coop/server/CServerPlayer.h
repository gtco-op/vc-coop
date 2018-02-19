#pragma once
class CServerPlayer
{
public:
	string name;
	SystemAddress address;
	int id;
	RakNetGUID guid;

	PlayerSyncData syncData;

	CServerPlayer(string connectname, SystemAddress connectaddress, int playerid, RakNetGUID rguid);
	~CServerPlayer();

	void Update(Packet *packet);
};