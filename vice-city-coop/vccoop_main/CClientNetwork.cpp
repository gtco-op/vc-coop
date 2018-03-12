#define LIBRG_IMPLEMENTATION
#define LIBRG_DEBUG
#include "main.h"

librg_address_t						CClientNetwork::addr;
librg_ctx_t							CClientNetwork::ctx;
librg_entity_t *					CClientNetwork::local_player;

bool								CClientNetwork::client_running;
bool								CClientNetwork::connected;

std::vector<librg_entity_t*>	playerEntities;
char							playerNames[MAX_PLAYERS][25];
std::vector<librg_entity_t*>	otherEntities;

CClientNetwork::CClientNetwork()
{
	ctx = { 0 };
	local_player = nullptr;

	client_running = false;
	connected = false;
}
CClientNetwork::~CClientNetwork()
{
	this->StopThread();
	gLog->Log("[CClientNetwork] CRichPresence shutting down\n");

	gLog->Log("[CClientNetwork] CClientNetwork shutting down\n");
	this->StopClientThread();
}
void CClientNetwork::PlayerSpawnEvent(librg_message_t* msg)
{
	u32 playerid;
	librg_data_rptr(msg->data, &playerid, sizeof(u32));

	CClientPlayer * player = (CClientPlayer*)GetNetworkEntityFromNetworkID(playerid);
	if (!player) return;

	player->Respawn();

	gLog->Log("[CClientNetwork] Respawning %s\n", player->szName);
}

void CClientNetwork::BulletSyncEvent(librg_message_t* msg)
{
	bulletSyncData bsData;
	librg_data_rptr(msg->data, &bsData, sizeof(bulletSyncData));

	CPed * shooterPlayer = (CPed*)GetEntityFromNetworkID(bsData.player); 
	if (shooterPlayer && shooterPlayer != LocalPlayer())
	{
		CEntity * hitEntity = NULL;
		if (bsData.targetEntityID != -1)hitEntity = GetEntityFromNetworkID(bsData.targetEntityID);
		//gLog->Log("Shooting bullet from player %d hitting entity %d\n", bsData.player, bsData.targetEntityID);
		//gLog->Log("Start vector: %f %f %f End vector: %f %f %f\n", bsData.start.x, bsData.start.y, bsData.start.z, bsData.end.x, bsData.end.y, bsData.end.z);
		//gLog->Log("surfacetypeA: %d | 2d vec: %f %f\n", bsData.colPoint.m_nSurfaceTypeA, bsData.ahead.x, bsData.ahead.y);
		CHooks::DoBulletImpact(&shooterPlayer->m_aWeapons[shooterPlayer->m_nWepSlot], shooterPlayer, hitEntity, &bsData.start, &bsData.end, &bsData.colPoint, bsData.ahead);
	}
}
void CClientNetwork::ClientReceiveMessage(librg_message_t* msg)
{
	char str[256];
	librg_data_rptr(msg->data, &str, sizeof(str));

	gChat->AddChatMessage(str);
}

CEntity* CClientNetwork::GetEntityFromNetworkID(int id)
{
	if (id < 0)return NULL;

	for (auto it = gNetwork->networkEntities.begin(); it != gNetwork->networkEntities.end(); ++it)
	{
		if ((*it)->networkID == id)
		{
			return (*it)->GetEntity();
		}
	}
	return NULL;
}

CClientEntity* CClientNetwork::GetNetworkEntityFromNetworkID(int id)
{
	if (id < 0)return NULL;

	for (auto it = gNetwork->networkEntities.begin(); it != gNetwork->networkEntities.end(); ++it)
	{
		if ((*it)->networkID == id)
		{
			return (*it);
		}
	}
	return NULL;
}

int CClientNetwork::GetNetworkIDFromEntity(CEntity* ent)
{
	for (auto it = gNetwork->networkEntities.begin(); it != gNetwork->networkEntities.end(); ++it)
	{
		if ((*it)->GetEntity() == ent)
		{
			return (*it)->networkID;
		}
	}
	return -1;
}

void CClientNetwork::on_connect_request(librg_event_t *event) 
{
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
	gNetwork->networkEntities.push_back((CClientPlayer*)event->entity->user_data);

	//Inform server about our name
	char name[25];
	strcpy(name, gGame->Name.c_str());
	librg_message_send_all(&gNetwork->ctx, VCOOP_CONNECT, &name, sizeof(name));
}
void CClientNetwork::on_connect_refused(librg_event_t *event) 
{
	gLog->Log("[CClientNetwork] Connection Refused\n");
}

void CClientNetwork::StopClientThread()
{
	client_running = false;
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

void CClientNetwork::ClientConnect(librg_message_t* msg)
{
	gLog->Log("[CClientNetwork] Connecting\n");
	connectData cData;
	librg_data_rptr(msg->data, &cData, sizeof(connectData));

	CClientPlayer * player = new CClientPlayer(cData.playerId, CHooks::FindFreeIDForPed());
	sprintf(player->szName, cData.name);
	gNetwork->networkEntities.push_back(player);

	gLog->Log("[CClientNetwork] %s connected\n", player->szName);
}

void CClientNetwork::on_entity_create(librg_event_t *event) 
{
	gLog->Log("creating entity\n");
	zplm_vec3_t position = event->entity->position;

	if (event->entity->type == VCOOP_PLAYER) 
	{
		PlayerSyncData spd;
		librg_data_rptr(event->data, &spd, sizeof(PlayerSyncData));

		CClientPlayer * player = (CClientPlayer*)gNetwork->GetNetworkEntityFromNetworkID(event->entity->id);

		if (!player)
		{
			gLog->Log("[CClientNetwork] Remote entity doesn't exists\n");
			return;
		}

		if (!event->entity->user_data) event->entity->user_data = player;

		player->StreamIn();
		gLog->Log("[CClientNetwork] %s streamed in\n", player->szName);
	}
	else if (event->entity->type == VCOOP_PED) 
	{
		PedSyncData spd;
		librg_data_rptr(event->data, &spd, sizeof(PedSyncData));

		event->entity->user_data = new CClientPed(event->entity->id);
		gNetwork->networkEntities.push_back((CClientPed*)event->entity->user_data);
	}
	else if (event->entity->type == VCOOP_VEHICLE)
	{
		VehicleSyncData spd;
		librg_data_rptr(event->data, &spd, sizeof(VehicleSyncData));

		CClientVehicle * vehicle = (CClientVehicle*)gNetwork->GetNetworkEntityFromNetworkID(event->entity->id);
		if (!vehicle)
		{
			gLog->Log("[CClientNetwork] Creating vehicle: %d\n", event->entity->id);

			vehicle = new CClientVehicle(event->entity->id, spd.modelID);
			gNetwork->networkEntities.push_back(vehicle);
			event->entity->user_data = vehicle;
		}
		else
		{
			gLog->Log("[CClientNetwork] Streaming vehicle: %d\n", event->entity->id);
			vehicle->StreamIn();
		}
	}
	gLog->Log("[CClientNetwork] Network entity %d initialized\n", event->entity->id); 
}

void CClientNetwork::on_entity_update(librg_event_t *event) 
{
	if (!event->entity->user_data)return; //dont do shit if its not initialized yet

	if (event->entity->type == VCOOP_PLAYER)
	{
		PlayerSyncData spd;
		librg_data_rptr(event->data, &spd, sizeof(PlayerSyncData));

		auto player = (CClientPlayer *)event->entity->user_data;
		auto ped = player->ped;

		if(!spd.isInVehicle)ped->Teleport(*(CVector *)&event->entity->position);

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
		VehicleSyncData spd;
		librg_data_rptr(event->data, &spd, sizeof(VehicleSyncData));

		auto vehicle = (CClientVehicle *)event->entity->user_data;
		auto veh = vehicle->veh;

		veh->m_placement.pos.x = event->entity->position.x;
		veh->m_placement.pos.y = event->entity->position.y;
		veh->m_placement.pos.z = event->entity->position.z;

		vehicle->SyncVehicle(spd);
	}
}
void CClientNetwork::on_client_stream(librg_event_t *event) 
{
	if (!event->entity->user_data)return;

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
	else if (event->entity->type == VCOOP_VEHICLE)//Unoccupied
	{
		VehicleSyncData spd;

		auto vehicle = (CClientVehicle *)event->entity->user_data;
		auto veh = vehicle->veh;

		//if (veh != LocalPlayer()->m_pVehicle)return;

		event->entity->position = *(zplm_vec3_t *)&veh->GetPosition();

		spd = vehicle->BuildSyncData();

		librg_data_wptr(event->data, &spd, sizeof(VehicleSyncData));
	}
}
void CClientNetwork::on_entity_remove(librg_event_t *event) 
{
	if (event->entity->type == VCOOP_PLAYER)
	{
		auto player = (CClientPlayer *)event->entity->user_data;
		if (player)
		{
			player->StreamOut();
			gLog->Log("%s streamed out\n", player->szName);
		}
	}
	else if (event->entity->type == VCOOP_PED)
	{
		auto pedestrian = (CClientPed *)event->entity->user_data;
		for (auto it = gNetwork->networkEntities.begin(); it != gNetwork->networkEntities.end(); ++it)
		{
			if (pedestrian == (*it))
			{
				gNetwork->networkEntities.erase(it);
				break;
			}
		}
		delete pedestrian;
		pedestrian = NULL;
		event->entity->user_data = NULL;
	}
	else if (event->entity->type = VCOOP_VEHICLE)
	{
		auto vehicle = (CClientVehicle*)event->entity->user_data;
		if(vehicle)	
		{
			vehicle->StreamOut();
			gLog->Log("Vehicle %d streamed out\n", event->entity->id);
		}
	}
}

void CClientNetwork::ClientDisconnect(librg_message_t* msg)
{
	u32 playerid;
	librg_data_rptr(msg->data, &playerid, sizeof(u32));
	librg_entity_t * entity = librg_entity_fetch(msg->ctx, playerid);
	
	CClientPlayer * player = (CClientPlayer *)entity->user_data;

	gLog->Log("[CClientNetwork] %s has disconnected.\n", player->szName);

	for (auto it = gNetwork->networkEntities.begin(); it != gNetwork->networkEntities.end(); ++it)
	{
		if (player == (*it))
		{
			gNetwork->networkEntities.erase(it);
			break;
		}
	}

	delete player;
	entity->user_data = NULL;
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
	double scriptSize = 0;
	char buf[sizeof(double)];
	memcpy(buf, scriptData, sizeof(double));
	scriptSize = atoi(buf);

	// remove the first four bytes, scriptData now contains just the script
	memcpy(scriptData, scriptData + sizeof(double), scriptSize);

	CLua* lua = new CLua();
	lua->SetLuaStatus(TRUE);
	lua->mainScript = new char[scriptSize];
	lua->mainScriptSize = scriptSize;
	
	memcpy(lua->mainScript, scriptData, scriptSize);
	
	lua->CreateLuaThread();
}
void CClientNetwork::ClientSpawnAllowed(librg_message_t* msg)
{
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
		CCameraStuff::RestoreCamera();
		CCameraStuff::SetCameraBehindPlayer();
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
	librg_network_add(&ctx, VCOOP_DISCONNECT, ClientDisconnect);
	librg_network_add(&ctx, VCOOP_CONNECT, ClientConnect);
	librg_network_add(&ctx, VCOOP_SPAWN_ALLOWED, ClientSpawnAllowed);
	librg_network_add(&ctx, VCOOP_BULLET_SYNC, BulletSyncEvent);

	addr.host = szAddress;
	addr.port = iPort;

	gNetwork->ServerAddress = szAddress;
	gNetwork->ServerPort = iPort;

	gLog->Log("[CClientNetwork] Attempting to connect to %s:%d\n", addr.host, addr.port);
	librg_network_start(&ctx, addr);

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ClientConnectThread, NULL, 0, NULL);
}
