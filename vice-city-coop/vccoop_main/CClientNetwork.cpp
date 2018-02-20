#define LIBRG_IMPLEMENTATION
#define LIBRG_DEBUG
#include "main.h"

librg_address_t						CClientNetwork::addr;
librg_ctx_t							CClientNetwork::ctx;
librg_entity_t *					CClientNetwork::local_player;

bool								CClientNetwork::client_connected;
bool								CClientNetwork::client_running;
bool								CClientNetwork::connected;


CClientNetwork::CClientNetwork()
{
	ctx = { 0 };
	local_player = nullptr;

	client_running = false;
	client_connected = false;
	connected = false;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		this->networkPlayers[i] = NULL;
	}
}
CClientNetwork::~CClientNetwork()
{
	gLog->Log("[CClientNetwork] CClientNetwork shutting down\n");
	this->StopClientThread();
}
void CClientNetwork::PlayerSpawnEvent(librg_message_t* msg)
{
	u32 playerid;
	librg_data_rptr(msg->data, &playerid, sizeof(u32));
	
	gNetwork->networkPlayers[playerid]->Respawn();

	gLog->Log("Respawning entity: %d\n", playerid);
}
void CClientNetwork::ClientReceiveMessage(librg_message_t* msg)
{
	char str[256];
	librg_data_rptr(msg->data, &str, sizeof(str));

	gChat->AddChatMessage(str);
}
CEntity* CClientNetwork::GetEntityFromNetworkID(int id)
{
	return gNetwork->networkPlayers[id]->ped;
}
int CClientNetwork::GetNetworkIDFromEntity(CEntity* ent)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (!gNetwork->networkPlayers[i])continue;
		if (gNetwork->networkPlayers[i]->ped == ent)
		{
			return gNetwork->networkPlayers[i]->networkID;
		}
	}
	return -1;
}
void CClientNetwork::on_connect_request(librg_event_t *event) {
	gNetwork->SetReadyToSpawn(FALSE);
	
	char name[25];
	strcpy(name, gGame->Name.c_str());

	gLog->Log("[CClientNetwork] Connecting as %s\n", name);

	librg_data_wptr(event->data, (void*)&name, sizeof(name));
	librg_data_wu32(event->data, VCCOOP_DEFAULT_SERVER_SECRET);
}
void CClientNetwork::on_connect_accepted(librg_event_t *event) 
{
	gLog->Log("[CClientNetwork] Connection Accepted\n");

	local_player = event->entity;
	event->entity->user_data = new CClientPlayer(event->entity->id);
}
void CClientNetwork::on_connect_refused(librg_event_t *event) 
{
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
	while (client_running) 
	{
		librg_tick(&ctx);
		zpl_sleep_ms(1);
	}

	librg_network_stop(&ctx);
	librg_free(&ctx);
}
void CClientNetwork::on_entity_create(librg_event_t *event) 
{
	zplm_vec3_t position = event->entity->position;

	if (event->entity->type == VCOOP_PLAYER || event->entity->type == VCOOP_PED)
	{
		PlayerSyncData spd;
		CStreaming::RequestModel(7, 0);
		librg_data_rptr(event->data, &spd, sizeof(PlayerSyncData));

		if (event->entity->type == VCOOP_PLAYER) 
		{
			event->entity->user_data = new CClientPlayer(event->entity->id, 1);//needs to be changed later
			gNetwork->networkPlayers[event->entity->id] = (CClientPlayer*)event->entity->user_data;
		}
		else if (event->entity->type == VCOOP_PED) 
		{
			event->entity->user_data = new CClientPed(event->entity->id);
		}
	}
	else if (event->entity->type == VCOOP_VEHICLE)
	{
		//not done yet
	}
	gLog->Log("[CClientNetwork] Network entity %d initialized\n", event->entity->id);
}
void CClientNetwork::on_entity_update(librg_event_t *event) 
{
	if (event->entity->type == VCOOP_PLAYER)
	{
		PlayerSyncData spd;
		librg_data_rptr(event->data, &spd, sizeof(PlayerSyncData));

		auto player = (CClientPlayer *)event->entity->user_data;
		auto ped = player->ped;

		ped->Teleport(*(CVector *)&event->entity->position);

		player->SyncPlayer(spd);
	}
	else if(event->entity->type == VCOOP_PED)
	{
		PedSyncData spd;
		librg_data_rptr(event->data, &spd, sizeof(PedSyncData));

		auto pedestrian = (CClientPed *)event->entity->user_data;
		auto ped = pedestrian->ped;

		ped->Teleport(*(CVector *)&event->entity->position);

		pedestrian->SyncPed(spd);
	}
	else if (event->entity->type == VCOOP_VEHICLE)
	{
		auto veh = (CVehicle *)event->entity->user_data;
		veh->Teleport(*(CVector *)&event->entity->position);
	}
}
void CClientNetwork::on_client_stream(librg_event_t *event) 
{
	if (event->entity->type == VCOOP_PLAYER)
	{
		PlayerSyncData spd;

		auto player = (CClientPlayer *)event->entity->user_data;
		auto ped = player->ped;

		event->entity->position = *(zplm_vec3_t *)&ped->GetPosition();

		spd = player->BuildSyncData();

		librg_data_wptr(event->data, &spd, sizeof(PlayerSyncData));
	}
	else if(event->entity->type == VCOOP_PED)
	{
		PedSyncData spd;

		auto pedestrian = (CClientPed *)event->entity->user_data;
		auto ped = pedestrian->ped;

		event->entity->position = *(zplm_vec3_t *)&ped->GetPosition();

		spd = pedestrian->BuildSyncData();

		spd.gameTimer = 0;

		librg_data_wptr(event->data, &spd, sizeof(PedSyncData));
	}
	else if (event->entity->type == VCOOP_VEHICLE)
	{
		//not done yet
		CVehicle *veh = (CVehicle *)event->entity->user_data;
		event->entity->position = *(zplm_vec3_t *)&veh->GetPosition();

		//SPlayerData spd;
		//spd.Health = veh->m_fHealth;
		//librg_data_wptr(event->data, &spd, sizeof(SPlayerData));
	}
}
void CClientNetwork::on_entity_remove(librg_event_t *event) 
{
	if (event->entity->type == VCOOP_PLAYER)
	{
		gLog->Log("Removing player %d due to inactivity", event->entity->id);
		auto player = (CClientPlayer *)event->entity->user_data;
		delete player;
		gNetwork->networkPlayers[event->entity->id] = NULL;
	}
	else if (event->entity->type == VCOOP_PED)
	{
		auto pedestrian = (CClientPed *)event->entity->user_data;
		delete pedestrian;
	}
}

void CClientNetwork::on_disconnect(librg_event_t *event) 
{
	StopClientThread();

	gLog->Log("[CClientNetwork] Disconnected.\n");
}
void CClientNetwork::ClientReceiveScript(librg_message_t* msg)
{
	// read all of the data..
	char* scriptData = new char[msg->data->capacity-2];
	librg_data_rptr(msg->data, scriptData, msg->data->capacity-2);

	// copy the first four bytes to obtain the script size..
	int scriptSize = 0;
	char buf[4];
	memcpy(buf, scriptData, 4);
	scriptSize = atoi(buf);

	// remove the first four bytes, scriptData now contains just the script
	memcpy(scriptData, scriptData + 4, scriptSize);
	
#ifdef VCCOOP_DEBUG
	gLog->Log("[CClientNetwork] Received script with size: %d\n", scriptSize);
#endif

	CLua* lua = new CLua();
	lua->SetLuaStatus(TRUE);
	lua->mainScript = new char[scriptSize];
	lua->mainScriptSize = scriptSize;
	
	memcpy(lua->mainScript, scriptData, scriptSize);

	lua->CreateLuaThread();

	// Set spawn status to true..
	gNetwork->SetReadyToSpawn(TRUE);
}
void CClientNetwork::SetReadyToSpawn(bool bReady)
{
	if (bReady)
	{
		connected				= true;
		gRender->bConnecting	= false;
		gRender->bGUI			= false;
		gRender->bAboutWindow	= false;
		gGame->RestoreCamera();
		gGame->SetCameraBehindPlayer();
		gGame->EnableHUD();
	}
	else
	{
		connected				= false;
		gRender->bConnecting	= false;
		gRender->bGUI			= true;
		gGame->DisableHUD();
	}
}
void CClientNetwork::AttemptConnect(char* szAddress, int iPort)
{
	client_running = true;
	ctx.mode = LIBRG_MODE_CLIENT;
	librg_init(&ctx);

	librg_event_add(&ctx, LIBRG_CONNECTION_REQUEST, on_connect_request);
	librg_event_add(&ctx, LIBRG_CONNECTION_ACCEPT, on_connect_accepted);
	librg_event_add(&ctx, LIBRG_CONNECTION_DISCONNECT, on_disconnect);

	librg_event_add(&ctx, LIBRG_ENTITY_CREATE, on_entity_create);
	librg_event_add(&ctx, LIBRG_ENTITY_UPDATE, on_entity_update);
	librg_event_add(&ctx, LIBRG_ENTITY_REMOVE, on_entity_remove);

	librg_event_add(&ctx, LIBRG_CLIENT_STREAMER_UPDATE, on_client_stream);

	librg_network_add(&ctx, VCOOP_RECEIVE_MESSAGE, ClientReceiveMessage);
	librg_network_add(&ctx, VCOOP_RESPAWN_AFTER_DEATH, PlayerSpawnEvent);
	librg_network_add(&ctx, VCOOP_GET_LUA_SCRIPT, ClientReceiveScript);

	addr.host = szAddress;
	addr.port = iPort;

	gLog->Log("[CClientNetwork] Attempting to connect to %s:%d\n", addr.host, addr.port);
	librg_network_start(&ctx, addr);

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ClientConnectThread, NULL, 0, NULL);
}
