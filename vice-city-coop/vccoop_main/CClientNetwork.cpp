#define LIBRG_IMPLEMENTATION
#define LIBRG_DEBUG
#include "main.h"

librg_address_t						CClientNetwork::addr;
librg_ctx_t							CClientNetwork::ctx;
librg_entity_t *					CClientNetwork::local_player;
std::vector<std::pair<CPed*, int>>	CClientNetwork::players;

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
}
CClientNetwork::~CClientNetwork()
{
	gLog->Log("[CClientNetwork] CClientNetwork shutting down\n");
	this->StopClientThread();
}
void CClientNetwork::ClientReceiveMessage(librg_message_t* msg)
{
	char str[256];
	librg_data_rptr(msg->data, &str, sizeof(str));

	gChat->AddChatMessage(str);
}
CEntity* CClientNetwork::GetEntityFromNetworkID(int id)
{
	CEntity* ent = NULL;
	vector <pair<CPed*, int>> ::iterator it;
	for (it = players.begin(); it != players.end(); it++) {
		if (it->second == id) {
			ent = it->first;
		}
	}
	return ent;
}
int CClientNetwork::GetNetworkIDFromEntity(CEntity* ent)
{
	int ID = 0;
	vector <pair<CPed*, int>> ::iterator it;
	for (it = players.begin(); it != players.end(); it++) {
		if (it->first == ent) {
			ID = it->second;
		}
	}
	return ID;
}
void CClientNetwork::on_connect_request(librg_event_t *event) {
	char name[25];
	strcpy(name, gGame->Name.c_str());

	gLog->Log("[CClientNetwork] Connecting as %s\n", name);

	librg_data_wptr(event->data, (void*)&name, sizeof(name));
	librg_data_wu32(event->data, VCCOOP_DEFAULT_SERVER_SECRET);
}
void CClientNetwork::on_connect_accepted(librg_event_t *event) 
{
	gLog->Log("[CClientNetwork] Connection Accepted\n");

	gRender->bConnecting = false;
	gRender->bGUI = false;
	gRender->bAboutWindow = false;
	
	connected = true;
	local_player = event->entity;
	event->entity->user_data = new CClientPlayer(event->entity->id);
	players.push_back(std::pair<CPed*, int>(LocalPlayer(), event->entity->id));
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
			players.push_back(std::pair<CPed*, int>(((CClientPlayer*)event->entity->user_data)->ped, event->entity->id));
		}
		else if (event->entity->type == VCOOP_PED) 
		{
			CPed *ped;
			ped = new CCivilianPed(PEDTYPE_CIVMALE, 7);
			ped->SetModelIndex(7);
			CWorld::Add(ped);
			ped->Teleport(CVector(position.x, position.y, position.z));
			ped->SetWanderPath((signed int)((long double)rand() * 0.000030517578 * 8.0));
			event->entity->user_data = ped;
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

		auto ped = (CPed *)event->entity->user_data;

		CTimer::m_snTimeInMilliseconds = spd.gameTimer;

		ped->Teleport(*(CVector *)&event->entity->position);

		ped->m_nModelIndex = spd.iModelIndex;
		ped->m_fHealth = spd.Health;
		ped->m_fRotationCur = spd.Rotation;
		ped->m_fArmour = spd.Armour;

		ped->m_fRotationDest = spd.m_fRotationDest;
		ped->m_fLookDirection = spd.m_fLookDirection;
		ped->m_vecAnimMoveDelta = spd.m_vecAnimMoveDelta;
		ped->m_dwAnimGroupId = spd.m_dwAnimGroupId;
		ped->m_vecMoveSpeed = spd.m_vecMoveSpeed;

		//Action sync
		ped->m_dwAction = spd.m_dwAction;
		ped->m_dwActionTimer = spd.m_dwActionTimer;
		ped->m_fActionX = spd.m_fActionX;
		ped->m_fActionY = spd.m_fActionY;

		//Objective sync
		ped->m_dwObjective = spd.m_dwObjective;
		ped->m_dwObjectiveTimer = spd.m_dwObjectiveTimer;
		ped->m_vecObjective = spd.m_vecObjective;
		ped->m_fObjectiveAngle = spd.m_fObjectiveAngle;
		ped->m_pObjectiveEntity = GetEntityFromNetworkID(spd.m_pObjectiveEntity);
		//ped->m_pObjectiveVehicle		= spd.m_pObjectiveVehicle;

		//Flee sync
		ped->m_dwFleeTimer = spd.m_dwFleeTimer;
		ped->m_fFleeFromPosX = spd.m_fFleeFromPosX;
		ped->m_fFleeFromPosY = spd.m_fFleeFromPosY;

		//Path sync
		ped->m_fPathNextNodeDir = spd.m_fPathNextNodeDir;
		ped->wRouteCurDir = spd.wRouteCurDir;
		ped->m_vecPathNextNode = spd.m_vecPathNextNode;
		ped->m_dwPathNodeTimer = spd.m_dwPathNodeTimer;
		ped->m_wCurPathNode = spd.m_wCurPathNode;
		ped->m_wPathNodes = spd.m_wPathNodes;

		for (int i = 0; i < 8; i++)
		{
			ped->m_aPathNodeStates[i] = spd.m_aPathNodeStates[i];
			//ped->m_apPathNodesStates[i] = spd.m_apPathNodesStates[i];
		}

		ped->m_dwPathNodeType = spd.m_dwPathNodeType;
		ped->m_nPathState = spd.m_nPathState;
		ped->m_pLastPathNode = spd.m_pLastPathNode;
		ped->m_pNextPathNode = spd.m_pNextPathNode;

		ped->SetMoveState((eMoveState)spd.m_dwMoveState);

		//Seek sync
		ped->m_fSeekExAngle = spd.m_fSeekExAngle;
		ped->m_vecSeekPosEx = spd.m_vecSeekPosEx;
		ped->m_vecOffsetSeek = spd.m_vecOffsetSeek;

		//Event sync
		ped->m_dwEventType = spd.m_dwEventType;
		ped->m_fAngleToEvent = spd.m_fAngleToEvent;
		ped->m_fEventOrThreatX = spd.m_fEventOrThreatX;
		ped->m_fEventOrThreatY = spd.m_fEventOrThreatY;
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
		auto ped = (CPed *)event->entity->user_data;
		event->entity->position = *(zplm_vec3_t *)&ped->GetPosition();

		spd.gameTimer = CTimer::m_snTimeInMilliseconds;

		spd.Health = ped->m_fHealth;
		spd.iCurrentAnimID = ped->m_dwAnimGroupId;
		spd.Armour = ped->m_fArmour;
		spd.iModelIndex = ped->m_nModelIndex;
		spd.Rotation = ped->m_fRotationCur;

		//Move sync
		spd.m_fRotationDest = ped->m_fRotationDest;
		spd.m_fLookDirection = ped->m_fLookDirection;
		spd.m_vecAnimMoveDelta = ped->m_vecAnimMoveDelta;
		spd.m_dwAnimGroupId = ped->m_dwAnimGroupId;
		spd.m_vecMoveSpeed = ped->m_vecMoveSpeed;

		//Action sync
		spd.m_dwAction = ped->m_dwAction;
		spd.m_dwActionTimer = ped->m_dwActionTimer;
		spd.m_fActionX = ped->m_fActionX;
		spd.m_fActionY = ped->m_fActionY;

		//Objective sync
		spd.m_dwObjective = ped->m_dwObjective;
		spd.m_dwObjectiveTimer = ped->m_dwObjectiveTimer;
		spd.m_vecObjective = ped->m_vecObjective;
		spd.m_fObjectiveAngle = ped->m_fObjectiveAngle;
		//spd.m_pObjectiveVehicle		= ped->m_pObjectiveVehicle;

		//Flee sync
		spd.m_dwFleeTimer = ped->m_dwFleeTimer;
		spd.m_fFleeFromPosX = ped->m_fFleeFromPosX;
		spd.m_fFleeFromPosY = ped->m_fFleeFromPosY;

		//Path sync
		spd.m_fPathNextNodeDir = ped->m_fPathNextNodeDir;
		spd.wRouteCurDir = ped->wRouteCurDir;
		spd.m_vecPathNextNode = ped->m_vecPathNextNode;
		spd.m_dwPathNodeTimer = ped->m_dwPathNodeTimer;
		spd.m_wCurPathNode = ped->m_wCurPathNode;
		spd.m_wPathNodes = ped->m_wPathNodes;

		//Path node sync
		for (int i = 0; i < 8; i++)
		{
			spd.m_aPathNodeStates[i] = ped->m_aPathNodeStates[i];
			//spd.m_apPathNodesStates[i] = ped->m_apPathNodesStates[i];
		}

		spd.m_dwPathNodeType = ped->m_dwPathNodeType;
		spd.m_nPathState = ped->m_nPathState;
		spd.m_pLastPathNode = ped->m_pLastPathNode;
		spd.m_pNextPathNode = ped->m_pNextPathNode;

		//Move state sync
		spd.m_dwMoveState = ped->m_dwMoveState;

		//Seek sync
		spd.m_fSeekExAngle = ped->m_fSeekExAngle;
		spd.m_vecSeekPosEx = ped->m_vecSeekPosEx;
		spd.m_vecOffsetSeek = ped->m_vecOffsetSeek;

		//Event sync
		spd.m_dwEventType = ped->m_dwEventType;
		spd.m_fAngleToEvent = ped->m_fAngleToEvent;
		spd.m_fEventOrThreatX = ped->m_fEventOrThreatX;
		spd.m_fEventOrThreatY = ped->m_fEventOrThreatY;

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
	vector <pair<CPed*, int>> ::iterator it;
	for (it = players.begin(); it != players.end(); it++) 
	{
		if (it->second == event->entity->id) 
		{
			if (it->first)
				CWorld::Remove(it->first);   // VCCOOP-001: Potential Crash

			players.erase(it);
			break;
		}
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

#ifdef VCCOOP_DEBUG
	gLog->Log("[CClientNetwork] Received script with size: %d\n", scriptSize);
#endif

	// remove the first four bytes, scriptData now contains just the script
	memcpy(scriptData, scriptData + 4, scriptSize);

	// execute (not done yet)

	// "spawn"
	gGame->RestoreCamera();
	gGame->SetCameraBehindPlayer();
	gGame->EnableHUD();
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
	librg_network_add(&ctx, VCOOP_GET_LUA_SCRIPT, ClientReceiveScript);

	addr.host = szAddress;
	addr.port = iPort;

	gLog->Log("[CClientNetwork] Attempting to connect to %s:%d\n", addr.host, addr.port);
	librg_network_start(&ctx, addr);

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ClientConnectThread, NULL, 0, NULL);
}
