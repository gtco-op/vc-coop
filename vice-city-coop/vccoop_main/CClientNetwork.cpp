#define LIBRG_IMPLEMENTATION
#define LIBRG_DEBUG
#include "main.h"

librg_ctx_t CClientNetwork::ctx;
librg_entity_t * CClientNetwork::local_player;
std::vector<CPed*> CClientNetwork::players;
SPlayerData LocalPlayerInfo;

bool CClientNetwork::client_connected;
bool CClientNetwork::client_running;
bool CClientNetwork::connected;

CClientNetwork::CClientNetwork()
{
	ctx = { 0 };
	local_player = nullptr;

	client_running = false;
	client_connected = false;
	connected = false;
}
CClientNetwork::~CClientNetwork()
{
	this->StopClientThread();
}
void CClientNetwork::on_connect_request(librg_event_t *event) {
	char name[25];
	strcpy(name, gGame->Name.c_str());

	gLog->Log("[CClientNetwork] Connecting as %s\n", name);

	librg_data_wptr(event->data, (void*)&name, sizeof(name));
}
void CClientNetwork::on_connect_accepted(librg_event_t *event) {
	gLog->Log("[CClientNetwork] Connection Accepted\n");

	connected = true;
	if (event->entity)
	{
		local_player = event->entity;

		// first player..
		if (event->entity->id == 0)
		{
			// Add CPopulation shit..
			gGame->UnpatchAddToPopulation();
			CPopulation::GeneratePedsAtStartOfGame();
		}
	}
}
void CClientNetwork::on_connect_refused(librg_event_t *event) {
	gLog->Log("[CClientNetwork] Connection Refused\n");
}
void CClientNetwork::StopClientThread()
{
	client_running = false;
	client_connected = false;
	connected = false;
}
void CClientNetwork::ClientConnectThread()
{
	while (client_running) {
		if (connected && local_player)
		{
			LocalPlayerInfo.iModelIndex			= LocalPlayer()->m_nModelIndex;
			LocalPlayerInfo.Health				= LocalPlayer()->m_fHealth;
			LocalPlayerInfo.Rotation			= LocalPlayer()->m_fRotationCur;
			LocalPlayerInfo.Armour				= LocalPlayer()->m_fArmour;
			LocalPlayerInfo.iCurrentAnimID		= LocalPlayer()->m_dwAnimGroupId;
		}
		librg_tick(&ctx);
		zpl_sleep_ms(1);
	}

	librg_network_stop(&ctx);
	librg_free(&ctx);
}
void CClientNetwork::on_entity_create(librg_event_t *event) {
	SPlayerData spd;
	zplm_vec3_t position = event->entity->position;
	CStreaming::RequestModel(7, 0);

	librg_data_rptr(event->data, &spd, sizeof(SPlayerData));

	CPed *ped = new CPlayerPed();
	ped->SetModelIndex(7);
	CWorld::Add(ped);
	ped->Teleport(CVector(position.x, position.y, position.z));

	event->entity->user_data = ped;

	players.push_back(ped);
	gLog->Log("[CClientNetwork] Network entity %d initialized\n", event->entity->id);
}

void CClientNetwork::on_entity_update(librg_event_t *event) {
	SPlayerData spd;
	librg_data_rptr(event->data, &spd, sizeof(SPlayerData));
	
	auto ped = (CPlayerPed *)event->entity->user_data;
	ped->Teleport(*(CVector *)&event->entity->position);

	ped->m_fHealth			= spd.Health;
	ped->m_nModelIndex		= spd.iModelIndex;
	ped->m_fRotationCur		= spd.Rotation;
	ped->m_fArmour			= spd.Armour;
}

void CClientNetwork::on_client_stream(librg_event_t *event) {
	auto pos = LocalPlayer()->GetPosition();
	event->entity->position.x = pos.x;
	event->entity->position.y = pos.y;
	event->entity->position.z = pos.z;
	librg_data_wptr(event->data, &LocalPlayerInfo, sizeof(LocalPlayerInfo));
}
void CClientNetwork::on_entity_remove(librg_event_t *event) {
	auto tmp = std::find(players.begin(), players.end(), event->entity->user_data);
	if (tmp != players.end())
	{
		players.erase(tmp);
	}

	delete event->entity->user_data;
}
void CClientNetwork::on_disconnect(librg_event_t *event) {
	StopClientThread();

	gLog->Log("[CClientNetwork] Disconnected.\n");
}
void CClientNetwork::AttemptConnect(char* szAddress, int iPort)
{
	client_running = true;
	ctx.mode = LIBRG_MODE_CLIENT;
	librg_init(&ctx);

	librg_event_add(&ctx, LIBRG_CONNECTION_REQUEST,		on_connect_request);
	librg_event_add(&ctx, LIBRG_CONNECTION_ACCEPT,		on_connect_accepted);
	librg_event_add(&ctx, LIBRG_CONNECTION_DISCONNECT,	on_disconnect);

	librg_event_add(&ctx, LIBRG_ENTITY_CREATE,			on_entity_create);
	librg_event_add(&ctx, LIBRG_ENTITY_UPDATE,			on_entity_update);
	librg_event_add(&ctx, LIBRG_ENTITY_REMOVE,			on_entity_remove);

	librg_event_add(&ctx, LIBRG_CLIENT_STREAMER_UPDATE, on_client_stream);

	librg_address_t addr;
	addr.host = szAddress;
	addr.port = iPort;

	gLog->Log("[CClientNetwork] Attempting to connect to %s:%d\n", addr.host, addr.port);
	librg_network_start(&ctx, addr);

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ClientConnectThread, NULL, 0, NULL);
}
