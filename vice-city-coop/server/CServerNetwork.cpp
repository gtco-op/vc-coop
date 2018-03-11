#include "server.h"

/* Server Configuration */
int								CServerNetwork::ServerPort;
int								CServerNetwork::ServerSecret;

HANDLE							CServerNetwork::server_handle;
bool							CServerNetwork::server_running, CServerNetwork::console_active;
librg_ctx_t						CServerNetwork::ctx;

std::vector<librg_entity_t*>	playerEntities;
char							playerNames[MAX_PLAYERS][25];
std::vector<librg_entity_t*>	otherEntities;

CLuaScript						*gGamemodeScript;

CServerNetwork::CServerNetwork()
{
	ctx = { 0 };
	server_running = true;
	server_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)server_thread, NULL, 0, NULL);
}
CServerNetwork::~CServerNetwork()
{

}
void CServerNetwork::BulletSyncEvent(librg_message_t *msg)
{
	bulletSyncData dData;
	librg_data_rptr(msg->data, &dData, sizeof(bulletSyncData));
	librg_message_send_except(&ctx, VCOOP_BULLET_SYNC, msg->peer, &dData, sizeof(bulletSyncData));
}

void CServerNetwork::PlayerDeathEvent(librg_message_t *msg)
{
	deathData dData;
	librg_data_rptr(msg->data, &dData, sizeof(deathData));

	librg_entity_t * player = librg_entity_find(msg->ctx, msg->peer);
	char msg1[256];
	sprintf(msg1, "[CServerNetwork] Player %d is killed by entity %d with weapon %d\n", player->id, dData.killer, dData.weapon);
	librg_message_send_except(&ctx, VCOOP_RECEIVE_MESSAGE, msg->peer, &msg1, sizeof(msg1));
	gLog->Log(msg1);

	gGamemodeScript->Call("onPlayerDeath", "iii", player->id, dData.killer, dData.weapon);
}
void CServerNetwork::PlayerSpawnEvent(librg_message_t *msg)
{
	librg_entity_t * player = librg_entity_find(msg->ctx, msg->peer);	
	librg_message_send_except(&ctx, VCOOP_RESPAWN_AFTER_DEATH, msg->peer, &player->id, sizeof(u32));

	gGamemodeScript->Call("onPlayerRespawn", "i", player->id);
}
void CServerNetwork::ClientSendMessage(librg_message_t *msg)
{
	char msg1[256];
	librg_data_rptr(msg->data, &msg1, sizeof(msg1));

	librg_message_send_except(&ctx, VCOOP_RECEIVE_MESSAGE, msg->peer, &msg1, sizeof(msg1));

	gGamemodeScript->Call("onPlayerMessage", "is", librg_entity_find(msg->ctx, msg->peer)->id, msg1);
}
void CServerNetwork::PedCreateEvent(librg_message_t *msg)
{
	librg_entity_t* entity = librg_entity_create(&ctx, VCOOP_PED);
	librg_entity_control_set(&ctx, entity->id, msg->peer);
	
	// crate our custom data container for ped
	entity->user_data = new PedSyncData();

	// spawn a ped at player's position
	entity->position = librg_entity_find(msg->ctx, msg->peer)->position;

	otherEntities.push_back(entity);
	gLog->Log("[CServerNetwork] Ped created. (%d)\n", entity->id);

	gGamemodeScript->Call("onPedCreated", "i", librg_entity_find(msg->ctx, msg->peer)->id);
}
void CServerNetwork::VehCreateEvent(librg_message_t *msg)
{
	librg_entity_t* entity = librg_entity_create(&ctx, VCOOP_VEHICLE);
	librg_entity_control_set(&ctx, entity->id, msg->peer);
	
	entity->user_data = new VehicleSyncData();
	((VehicleSyncData*)entity->user_data)->driver = -1;
	((VehicleSyncData*)entity->user_data)->vehicleID = -1;

	// spawn a vehicle at player's position
	entity->position = librg_entity_find(msg->ctx, msg->peer)->position;

	gLog->Log("[CServerNetwork] Vehicle created. (%d)\n", entity->id);

	gGamemodeScript->Call("onVehCreated", "i", librg_entity_find(msg->ctx, msg->peer)->id);
}
void CServerNetwork::HandShakeIsDone(librg_message_t *msg)
{
	char name[25];
	librg_data_rptr(msg->data, (void*)&name, 25);
	librg_entity_t * entity = librg_entity_find(msg->ctx, msg->peer);
	strcpy(playerNames[entity->id], name);
	
	//inform everyone we are connected
	connectData cData;
	sprintf(cData.name, playerNames[entity->id]);
	cData.playerId = entity->id;
	librg_message_send_except(msg->ctx, VCOOP_CONNECT, entity->client_peer, &cData, sizeof(connectData));

	gLog->Log("[CServerNetwork] Informing everyone about the connection of %s\n", name);

	gGamemodeScript->Call("onPlayerConnect", "i", entity->id);

	//loop trough connected playera and send it to this guy
	for (auto it : playerEntities)
	{
		if (it->id != entity->id)
		{
			sprintf(cData.name, playerNames[it->id]);
			cData.playerId = it->id;
			librg_message_send_to(msg->ctx, VCOOP_CONNECT, entity->client_peer, &cData, sizeof(connectData));
		}
	}
}
void CServerNetwork::on_connect_request(librg_event_t *event) 
{
	// Player Name
	char name[25];
	librg_data_rptr(event->data, (void*)&name, 25);

	u32 secret = librg_data_ru32(event->data);
	gLog->Log("[CServerNetwork][CLIENT REQUEST] Network entity with name %s is requesting to connect\n", name);

	if (secret != gServerNetwork->ServerSecret) 
	{
		gLog->Log("[CServerNetwork] Rejected event from network entity\n");
		librg_event_reject(event);
	}
}
void CServerNetwork::on_connect_accepted(librg_event_t *event) 
{
	// initialize sync data and set entity control of the new client
	event->entity->user_data = new PlayerSyncData();
	librg_entity_control_set(event->ctx, event->entity->id, event->entity->client_peer);

	// push back the entity into the entities vector
	playerEntities.push_back(event->entity);
	gLog->Log("[CServerNetwork][CLIENT CONNECTION] Network entity %d connected\n", event->entity->id);

	// send every script/data to the client
	for (auto it : gDataMgr->GetItems()) {

		if (it)
		{
			if (it->GetType() == TYPE_CLIENT_SCRIPT)
			{
				librg_message_send_to(&ctx, VCOOP_GET_LUA_SCRIPT, event->peer, it->GetData(), it->GetSize());
			}
		}
	}

	librg_message_send_to(&ctx, VCOOP_SPAWN_ALLOWED, event->peer, 0, 0);
}

void CServerNetwork::on_creating_entity(librg_event_t *event) 
{
	if (event->entity->type == VCOOP_PLAYER) 
	{
		librg_data_wptr(event->data, event->entity->user_data, sizeof(PlayerSyncData));
	}
	else if (event->entity->type == VCOOP_PED)
	{
		librg_data_wptr(event->data, event->entity->user_data, sizeof(PedSyncData));
	}
	else if (event->entity->type == VCOOP_VEHICLE)
	{
		librg_data_wptr(event->data, event->entity->user_data, sizeof(VehicleSyncData));
	}
}

void CServerNetwork::on_entity_update(librg_event_t *event)
{
	gGamemodeScript->Call("onEntityUpdate", "ii", event->entity->id, event->entity->type);

	if (event->entity->type == VCOOP_PLAYER)
	{
		librg_data_wptr(event->data, event->entity->user_data, sizeof(PlayerSyncData));
	}
	else if (event->entity->type == VCOOP_PED)
	{
		librg_data_wptr(event->data, event->entity->user_data, sizeof(PedSyncData));
	}
	else if (event->entity->type == VCOOP_VEHICLE)
	{
		librg_data_wptr(event->data, event->entity->user_data, sizeof(VehicleSyncData));
		if (reinterpret_cast<VehicleSyncData*>(event->entity->user_data)->driver == -1)
		{
			librg_peer_t * peer = librg_entity_control_get(event->ctx, event->entity->id);
			if (!peer)librg_entity_control_set(event->ctx, event->entity->id, event->peer);
		}
	}
}

void CServerNetwork::on_entity_remove(librg_event_t *event) //entity streamed out for entity
{
	gGamemodeScript->Call("onEntityRemove", "ii", librg_entity_find(&ctx, event->peer)->id, event->entity->type);

	if (event->entity->type == VCOOP_PED)
	{
		librg_peer_t * owner = librg_entity_control_get(event->ctx, event->entity->id);
		if (event->peer == owner)
		{
			gLog->Log("Destroying ped or looking for a new owner\n");
			librg_entity_id *entities;
			usize amount = librg_entity_query(event->ctx, event->entity->id, &entities);

			for (int i = 0; i < amount; i++)
			{
				librg_entity_t *entity = librg_entity_fetch(event->ctx, entities[i]);
				if (entity->type == VCOOP_PLAYER)
				{
					librg_entity_control_set(event->ctx, event->entity->id, entity->client_peer); 
					return;
				}
			}
			librg_entity_destroy(event->ctx, event->entity->id);
		}
	}
	else if (event->entity->type == VCOOP_VEHICLE)
	{
		if(librg_entity_control_get(event->ctx, event->entity->id) == event->peer)librg_entity_control_remove(event->ctx, event->entity->id);
	}
}

void CServerNetwork::on_stream_update(librg_event_t *event) 
{
	gGamemodeScript->Call("onStreamUpdate", "ii", event->entity->id, event->entity->type);

	if (event->entity->type == VCOOP_PLAYER)
	{
		librg_data_rptr(event->data, event->entity->user_data, sizeof(PlayerSyncData));
	}
	else if (event->entity->type == VCOOP_PED)
	{
		librg_data_rptr(event->data, event->entity->user_data, sizeof(PedSyncData));
	}
	else if (event->entity->type == VCOOP_VEHICLE)
	{
		librg_data_rptr(event->data, event->entity->user_data, sizeof(VehicleSyncData));

		u32 playerid = reinterpret_cast<VehicleSyncData*>(event->entity->user_data)->driver;
		if (playerid != -1)
		{
			if (librg_entity_fetch(event->ctx, playerid) == nullptr)
				return;

			if (librg_entity_fetch(event->ctx, playerid)->client_peer != nullptr)
			{
				librg_peer_t * owner = librg_entity_control_get(event->ctx, event->entity->id);
				librg_peer_t * driver = librg_entity_fetch(event->ctx, playerid)->client_peer;
				if (owner != driver)librg_entity_control_set(event->ctx, event->entity->id, driver);
			}
		}
	}
}

void CServerNetwork::on_disconnect(librg_event_t* event)
{
	gGamemodeScript->Call("onPlayerDisconnect", "i", event->entity->id);
	
	librg_entity_id *entities;
	usize amount = librg_entity_query(event->ctx, event->entity->id, &entities);

	for (int i = 0; i < amount; i++)
	{
		librg_entity_t *entity = librg_entity_fetch(event->ctx, entities[i]);
		if (entity->type == VCOOP_PED)
		{
			librg_peer_t * owner = librg_entity_control_get(event->ctx, entity->id);

			if (event->entity->client_peer == owner)
			{
				gLog->Log("[CServerNetwork] Destroying ped or looking for a new owner\n");
				librg_entity_id *entities2;
				usize amount2 = librg_entity_query(event->ctx, entity->id, &entities2);

				for (int z = 0; z < amount; z++)
				{
					librg_entity_t *entity2 = librg_entity_fetch(event->ctx, entities2[z]);
					if (entity2->type == VCOOP_PLAYER)
					{
						librg_entity_control_set(event->ctx, event->entity->id, entity2->client_peer);
						break;
					}
				}
				librg_entity_destroy(event->ctx, entity->id);
			}
		}
	}

	librg_entity_control_remove(event->ctx, event->entity->id);

	auto tmp = std::find(playerEntities.begin(), playerEntities.end(), event->entity);
	if (tmp != playerEntities.end())	
	{
		playerEntities.erase(tmp);
		delete event->entity->user_data;
	}	

	librg_message_send_except(&ctx, VCOOP_DISCONNECT, event->peer, &event->entity->id, sizeof(u32));

	gLog->Log("[ID#%d] Disconnected from server.\n", event->entity->id);
}

void CServerNetwork::measure(void *userptr) {
#ifndef VCCOOP_VERBOSE_LOG
	system("CLS");
#endif

	librg_ctx_t *ctx = (librg_ctx_t *)userptr;

	if (!ctx || !ctx->network.host) return;

	static u32 lastdl = 0;
	static u32 lastup = 0;

	f32 dl = (ctx->network.host->totalReceivedData - lastdl) * 8.0f / (1000.0f * 1000); // mbps
	f32 up = (ctx->network.host->totalSentData - lastup) * 8.0f / (1000.0f * 1000); // mbps

	lastdl = ctx->network.host->totalReceivedData;
	lastup = ctx->network.host->totalSentData;

#ifndef VCCOOP_VERBOSE_LOG
	std::string buf("[");
	buf.append(time_stamp(LOGGER_TIME_FORMAT));
	buf.append("][" VCCOOP_NAME "][CServerNetwork]");
	printf("%s Server Port: %d | Players: %d/2000 | Entities: %d/2000\n%s took %f ms. Used bandwidth D/U: (%f / %f) mbps.\n", buf.c_str(), ServerPort, playerEntities.size(), otherEntities.size(), buf.c_str(), ctx->last_update, dl, up);

	if (playerEntities.size() >= 1)	{
		printf("%s \tActive Players\n=======================================================================================================================\n", buf.c_str());
		for (auto it : playerEntities)		{
			printf("\t\t\t\t\t\tID#%d\t|\tPLAYER\n", it->id);
		}
	}
	if (otherEntities.size() >= 1) {
		printf("\n=======================================================================================================================\n\t\t\t\t\t    \tActive Entities\n=======================================================================================================================\n");
		for (auto it : otherEntities) {
			printf("\t\t\t\t\t\tID#%d\t|\tENTITY\n", it->id);
		}
	}
#endif
}
void CServerNetwork::server_thread()
{
	ctx.world_size			= zplm_vec3(5000.0f, 5000.0f, 5000.0f);
	ctx.mode				= LIBRG_MODE_SERVER;
	ctx.tick_delay			= 32;
	ctx.max_connections		= 2000;
	ctx.max_entities		= (MAX_PLAYERS + MAX_VEHICLES + MAX_PEDS);
	librg_init(&ctx);
	
	librg_event_add(&ctx,	LIBRG_CONNECTION_REQUEST,		on_connect_request);
	librg_event_add(&ctx,	LIBRG_CONNECTION_ACCEPT,		on_connect_accepted);
	librg_event_add(&ctx,	LIBRG_CONNECTION_DISCONNECT,	on_disconnect);

	librg_event_add(&ctx,	LIBRG_ENTITY_CREATE,			on_creating_entity);
	librg_event_add(&ctx,	LIBRG_ENTITY_UPDATE,			on_entity_update);
	librg_event_add(&ctx,	LIBRG_ENTITY_REMOVE,			on_entity_remove);

	librg_network_add(&ctx, VCOOP_CREATE_PED,				PedCreateEvent);
	librg_network_add(&ctx, VCOOP_CREATE_VEHICLE,			VehCreateEvent);
	librg_network_add(&ctx, VCOOP_SEND_MESSAGE,				ClientSendMessage);
	librg_network_add(&ctx, VCOOP_PED_IS_DEAD,				PlayerDeathEvent);
	librg_network_add(&ctx, VCOOP_RESPAWN_AFTER_DEATH,		PlayerSpawnEvent);
	librg_network_add(&ctx, VCOOP_CONNECT,					HandShakeIsDone);
	librg_network_add(&ctx, VCOOP_BULLET_SYNC,				BulletSyncEvent);

	librg_event_add(&ctx,	LIBRG_CLIENT_STREAMER_UPDATE,	on_stream_update);

	gLog->Log("[CServerNetwork][INFO] Server thread initialized\n");

	librg_address_t addr = { ServerPort };
	librg_network_start(&ctx, addr);
	gLog->Log("[CServerNetwork][INFO] Server started on port %d\n", addr.port);

#ifndef VCCOOP_VERBOSE_LOG	
	zpl_timer_t *tick_timer = zpl_timer_add(ctx.timers);
	tick_timer->user_data = (void *)&ctx; /* provide ctx as a argument to timer */
	zpl_timer_set(tick_timer, 1000 * 1000, -1, measure);
	zpl_timer_start(tick_timer, 1000);
#endif

	// Auto-detect all client scripts
	gDataMgr->LoadScripts();

	while (server_running) {
		if (!gGamemodeScript->GetServerStartStatus())		{
			gGamemodeScript->Call("onServerStart");
			gGamemodeScript->SetServerStartStatus(TRUE);
		}
		librg_tick(&ctx);
	}

	librg_network_stop(&ctx);
	librg_free(&ctx);

	server_running = false;
}