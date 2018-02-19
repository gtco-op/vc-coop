#include "server.h"

CServerPlayer::CServerPlayer(string connectname, SystemAddress connectaddress, int playerid, RakNetGUID rguid)
{
	this->id = playerid;
	this->name = connectname;
	this->address = connectaddress;
	this->guid = rguid;
}

CServerPlayer::~CServerPlayer()
{

}

void CServerPlayer::Update(Packet *packet)
{
	BitStream bitstream(packet->data + 1, packet->length + 1, false);
	bitstream.Read<PlayerSyncData>(this->syncData);
	
	BitStream broadcast;
	broadcast.Write((BYTE)ID_PACKET_PLAYER);
	broadcast.Write(this->id);
	broadcast.Write<PlayerSyncData>(this->syncData);
	gServerNetwork->peerInterface->Send(&broadcast, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, this->address, true);
}