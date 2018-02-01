#define LIBRG_IMPLEMENTATION
#define LIBRG_DEBUG
#include "main.h"

librg_ctx_t CNetwork::ctx;
librg_entity_t * CNetwork::local_player;
SPlayerData LocalPlayerInfo;
std::vector<CPed*> CNetwork::players;

bool CNetwork::client_running;
bool CNetwork::connected;

CNetwork::CNetwork()
{
	ctx = { 0 };
	local_player = nullptr;
	
	client_running = false;
	connected = false;
}
CNetwork::~CNetwork()
{
	this->StopClientThread();
}
void CNetwork::on_connect_request(librg_event_t *event) {
	gLog->Log("[VC CO-OP][CLIENT] Requesting to connect\n");
	librg_data_wu32(event->data, SERVER_SECRET);
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
void CNetwork::StopClientThread()
{
	client_running = false;
	connected = false;
}
void CNetwork::ClientConnectThread()
{
	while (client_running) {
		if (connected && local_player)
		{
			LocalPlayerInfo.iModelIndex = FindPlayerPed()->m_nModelIndex;
			LocalPlayerInfo.Health = FindPlayerPed()->m_fHealth;
			LocalPlayerInfo.Armour = FindPlayerPed()->m_fArmour;
			LocalPlayerInfo.iCurrentAnimID = FindPlayerPed()->m_dwAnimGroupId;
			LocalPlayerInfo.Rotation = FindPlayerPed()->m_fRotationCur;
			LocalPlayerInfo.CurrWep = FindPlayerPed()->m_aWeapons->m_nType;
			LocalPlayerInfo.WepModelIndex = FindPlayerPed()->m_dwWepModelID;
			LocalPlayerInfo.Ammo = FindPlayerPed()->m_aWeapons->m_nTotalAmmo;
			
		}
		librg_tick(&ctx);
		zpl_sleep_ms(1);
	}

	librg_network_stop(&ctx);
	librg_free(&ctx);
}
void CNetwork::on_client_stream(librg_event_t *event) {
	// position is sent here
	event->entity->position.x = FindPlayerPed()->GetPosition().x;
	event->entity->position.y = FindPlayerPed()->GetPosition().y;
	event->entity->position.z = FindPlayerPed()->GetPosition().z;

	// here we send all of the data above to the server..
	librg_data_wptr(event->data, &LocalPlayerInfo, sizeof(LocalPlayerInfo));
}
void CNetwork::AttemptConnect(char* szAddress, int iPort)
{
	client_running = true;
	ctx.mode = LIBRG_MODE_CLIENT;
	librg_init(&ctx);

	librg_event_add(&ctx, LIBRG_CONNECTION_REQUEST, on_connect_request);
	librg_event_add(&ctx, LIBRG_CONNECTION_ACCEPT, on_connect_accepted);
	librg_event_add(&ctx, LIBRG_ENTITY_CREATE, on_entity_create);
	librg_event_add(&ctx, LIBRG_ENTITY_UPDATE, on_entity_update);

	// Stream our data
	librg_event_add(&ctx, LIBRG_CLIENT_STREAMER_UPDATE, on_client_stream);

	librg_address_t addr;
	addr.host = szAddress;
	addr.port = iPort;

	gLog->Log("[CNetwork] Attempting to connect to %s:%d\n", addr.host, addr.port);
	librg_network_start(&ctx, addr);

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ClientConnectThread, NULL, 0, NULL);
}