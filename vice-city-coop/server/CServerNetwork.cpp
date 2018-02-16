#include "server.h"

/* Server Configuration */
int								CServerNetwork::ServerPort;
int								CServerNetwork::ServerSecret;

HANDLE							CServerNetwork::server_handle;
bool							CServerNetwork::server_running, CServerNetwork::console_active;
librg_ctx_t						CServerNetwork::ctx;
std::vector<librg_entity_t*>	CServerNetwork::playerEntities;
std::vector<librg_entity_t*>	CServerNetwork::otherEntities;

std::vector<std::pair<char*, int>> dataArray;

CServerNetwork::CServerNetwork()
{
	ctx = { 0 };
	server_running = true;
	server_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)server_thread, NULL, 0, NULL);
}
CServerNetwork::~CServerNetwork()
{

}

void CServerNetwork::PlayerDeathEvent(librg_message_t *msg)
{
	deathData dData;
	librg_data_rptr(msg->data, &dData, sizeof(deathData));

	librg_entity_t *player = librg_entity_find(msg->ctx, msg->peer);
	
	char msg1[256];
	
	sprintf(msg1, "[CServerNetwork] Player %d is killed by entity %d with weapon %d\n", player->id, dData.killer, dData.weapon);
	librg_message_send_except(&ctx, VCOOP_RECEIVE_MESSAGE, msg->peer, &msg1, sizeof(msg1));

	gLog->Log(msg1);
}

void CServerNetwork::PlayerSpawnEvent(librg_message_t *msg)
{
	librg_entity_t *player = librg_entity_find(msg->ctx, msg->peer);

	librg_message_send_except(&ctx, VCOOP_RESPAWN_AFTER_DEATH, msg->peer, &player->id, sizeof(u32));
}

void CServerNetwork::ClientSendMessage(librg_message_t *msg)
{
	char msg1[256];
	librg_data_rptr(msg->data, &msg1, sizeof(msg1));

	librg_message_send_except(&ctx, VCOOP_RECEIVE_MESSAGE, msg->peer, &msg1, sizeof(msg1));
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
}

void CServerNetwork::VehCreateEvent(librg_message_t *msg)
{
	librg_entity_t* entity = librg_entity_create(&ctx, VCOOP_VEHICLE);
	librg_entity_control_set(&ctx, entity->id, msg->peer);
	
	// spawn a vehicle at player's position
	entity->position = librg_entity_find(msg->ctx, msg->peer)->position;

	gLog->Log("[CServerNetwork] Vehicle created. (%d)\n", entity->id);
}
void CServerNetwork::on_connect_request(librg_event_t *event) {
	// Player Name
	char name[25];
	librg_data_rptr(event->data, (void*)&name, 25);

	u32 secret = librg_data_ru32(event->data);
	gLog->Log("[CServerNetwork][CLIENT REQUEST] Network entity with name '%s' is requesting to connect\n", name);

	if (strlen(name) < 0 || secret != gServerNetwork->ServerSecret) {
		gLog->Log("[CServerNetwork] Rejected event from network entity\n");
		librg_event_reject(event);
	}
}
std::pair<char*,int> CServerNetwork::LoadScript(std::string filename)
{
	// load the data into memory
	std::string path = GetExecutablePath();
	path.append("\\scripts\\");
	path.append(filename);

	std::ifstream t(path);
	std::stringstream buffer;
	buffer << t.rdbuf();

	// allocate data
	std::string dataData = buffer.str();
	int dataLen = buffer.str().size() + 1;

#ifdef VCCOOP_LIBRG_DEBUG
	gLog->Log("[CServerNetwork] Loaded data with size %d\n", dataLen);
	gLog->Log("[CServerNetwork] Total data Size: %d\n", sizeof(dataLen) + (buffer.str().size() + 1));
#endif

	// construct the dataset
	char* databuf = new char[sizeof(dataLen) + (buffer.str().size() + 1)];
	memset(databuf, 0, sizeof(dataLen) + (buffer.str().size() + 1));
	sprintf(databuf, "%d%s", dataLen, dataData.c_str());

	return std::pair<char*, int>(databuf, sizeof(dataLen) + (buffer.str().size() + 1));
}
void CServerNetwork::on_connect_accepted(librg_event_t *event) {
	// initialize sync data and set entity control of the new client
	event->entity->user_data = new PlayerSyncData();
	librg_entity_control_set(event->ctx, event->entity->id, event->entity->client_peer);

	// push back the entity into the entities vector
	playerEntities.push_back(event->entity);
	gLog->Log("[CServerNetwork][CLIENT CONNECTION] Network entity %d connected\n", event->entity->id);

	// send every script/data to the client
	for (auto it : dataArray)
		librg_message_send_to(&ctx, VCOOP_GET_LUA_SCRIPT, event->peer, it.first, it.second);
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
}
void CServerNetwork::on_entity_update(librg_event_t *event) 
{
	if (event->entity->type == VCOOP_PLAYER)
	{
		librg_data_wptr(event->data, event->entity->user_data, sizeof(PlayerSyncData));
	}
	else if (event->entity->type == VCOOP_PED)
	{
		librg_data_wptr(event->data, event->entity->user_data, sizeof(PedSyncData));
	}
}
void CServerNetwork::on_stream_update(librg_event_t *event) 
{
	if (event->entity->type == VCOOP_PLAYER)
	{
		librg_data_rptr(event->data, event->entity->user_data, sizeof(PlayerSyncData));
	}
	else if (event->entity->type == VCOOP_PED)
	{
		librg_data_rptr(event->data, event->entity->user_data, sizeof(PedSyncData));
	}
}

void CServerNetwork::on_disconnect(librg_event_t* event){
	auto tmp = std::find(playerEntities.begin(), playerEntities.end(), event->entity);
	if (tmp != playerEntities.end())	{
		playerEntities.erase(tmp);
		delete event->entity->user_data;
	}	
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
	ctx.world_size		= zplm_vec3(5000.0f, 5000.0f, 5000.0f);
	ctx.mode			= LIBRG_MODE_SERVER;
	ctx.tick_delay		= 32;
	ctx.max_connections = 2000;
	ctx.max_entities	= 2000;
	librg_init(&ctx);
	
	librg_event_add(&ctx,	LIBRG_CONNECTION_REQUEST,		on_connect_request);
	librg_event_add(&ctx,	LIBRG_CONNECTION_ACCEPT,		on_connect_accepted);
	librg_event_add(&ctx,	LIBRG_CONNECTION_DISCONNECT,	on_disconnect);

	librg_event_add(&ctx,	LIBRG_ENTITY_CREATE,			on_creating_entity);
	librg_event_add(&ctx,	LIBRG_ENTITY_UPDATE,			on_entity_update);
	librg_event_add(&ctx,	LIBRG_ENTITY_REMOVE,			on_disconnect);

	librg_network_add(&ctx, VCOOP_CREATE_PED,				PedCreateEvent);
	librg_network_add(&ctx, VCOOP_CREATE_VEHICLE,			VehCreateEvent);
	librg_network_add(&ctx, VCOOP_SEND_MESSAGE,				ClientSendMessage);
	librg_network_add(&ctx, VCOOP_PED_IS_DEAD,				PlayerDeathEvent);
	librg_network_add(&ctx, VCOOP_RESPAWN_AFTER_DEATH,		PlayerSpawnEvent);


	librg_event_add(&ctx,	LIBRG_CLIENT_STREAMER_UPDATE, on_stream_update);

	gLog->Log("[CServerNetwork] Server thread initialized\n");

	librg_address_t addr = { ServerPort };
	librg_network_start(&ctx, addr);
	gLog->Log("[CServerNetwork] Server starting on port %d\n", addr.port);

#ifndef VCCOOP_VERBOSE_LOG	
	zpl_timer_t *tick_timer = zpl_timer_add(ctx.timers);
	tick_timer->user_data = (void *)&ctx; /* provide ctx as a argument to timer */
	zpl_timer_set(tick_timer, 1000 * 1000, -1, measure);
	zpl_timer_start(tick_timer, 1000);
#endif

	gLog->Log("[CServerNetwork] Loading main client script into memory..\n");
	dataArray.push_back(LoadScript("client.lua"));
	gLog->Log("[CServerNetwork] Script processed.\n");

	while (server_running) {
		librg_tick(&ctx);
	}

	librg_network_stop(&ctx);
	librg_free(&ctx);

	server_running = false;
}