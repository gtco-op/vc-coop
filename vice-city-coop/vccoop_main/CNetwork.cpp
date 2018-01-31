#define LIBRG_IMPLEMENTATION
#define LIBRG_DEBUG
#include "main.h"

librg_ctx_t CNetwork::ctx;
librg_entity_t * CNetwork::local_player;
std::vector<CPed*> CNetwork::players;

bool CNetwork::client_running;
bool CNetwork::client_connected;
bool CNetwork::connected;

CNetwork::CNetwork()
{
	ctx = { 0 };
	local_player = nullptr;
	
	client_connected = false;
	client_connected = false;
	connected = false;
}
CNetwork::~CNetwork()
{

}
void CNetwork::on_connect_request(librg_event_t *event) {
	gLog->Log("[VC CO-OP][CLIENT] Requesting to connect\n");
}
void CNetwork::on_connect_accepted(librg_event_t *event) {
	gLog->Log("[VC CO-OP][CLIENT] Connection Accepted\n");

	connected = true;
	if (event->entity)
	{
		local_player = event->entity;
	}
}
void CNetwork::on_connect_refused(librg_event_t *event) {
	gLog->Log("[VC CO-OP][CLIENT] Connection Refused\n");
}
void CNetwork::on_entity_create(librg_event_t * event) {

	gLog->Log("Creating entity..\n");
	zplm_vec3_t position = event->entity->position;
	CStreaming::RequestModel(0, 0);
	CPed *ped = new CCivilianPed(ePedType::PEDTYPE_CIVMALE, 0);
	CWorld::Add(ped);
	CVector posn = CVector::CVector(position.x, position.y, position.z);
	gLog->Log("[VC CO-OP][CLIENT] Spawned player %d\n", event->entity->id);
	ped->Teleport(posn);

	players.push_back(ped);
}
void CNetwork::on_entity_update(librg_event_t *event) {
	int entity_id = event->entity->id;

	for (auto iter : players)
	{
		iter->Teleport(CVector(event->entity->position.x, event->entity->position.y, event->entity->position.z));
	}
}
void CNetwork::ClientConnectThread()
{
	while (client_running) {
		if (connected && local_player)
		{
			local_player->position.x = FindPlayerPed()->GetPosition().x;
			local_player->position.y = FindPlayerPed()->GetPosition().y;
			local_player->position.z = FindPlayerPed()->GetPosition().z;
		}
		librg_tick(&ctx);
		zpl_sleep_ms(1);
	}

	librg_network_stop(&ctx);
	librg_free(&ctx);
}
void CNetwork::AttemptConnect()
{
	client_running = true;
	ctx.mode = LIBRG_MODE_CLIENT;
	librg_init(&ctx);

	librg_event_add(&ctx, LIBRG_CONNECTION_REQUEST, on_connect_request);
	librg_event_add(&ctx, LIBRG_CONNECTION_ACCEPT, on_connect_accepted);
	librg_event_add(&ctx, LIBRG_ENTITY_CREATE, on_entity_create);
	librg_event_add(&ctx, LIBRG_ENTITY_UPDATE, on_entity_update);

	librg_address_t addr;
	addr.host = VCCOOP_DEFAULT_SERVER_ADDRESS;
	addr.port = VCCOOP_DEFAULT_SERVER_PORT;
	librg_network_start(&ctx, addr);

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ClientConnectThread, NULL, 0, NULL);
}
