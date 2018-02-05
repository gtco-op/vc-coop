#include "server.h"

HANDLE							CServerNetwork::server_handle;
bool							CServerNetwork::server_running, CServerNetwork::console_active;
librg_ctx_t						CServerNetwork::ctx;
std::vector<librg_entity_t*>	CServerNetwork::entities;
librg_entity_t* tmpPed;

CServerNetwork::CServerNetwork()
{
	ctx = { 0 };
	server_running = true;
	server_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)server_thread, NULL, 0, NULL);
}
CServerNetwork::~CServerNetwork()
{

}
void CServerNetwork::PedCreateEvent(librg_message_t *msg)
{
	librg_entity_t* entity = librg_entity_create(&ctx, VCOOP_PED);
	librg_entity_control_set(&ctx, entity->id, msg->peer);

	// crate our custom data container for ped
	entity->user_data = new SPlayerData();

	// spawn a ped at player's position
	entity->position = librg_entity_find(msg->ctx, msg->peer)->position;

	gLog->Log("[CServerNetwork] Ped created. (%d)\n", entity->id);
}
void CServerNetwork::VehCreateEvent(librg_message_t *msg)
{
	librg_entity_t* entity = librg_entity_create(&ctx, VCOOP_VEHICLE);
	librg_entity_control_set(&ctx, entity->id, msg->peer);
	
	// spawn a ped at player's position
	entity->position = librg_entity_find(msg->ctx, msg->peer)->position;

	gLog->Log("[CServerNetwork] Vehicle created. (%d)\n", entity->id);
}
void CServerNetwork::on_connect_request(librg_event_t *event) {
	char name[25];
	librg_data_rptr(event->data, (void*)&name, 25);

	if (strlen(name) < 0) {
		librg_event_reject(event);
	}

	gLog->Log("[CServerNetwork][CLIENT REQUEST] Network entity with name '%s' is requesting to connect\n", name);
}
void CServerNetwork::on_connect_accepted(librg_event_t *event) {
	event->entity->user_data = new SPlayerData();
	librg_entity_control_set(event->ctx, event->entity->id, event->entity->client_peer);

	entities.push_back(event->entity);
	gLog->Log("[CServerNetwork][CLIENT CONNECTION] Network entity %d connected\n", event->entity->id);
}
void CServerNetwork::on_creating_entity(librg_event_t *event) {
	if (event->entity->type == VCOOP_PED || event->entity->type == VCOOP_PLAYER) {
		librg_data_wptr(event->data, event->entity->user_data, sizeof(SPlayerData));
	}
}
void CServerNetwork::on_entity_update(librg_event_t *event) {
	if (event->entity->type == VCOOP_PED || event->entity->type == VCOOP_PLAYER) {
		librg_data_wptr(event->data, event->entity->user_data, sizeof(SPlayerData));
	}
}
void CServerNetwork::on_stream_update(librg_event_t *event) {
	if (event->entity->type == VCOOP_PED || event->entity->type == VCOOP_PLAYER) {
		librg_data_rptr(event->data, event->entity->user_data, sizeof(SPlayerData));
	}
}

void CServerNetwork::on_disconnect(librg_event_t* event){
	auto tmp = std::find(entities.begin(), entities.end(), event->entity);
	if (tmp != entities.end())	{
		entities.erase(tmp);
		delete event->entity->user_data;
	}	
	gLog->Log("[ID#%d] Disconnected from server.\n", event->entity->id);
}

void measure(void *userptr) {
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
	printf("%s took %f ms. Used bandwidth D/U: (%f / %f) mbps. \n", buf.c_str(), ctx->last_update, dl, up);
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
	
	librg_event_add(&ctx, LIBRG_CONNECTION_REQUEST,		on_connect_request);
	librg_event_add(&ctx, LIBRG_CONNECTION_ACCEPT,		on_connect_accepted);
	librg_event_add(&ctx, LIBRG_CONNECTION_DISCONNECT,	on_disconnect);

	librg_event_add(&ctx, LIBRG_ENTITY_CREATE,			on_creating_entity);
	librg_event_add(&ctx, LIBRG_ENTITY_UPDATE,			on_entity_update);
	librg_event_add(&ctx, LIBRG_ENTITY_REMOVE,			on_disconnect);

	librg_network_add(&ctx, VCOOP_CREATE_PED,			PedCreateEvent);
	librg_network_add(&ctx, VCOOP_CREATE_VEHICLE,		VehCreateEvent);

	librg_event_add(&ctx, LIBRG_CLIENT_STREAMER_UPDATE, on_stream_update);

	gLog->Log("[CServerNetwork] Server thread initialized\n");

	librg_address_t addr = { 23546 };
	librg_network_start(&ctx, addr);
	gLog->Log("[CServerNetwork] Server starting on port %d\n", addr.port);

#ifndef VCCOOP_VERBOSE_LOG	
	zpl_timer_t *tick_timer = zpl_timer_add(ctx.timers);
	tick_timer->user_data = (void *)&ctx; /* provide ctx as a argument to timer */
	zpl_timer_set(tick_timer, 1000 * 1000, -1, measure);
	zpl_timer_start(tick_timer, 1000);
#endif

	while (server_running) {
		librg_tick(&ctx);
	}

	librg_network_stop(&ctx);
	librg_free(&ctx);

	server_running = false;
}