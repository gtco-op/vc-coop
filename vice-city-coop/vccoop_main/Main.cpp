#define LIBRG_IMPLEMENTATION
#define LIBRG_DEBUG
#include "librg\librg.h"
#include "includes.h"

using namespace plugin;

class CNetworking {
public:
	CNetworking(CLogger*);
	~CNetworking();

	static void on_connect_accepted(librg_event_t *event);
	static void on_connect_request(librg_event_t *event);
	static void on_connect_refused(librg_event_t *event);
	static void on_entity_create(librg_event_t * event);
	static void on_entity_update(librg_event_t *event);

	void AttemptConnect();
	static void ClientConnectThread();

	librg_address_t address;
	static librg_ctx_t ctx;

	static librg_entity_t *local_player;
	static std::vector<CPed*> players;

	static bool connected;
	static bool client_running;

	static CLogger* l;
};

CNetworking::CNetworking(CLogger* Log)
{
	l = Log;
	address = { 23546, "127.0.0.1" };
	ctx = { 0 };
	l->Log("[CNetworking] Initialized\n");
}
CNetworking::~CNetworking()
{

}
void CNetworking::on_connect_request(librg_event_t *event) {
	l->Log("[CNetworking][CLIENT] Requesting to connect\n");
}
void CNetworking::on_connect_accepted(librg_event_t *event) {
	CStreaming::LoadAllRequestedModels(0);
	CPed *playerped = FindPlayerPed();
	playerped->m_fHealth = 200; // just values , because if the player was his health is so low in the lobby
	playerped->m_fArmour = 100; //--^
	l->Log("[CNetworking][CLIENT] Connection Accepted\n");
	connected = true;

	if (event->entity)
	{
		local_player = event->entity;

	}
}
void CNetworking::on_connect_refused(librg_event_t *event) {
	l->Log("[CNetworking][CLIENT] Connection Refused\n");
}
void CNetworking::on_entity_create(librg_event_t * event) {

	l->Log("[CNetworking][CLIENT] Creating entity..\n");
	zplm_vec3_t position = event->entity->position;
	CStreaming::RequestModel(0, 0);
	CPed *ped = new CPlayerPed();
	CWorld::Add(ped);

	CVector posn = CVector::CVector(position.x, position.y, position.z);
	l->Log("[CNetworking][CLIENT] Spawned player %d\n", event->entity->id);
	ped->Teleport(posn);

	players.push_back(ped);
}
void CNetworking::on_entity_update(librg_event_t *event) {
	int entity_id = event->entity->id;
	for (auto iter : players)
	{
		iter->Teleport(CVector(event->entity->position.x, event->entity->position.y, event->entity->position.z));
	}
}

void CNetworking::ClientConnectThread()
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
void CNetworking::AttemptConnect()
{
	client_running = true;
	ctx.mode = LIBRG_MODE_CLIENT;
	librg_init(&ctx);

	librg_event_add(&ctx, LIBRG_CONNECTION_REQUEST, &CNetworking::on_connect_request);
	librg_event_add(&ctx, LIBRG_CONNECTION_ACCEPT, &CNetworking::on_connect_accepted);
	librg_event_add(&ctx, LIBRG_ENTITY_CREATE, &CNetworking::on_entity_create);
	librg_event_add(&ctx, LIBRG_ENTITY_UPDATE, &CNetworking::on_entity_update);

	librg_network_start(&ctx, address);

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ClientConnectThread, NULL, 0, NULL);
	l->Log("[CLIENT] Client thread created\n");
}

CLogger* CNetworking::l;
std::vector<CPed*> CNetworking::players;
librg_ctx_t CNetworking::ctx;
librg_entity_t* CNetworking::local_player;
bool CNetworking::connected;
bool CNetworking::client_running;

CLogger* l;
CGUI* g;
CNetworking* n;

class vccoop {
public:

	vccoop()	{
		l = new CLogger();
		
#ifdef VCCOOP_DEBUG
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
#endif
		CMemoryEdits m(l);
		m.Patch();

		n = new CNetworking(l);
		g = new CGUI(l, n);
		
		Events::drawingEvent += [] {
			g->DrawEvent();
		};

		static int keyPressTimeConnect = 0;
		static int keyPressTimeDrawGUI = 0;
		Events::gameProcessEvent += [] {
			if (KeyPressed(VK_F8) && CTimer::m_snTimeInMilliseconds - keyPressTimeDrawGUI > 1000) {
				keyPressTimeDrawGUI = CTimer::m_snTimeInMilliseconds;
				
				g->ToggleGUI();
			}
			if (KeyPressed(VK_F9) && CTimer::m_snTimeInMilliseconds - keyPressTimeConnect > 1000) {
				keyPressTimeConnect = CTimer::m_snTimeInMilliseconds;

				n->AttemptConnect();
			}
		};
	}
	~vccoop()	{
		l->Log("Shutting down\n");
	}
} vccoop;