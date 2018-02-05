#pragma once

static char Nickname[25] = { 0 };
static char IP[]	= VCCOOP_DEFAULT_SERVER_ADDRESS;
static int Port		= VCCOOP_DEFAULT_SERVER_PORT;
class CClientNetwork
{
public:
	CClientNetwork();
	~CClientNetwork();

	static librg_ctx_t ctx;
	static librg_entity_t * local_player;
	static std::vector<std::pair<CPed*, int>> players;
	
	static bool client_running;
	static bool client_connected;
	static bool connected;	

	static CEntity*		GetEntityFromNetworkID(int id);
	static int			GetNetworkIDFromEntity(CEntity* ent);

	static void on_connect_request(librg_event_t *event);
	static void on_connect_accepted(librg_event_t *event);
	static void on_connect_refused(librg_event_t *event);
	static void on_disconnect(librg_event_t *event);
	
	static void on_entity_remove(librg_event_t *event);
	static void on_entity_create(librg_event_t * event);
	static void on_entity_update(librg_event_t *event);
	static void on_client_stream(librg_event_t *event);

	static void ClientConnectThread();
	static void StopClientThread();
	static void AttemptConnect(char* szAddress, int iPort);
};