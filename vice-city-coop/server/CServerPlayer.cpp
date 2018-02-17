#include "server.h"

CServerPlayer::CServerPlayer(string connectname, SystemAddress connectaddress, int playerid)
{
	this->id = playerid;
	this->name = connectname;
	this->address = connectaddress;
}

CServerPlayer::~CServerPlayer()
{

}