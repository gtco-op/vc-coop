#pragma once

static char Nickname[25] = { 0 };
static char IP[15]	= VCCOOP_DEFAULT_SERVER_ADDRESS;
static int Port		= VCCOOP_DEFAULT_SERVER_PORT;

class CClientNetwork : public CRichPresence
{
public:
	CClientNetwork();
	~CClientNetwork();

	char	*ServerAddress;
	int		ServerPort;

	static librg_address_t addr;

	static librg_ctx_t ctx;
	static librg_entity_t * local_player;
	
	static bool client_running;
	static bool client_connected;
	static bool connected;	

	static CEntity*			GetEntityFromNetworkID(int id);
	static CClientEntity*	GetNetworkEntityFromNetworkID(int id);
	static int				GetNetworkIDFromEntity(CEntity* ent);

	static void SetReadyToSpawn(bool bReady);

	static void ReceiveSPDUpdate(librg_message_t* msg);

	static void ClientStartMissionScript(librg_message_t* msg);
	static void ClientReceiveMessage(librg_message_t* msg);
	static void ClientReceiveScript(librg_message_t* msg);
	static void ClientSpawnAllowed(librg_message_t* msg);
	static void PlayerSpawnEvent(librg_message_t* msg);

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
	static void ClientDisconnect(librg_message_t* msg);
	static void ClientConnect(librg_message_t* msg);
	static void BulletSyncEvent(librg_message_t* msg);

	std::vector<CClientEntity*> networkEntities;
};