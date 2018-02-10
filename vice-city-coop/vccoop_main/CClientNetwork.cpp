#define LIBRG_IMPLEMENTATION
#define LIBRG_DEBUG
#include "main.h"

SPlayerData							LocalPlayerInfo;

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
void CClientNetwork::on_connect_accepted(librg_event_t *event) {
	gLog->Log("[CClientNetwork] Connection Accepted\n");

	gRender->bConnecting = false;
	gRender->bGUI = false;
	gRender->bAboutWindow = false;
	
	connected = true;
	local_player = event->entity;
	event->entity->user_data = new CClientPlayer(event->entity->id);

	gGame->RestoreCamera();
	gGame->SetCameraBehindPlayer();
	gGame->EnableHUD();

	players.push_back(std::pair<CPed*, int>(LocalPlayer(), event->entity->id));
}
void CClientNetwork::on_connect_refused(librg_event_t *event) {
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
	strcpy(LocalPlayerInfo.szName, gGame->Name.c_str());

	while (client_running) {
		if (connected && local_player)
		{
			LocalPlayerInfo.iModelIndex = LocalPlayer()->m_nModelIndex;
			LocalPlayerInfo.iCurrentAnimID = LocalPlayer()->m_dwAnimGroupId;

			LocalPlayerInfo.Health = LocalPlayer()->m_fHealth;
			LocalPlayerInfo.Rotation = LocalPlayer()->m_fRotationCur;
			LocalPlayerInfo.Armour = LocalPlayer()->m_fArmour;
		}
		librg_tick(&ctx);
		zpl_sleep_ms(1);
	}

	librg_network_stop(&ctx);
	librg_free(&ctx);
}
void CClientNetwork::on_entity_create(librg_event_t *event) {
	zplm_vec3_t position = event->entity->position;

	if (event->entity->type == VCOOP_PLAYER || event->entity->type == VCOOP_PED)
	{
		SPlayerData spd;
		CStreaming::RequestModel(7, 0);
		librg_data_rptr(event->data, &spd, sizeof(SPlayerData));

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
void CClientNetwork::on_entity_update(librg_event_t *event) {
	SPlayerData spd;
	librg_data_rptr(event->data, &spd, sizeof(SPlayerData));

	if (event->entity->type == VCOOP_PLAYER || event->entity->type == VCOOP_PED)
	{
		if (event->entity->type == VCOOP_PLAYER)
		{
			auto player = (CClientPlayer *)event->entity->user_data;

			auto ped = player->ped;
			ped->Teleport(*(CVector *)&event->entity->position);

			ped->m_nModelIndex = spd.iModelIndex;
			ped->m_dwAnimGroupId = spd.iCurrentAnimID;
			ped->m_fHealth = spd.Health;
			ped->m_fRotationCur = spd.Rotation;
			ped->m_fArmour = spd.Armour;

			gGame->remotePlayerKeys[player->gameID] = spd.playerKeys;
			gGame->remotePlayerLookFrontX[player->gameID] = spd.playerLook;
		}
		else
		{
			auto ped = (CPed *)event->entity->user_data;

			CTimer::m_snTimeInMilliseconds = spd.pedData.gameTimer;

			ped->Teleport(*(CVector *)&event->entity->position);

			ped->m_nModelIndex = spd.iModelIndex;
			ped->m_fHealth = spd.Health;
			ped->m_fRotationCur = spd.Rotation;
			ped->m_fArmour = spd.Armour;

			ped->m_fRotationDest = spd.pedData.m_fRotationDest;
			ped->m_fLookDirection = spd.pedData.m_fLookDirection;
			ped->m_vecAnimMoveDelta = spd.pedData.m_vecAnimMoveDelta;
			ped->m_dwAnimGroupId = spd.pedData.m_dwAnimGroupId;
			ped->m_vecMoveSpeed = spd.pedData.m_vecMoveSpeed;

			//Action sync
			ped->m_dwAction = spd.pedData.m_dwAction;
			ped->m_dwActionTimer = spd.pedData.m_dwActionTimer;
			ped->m_fActionX = spd.pedData.m_fActionX;
			ped->m_fActionY = spd.pedData.m_fActionY;

			//Objective sync
			ped->m_dwObjective = spd.pedData.m_dwObjective;
			ped->m_dwObjectiveTimer = spd.pedData.m_dwObjectiveTimer;
			ped->m_vecObjective = spd.pedData.m_vecObjective;
			ped->m_fObjectiveAngle = spd.pedData.m_fObjectiveAngle;
			ped->m_pObjectiveEntity = GetEntityFromNetworkID(spd.pedData.m_pObjectiveEntity);
			//ped->m_pObjectiveVehicle		= spd.pedData.m_pObjectiveVehicle;

			//Flee sync
			ped->m_dwFleeTimer = spd.pedData.m_dwFleeTimer;
			ped->m_fFleeFromPosX = spd.pedData.m_fFleeFromPosX;
			ped->m_fFleeFromPosY = spd.pedData.m_fFleeFromPosY;

			//Path sync
			ped->m_fPathNextNodeDir = spd.pedData.m_fPathNextNodeDir;
			ped->wRouteCurDir = spd.pedData.wRouteCurDir;
			ped->m_vecPathNextNode = spd.pedData.m_vecPathNextNode;
			ped->m_dwPathNodeTimer = spd.pedData.m_dwPathNodeTimer;
			ped->m_wCurPathNode = spd.pedData.m_wCurPathNode;
			ped->m_wPathNodes = spd.pedData.m_wPathNodes;

			for (int i = 0; i < 8; i++)
			{
				ped->m_aPathNodeStates[i] = spd.pedData.m_aPathNodeStates[i];
				//ped->m_apPathNodesStates[i] = spd.pedData.m_apPathNodesStates[i];
			}

			ped->m_dwPathNodeType = spd.pedData.m_dwPathNodeType;
			ped->m_nPathState = spd.pedData.m_nPathState;
			ped->m_pLastPathNode = spd.pedData.m_pLastPathNode;
			ped->m_pNextPathNode = spd.pedData.m_pNextPathNode;

			ped->SetMoveState((eMoveState)spd.pedData.m_dwMoveState);

			//Seek sync
			ped->m_fSeekExAngle = spd.pedData.m_fSeekExAngle;
			ped->m_vecSeekPosEx = spd.pedData.m_vecSeekPosEx;
			ped->m_vecOffsetSeek = spd.pedData.m_vecOffsetSeek;

			//Event sync
			ped->m_dwEventType = spd.pedData.m_dwEventType;
			ped->m_fAngleToEvent = spd.pedData.m_fAngleToEvent;
			ped->m_fEventOrThreatX = spd.pedData.m_fEventOrThreatX;
			ped->m_fEventOrThreatY = spd.pedData.m_fEventOrThreatY;
		}
	}
	else if (event->entity->type == VCOOP_VEHICLE)
	{
		auto veh = (CVehicle *)event->entity->user_data;
		veh->Teleport(*(CVector *)&event->entity->position);
	}
}
void CClientNetwork::on_client_stream(librg_event_t *event) {
	if (event->entity->type == VCOOP_PLAYER || event->entity->type == VCOOP_PED)
	{
		SPlayerData spd;
		if (event->entity->type == VCOOP_PLAYER)
		{
			auto player = (CClientPlayer *)event->entity->user_data;
			auto ped = player->ped;

			event->entity->position = *(zplm_vec3_t *)&ped->GetPosition();

			spd.Health = ped->m_fHealth;
			spd.iCurrentAnimID = ped->m_dwAnimGroupId;
			spd.Armour = ped->m_fArmour;
			spd.iModelIndex = ped->m_nModelIndex;
			spd.Rotation = ped->m_fRotationCur;

			spd.playerKeys = *(GTA_CONTROLSET*)0x7DBCB0;
			spd.playerLook = *(CAMERA_AIM*)0x7E4978;

			librg_data_wptr(event->data, &spd, sizeof(SPlayerData));
		}
		else
		{
			auto ped = (CPed *)event->entity->user_data;
			event->entity->position = *(zplm_vec3_t *)&ped->GetPosition();

			spd.pedData.gameTimer = CTimer::m_snTimeInMilliseconds;

			spd.Health = ped->m_fHealth;
			spd.iCurrentAnimID = ped->m_dwAnimGroupId;
			spd.Armour = ped->m_fArmour;
			spd.iModelIndex = ped->m_nModelIndex;
			spd.Rotation = ped->m_fRotationCur;

			//Move sync
			spd.pedData.m_fRotationDest = ped->m_fRotationDest;
			spd.pedData.m_fLookDirection = ped->m_fLookDirection;
			spd.pedData.m_vecAnimMoveDelta = ped->m_vecAnimMoveDelta;
			spd.pedData.m_dwAnimGroupId = ped->m_dwAnimGroupId;
			spd.pedData.m_vecMoveSpeed = ped->m_vecMoveSpeed;

			//Action sync
			spd.pedData.m_dwAction = ped->m_dwAction;
			spd.pedData.m_dwActionTimer = ped->m_dwActionTimer;
			spd.pedData.m_fActionX = ped->m_fActionX;
			spd.pedData.m_fActionY = ped->m_fActionY;

			//Objective sync
			spd.pedData.m_dwObjective = ped->m_dwObjective;
			spd.pedData.m_dwObjectiveTimer = ped->m_dwObjectiveTimer;
			spd.pedData.m_vecObjective = ped->m_vecObjective;
			spd.pedData.m_fObjectiveAngle = ped->m_fObjectiveAngle;
			//spd.pedData.m_pObjectiveVehicle		= ped->m_pObjectiveVehicle;

			//Flee sync
			spd.pedData.m_dwFleeTimer = ped->m_dwFleeTimer;
			spd.pedData.m_fFleeFromPosX = ped->m_fFleeFromPosX;
			spd.pedData.m_fFleeFromPosY = ped->m_fFleeFromPosY;

			//Path sync
			spd.pedData.m_fPathNextNodeDir = ped->m_fPathNextNodeDir;
			spd.pedData.wRouteCurDir = ped->wRouteCurDir;
			spd.pedData.m_vecPathNextNode = ped->m_vecPathNextNode;
			spd.pedData.m_dwPathNodeTimer = ped->m_dwPathNodeTimer;
			spd.pedData.m_wCurPathNode = ped->m_wCurPathNode;
			spd.pedData.m_wPathNodes = ped->m_wPathNodes;

			//Path node sync
			for (int i = 0; i < 8; i++)
			{
				spd.pedData.m_aPathNodeStates[i] = ped->m_aPathNodeStates[i];
				//spd.pedData.m_apPathNodesStates[i] = ped->m_apPathNodesStates[i];
			}

			spd.pedData.m_dwPathNodeType = ped->m_dwPathNodeType;
			spd.pedData.m_nPathState = ped->m_nPathState;
			spd.pedData.m_pLastPathNode = ped->m_pLastPathNode;
			spd.pedData.m_pNextPathNode = ped->m_pNextPathNode;

			//Move state sync
			spd.pedData.m_dwMoveState = ped->m_dwMoveState;

			//Seek sync
			spd.pedData.m_fSeekExAngle = ped->m_fSeekExAngle;
			spd.pedData.m_vecSeekPosEx = ped->m_vecSeekPosEx;
			spd.pedData.m_vecOffsetSeek = ped->m_vecOffsetSeek;

			//Event sync
			spd.pedData.m_dwEventType = ped->m_dwEventType;
			spd.pedData.m_fAngleToEvent = ped->m_fAngleToEvent;
			spd.pedData.m_fEventOrThreatX = ped->m_fEventOrThreatX;
			spd.pedData.m_fEventOrThreatY = ped->m_fEventOrThreatY;

			librg_data_wptr(event->data, &spd, sizeof(SPlayerData));
		}
	}
	else if (event->entity->type == VCOOP_VEHICLE)
	{
		//not done yet
		CVehicle *veh = (CVehicle *)event->entity->user_data;
		event->entity->position = *(zplm_vec3_t *)&veh->GetPosition();

		SPlayerData spd;
		spd.Health = veh->m_fHealth;
		librg_data_wptr(event->data, &spd, sizeof(SPlayerData));
	}
}
void CClientNetwork::on_entity_remove(librg_event_t *event) {
	vector <pair<CPed*, int>> ::iterator it;
	for (it = players.begin(); it != players.end(); it++) {
		if (it->second == event->entity->id) {
			if (it->first)
				CWorld::Remove(it->first);   // VCCOOP-001: Potential Crash

			players.erase(it);
			break;
		}
	}
}
void CClientNetwork::on_disconnect(librg_event_t *event) {
	StopClientThread();

	gLog->Log("[CClientNetwork] Disconnected.\n");
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

	addr.host = szAddress;
	addr.port = iPort;

	gLog->Log("[CClientNetwork] Attempting to connect to %s:%d\n", addr.host, addr.port);
	librg_network_start(&ctx, addr);

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ClientConnectThread, NULL, 0, NULL);
}
