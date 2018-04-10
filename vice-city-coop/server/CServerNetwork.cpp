#include "server.h"

/* Server Configuration */
int								CServerNetwork::ServerPort;
int								CServerNetwork::ServerSecret;

HANDLE							CServerNetwork::server_handle;
bool							CServerNetwork::server_running, CServerNetwork::console_active;
librg_ctx_t						CServerNetwork::ctx;

librg_entity_t*	playerEntities[MAX_PLAYERS];
char							playerNames[MAX_PLAYERS][25];
librg_entity_t*	otherEntities[MAX_ENTITIES];

int playerCount = 0;
int entityCount = 0;

CLuaScript						*gGamemodeScript;

PlayerSyncData	playerData[MAX_PLAYERS];
PedSyncData		pedData[MAX_PEDS];

CServerNetwork::CServerNetwork()
{
	ctx = { 0 };
	server_running = true;
#if defined(_MSC_VER)
	server_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)server_thread, NULL, 0, NULL);
#else
	pthread_t thread;
	double threadid;
	pthread_create(&thread, NULL, server_thread, &threadid);
#endif
}
CServerNetwork::~CServerNetwork()
{

}
PedSyncData* CServerNetwork::GetPedSyncData(int id)
{
	return &pedData[id];
}
void CServerNetwork::SetPedSyncData(int id, PedSyncData spd)
{
	if (id == -1 || librg_entity_fetch(&gServerNetwork->ctx, id) == nullptr || CServerNetwork::GetPedSyncData(id) == nullptr)
		return;

	PedSyncData* pedSyncData = CServerNetwork::GetPedSyncData(id);
	librg_entity_t* pedEntity = librg_entity_fetch(&gServerNetwork->ctx, id);
	librg_entity_t* controlEntity = librg_entity_find(&gServerNetwork->ctx, librg_entity_control_get(&gServerNetwork->ctx, id));

	if (controlEntity)	{
		librg_entity_control_remove(&gServerNetwork->ctx, id);
	}
	
	pedSyncData->Health = spd.Health;
	pedSyncData->Armour = spd.Armour;
	pedSyncData->Rotation = spd.Rotation;
	pedSyncData->OrientX = spd.OrientX;
	pedSyncData->OrientY = spd.OrientY;
	pedSyncData->OrientZ = spd.OrientZ;
	pedSyncData->Wander = spd.Wander;
	pedSyncData->iModelIndex = spd.iModelIndex;
	pedSyncData->iInteriorID = spd.iInteriorID;
	pedSyncData->iCurrentAnimID = spd.iCurrentAnimID;
	pedSyncData->CurrWep = spd.CurrWep;
	pedSyncData->WepModelIndex = spd.WepModelIndex;
	pedSyncData->Ammo = spd.Ammo;
	pedSyncData->gameTimer = spd.gameTimer;
	pedSyncData->m_fActionX = spd.m_fActionX;
	pedSyncData->m_fActionY = spd.m_fActionY;
	pedSyncData->m_fRotationDest = spd.m_fRotationDest;
	pedSyncData->m_fLookDirection = spd.m_fLookDirection;
	pedSyncData->m_vecAnimMoveDelta = spd.m_vecAnimMoveDelta;
	pedSyncData->m_dwAnimGroupId = spd.m_dwAnimGroupId;
	pedSyncData->m_vecMoveSpeed = spd.m_vecMoveSpeed;
	pedSyncData->m_dwAction = spd.m_dwAction;
	pedSyncData->m_dwActionTimer = spd.m_dwActionTimer;
	pedSyncData->m_dwActionX = spd.m_dwActionX;
	pedSyncData->m_dwActionY = spd.m_dwActionY;
	pedSyncData->m_dwObjective = spd.m_dwObjective;
	pedSyncData->m_dwObjectiveTimer = spd.m_dwObjectiveTimer;
	pedSyncData->m_vecObjective = spd.m_vecObjective;
	pedSyncData->m_fObjectiveAngle = spd.m_fObjectiveAngle;
	pedSyncData->m_pObjectiveEntity = spd.m_pObjectiveEntity;
	pedSyncData->m_pObjectiveVehicle = spd.m_pObjectiveVehicle;
	pedSyncData->m_dwFleeTimer = spd.m_dwFleeTimer;
	pedSyncData->m_fFleeFromPosX = spd.m_fFleeFromPosX;
	pedSyncData->m_fFleeFromPosY = spd.m_fFleeFromPosY;
	pedSyncData->m_fPathNextNodeDir = spd.m_fPathNextNodeDir;
	pedSyncData->wRouteCurDir = spd.wRouteCurDir;
	pedSyncData->m_vecPathNextNode = spd.m_vecPathNextNode;
	pedSyncData->m_dwPathNodeTimer = spd.m_dwPathNodeTimer;
	pedSyncData->m_wCurPathNode = spd.m_wCurPathNode;
	pedSyncData->m_wPathNodes = spd.m_wPathNodes;
	//pedSyncData->m_aPathNodeStates = spd.m_aPathNodeStates;
	//pedSyncData->m_apPathNodesStates = spd.m_apPathNodesStates;
	pedSyncData->m_dwPathNodeType = spd.m_dwPathNodeType;
	pedSyncData->m_nPathState = spd.m_nPathState;
	pedSyncData->m_pLastPathNode = spd.m_pLastPathNode;
	pedSyncData->m_pNextPathNode = spd.m_pNextPathNode;
	pedSyncData->m_dwMoveState = spd.m_dwMoveState;
	pedSyncData->m_fSeekExAngle = spd.m_fSeekExAngle;
	pedSyncData->m_vecSeekPosEx = spd.m_vecSeekPosEx;
	pedSyncData->m_vecOffsetSeek = spd.m_vecOffsetSeek;
	pedSyncData->m_dwEventType = spd.m_dwEventType;
	pedSyncData->m_fAngleToEvent = spd.m_fAngleToEvent;
	pedSyncData->m_fEventOrThreatX = spd.m_fEventOrThreatX;
	pedSyncData->m_fEventOrThreatY = spd.m_fEventOrThreatY;
}
PlayerSyncData* CServerNetwork::GetPlayerSyncData(int id)
{
	return &playerData[id];
}
void CServerNetwork::SetPlayerSyncData(int id, PlayerSyncData spd)
{
	if (id == -1 || librg_entity_fetch(&gServerNetwork->ctx, id) == nullptr || CServerNetwork::GetPlayerSyncData(id) == nullptr)
		return;

	PlayerSyncData* playerSyncData	= CServerNetwork::GetPlayerSyncData(id);
	librg_entity_t* playerEntity	= librg_entity_fetch(&gServerNetwork->ctx, id);
	librg_entity_t* controlEntity	= librg_entity_find(&gServerNetwork->ctx, librg_entity_control_get(&gServerNetwork->ctx, id));
	
	if (controlEntity)	
	{
		librg_entity_control_remove(&gServerNetwork->ctx, id);
	}

	
	playerSyncData->iModelIndex = spd.iModelIndex;
	playerSyncData->objective = spd.objective;

	//playerSyncData->m_dwAnimGroupId = spd.iCurrentAnimID;
	playerSyncData->Health = spd.Health;
	playerSyncData->Rotation= spd.Rotation;
	playerSyncData->Armour= spd.Armour;
	
	if (!(spd.Ammo <= -1) && !(spd.CurrWep <= -1) && !(spd.WepModelIndex <= -1)) {
		playerSyncData->CurrWep = spd.CurrWep;
		playerSyncData->Ammo = spd.Ammo;
		playerSyncData->WepModelIndex = spd.WepModelIndex;
	}

	playerSyncData->m_nPedFlags.bIsStanding = spd.m_nPedFlags.bIsStanding;
	playerSyncData->m_nPedFlags.bWasStanding = spd.m_nPedFlags.bWasStanding;
	playerSyncData->m_nPedFlags.b03 = spd.m_nPedFlags.b03;
	playerSyncData->m_nPedFlags.bIsPointingGunAt = spd.m_nPedFlags.bIsPointingGunAt;
	playerSyncData->m_nPedFlags.bIsLooking = spd.m_nPedFlags.bIsLooking;
	playerSyncData->m_nPedFlags.b06 = spd.m_nPedFlags.b06;
	playerSyncData->m_nPedFlags.bIsRestoringLook = spd.m_nPedFlags.bIsRestoringLook;
	playerSyncData->m_nPedFlags.bIsAimingGun = spd.m_nPedFlags.bIsAimingGun;

	playerSyncData->m_nPedFlags.bIsRestoringGun = spd.m_nPedFlags.bIsRestoringGun;
	playerSyncData->m_nPedFlags.bCanPointGunAtTarget = spd.m_nPedFlags.bCanPointGunAtTarget;
	playerSyncData->m_nPedFlags.bIsTalking = spd.m_nPedFlags.bIsTalking;
	playerSyncData->m_nPedFlags.bIsInTheAir = spd.m_nPedFlags.bIsInTheAir;
	playerSyncData->m_nPedFlags.bIsLanding = spd.m_nPedFlags.bIsLanding;
	playerSyncData->m_nPedFlags.bIsRunning = spd.m_nPedFlags.bIsRunning;
	playerSyncData->m_nPedFlags.b15 = spd.m_nPedFlags.b15;
	playerSyncData->m_nPedFlags.b16 = spd.m_nPedFlags.b16;

	playerSyncData->m_nPedFlags.bCanPedEnterSeekedCar = spd.m_nPedFlags.bCanPedEnterSeekedCar;
	playerSyncData->m_nPedFlags.bRespondsToThreats = spd.m_nPedFlags.bRespondsToThreats;
	playerSyncData->m_nPedFlags.bRenderPedInCar = spd.m_nPedFlags.bRenderPedInCar;
	playerSyncData->m_nPedFlags.b20 = spd.m_nPedFlags.b20;
	playerSyncData->m_nPedFlags.bUpdateAnimHeading = spd.m_nPedFlags.bUpdateAnimHeading;
	playerSyncData->m_nPedFlags.bRemoveHead = spd.m_nPedFlags.bRemoveHead;
	playerSyncData->m_nPedFlags.bFiringWeapon = spd.m_nPedFlags.bFiringWeapon;
	playerSyncData->m_nPedFlags.b24 = spd.m_nPedFlags.b24;

	playerSyncData->m_nPedFlags.b25a = spd.m_nPedFlags.b25a;
	playerSyncData->m_nPedFlags.b25b = spd.m_nPedFlags.b25b;
	playerSyncData->m_nPedFlags.bStopAndShoot = spd.m_nPedFlags.bStopAndShoot;
	playerSyncData->m_nPedFlags.bIsPedDieAnimPlaying = spd.m_nPedFlags.bIsPedDieAnimPlaying;
	playerSyncData->m_nPedFlags.b30 = spd.m_nPedFlags.b30;
	playerSyncData->m_nPedFlags.b31 = spd.m_nPedFlags.b31;
	playerSyncData->m_nPedFlags.b32 = spd.m_nPedFlags.b32;

	playerSyncData->m_nPedFlags.bKindaStayInSamePlace = spd.m_nPedFlags.bKindaStayInSamePlace;
	playerSyncData->m_nPedFlags.bBeingChasedByPolice = spd.m_nPedFlags.bBeingChasedByPolice;
	playerSyncData->m_nPedFlags.bNotAllowedToDuck = spd.m_nPedFlags.bNotAllowedToDuck;
	playerSyncData->m_nPedFlags.bCrouchWhenShooting = spd.m_nPedFlags.bCrouchWhenShooting;
	playerSyncData->m_nPedFlags.bIsDucking = spd.m_nPedFlags.bIsDucking;
	playerSyncData->m_nPedFlags.bGetUpAnimStarted = spd.m_nPedFlags.bGetUpAnimStarted;
	playerSyncData->m_nPedFlags.bDoBloodyFootprints = spd.m_nPedFlags.bDoBloodyFootprints;
	playerSyncData->m_nPedFlags.b40 = spd.m_nPedFlags.b40;

	playerSyncData->m_nPedFlags.b41 = spd.m_nPedFlags.b41;
	playerSyncData->m_nPedFlags.b42 = spd.m_nPedFlags.b42;
	playerSyncData->m_nPedFlags.b43 = spd.m_nPedFlags.b43;
	playerSyncData->m_nPedFlags.b44 = spd.m_nPedFlags.b44;
	playerSyncData->m_nPedFlags.b45 = spd.m_nPedFlags.b45;
	playerSyncData->m_nPedFlags.b46 = spd.m_nPedFlags.b46;
	playerSyncData->m_nPedFlags.b47 = spd.m_nPedFlags.b47;
	playerSyncData->m_nPedFlags.b48 = spd.m_nPedFlags.b48;

	playerSyncData->m_nPedFlags.b49 = spd.m_nPedFlags.b49;
	playerSyncData->m_nPedFlags.b50 = spd.m_nPedFlags.b50;
	playerSyncData->m_nPedFlags.b51 = spd.m_nPedFlags.b51;
	playerSyncData->m_nPedFlags.b52 = spd.m_nPedFlags.b52;
	playerSyncData->m_nPedFlags.b53 = spd.m_nPedFlags.b53;
	playerSyncData->m_nPedFlags.b54 = spd.m_nPedFlags.b54;
	playerSyncData->m_nPedFlags.bIsBeingJacked = spd.m_nPedFlags.bIsBeingJacked;
	playerSyncData->m_nPedFlags.bFadeOut = spd.m_nPedFlags.bFadeOut;

	playerSyncData->m_nPedFlags.b57 = spd.m_nPedFlags.b57;
	playerSyncData->m_nPedFlags.b58 = spd.m_nPedFlags.b58;
	playerSyncData->m_nPedFlags.b59 = spd.m_nPedFlags.b59;
	playerSyncData->m_nPedFlags.bClearObjective = spd.m_nPedFlags.bClearObjective;
	playerSyncData->m_nPedFlags.b61 = spd.m_nPedFlags.b61;
	playerSyncData->m_nPedFlags.b62 = spd.m_nPedFlags.b62;
	playerSyncData->m_nPedFlags.b63 = spd.m_nPedFlags.b63;
	playerSyncData->m_nPedFlags.b64 = spd.m_nPedFlags.b64;

	playerSyncData->m_nPedFlags.bShakeFist = spd.m_nPedFlags.bShakeFist;
	playerSyncData->m_nPedFlags.bNoCriticalHits = spd.m_nPedFlags.bNoCriticalHits;
	playerSyncData->m_nPedFlags.b67 = spd.m_nPedFlags.b67;
	playerSyncData->m_nPedFlags.bHasAlreadyBeenRecorded = spd.m_nPedFlags.bHasAlreadyBeenRecorded;
	playerSyncData->m_nPedFlags.bFallenDown = spd.m_nPedFlags.bFallenDown;
	playerSyncData->m_nPedFlags.bUpdateMatricesRequired = spd.m_nPedFlags.bUpdateMatricesRequired;
	playerSyncData->m_nPedFlags.b71 = spd.m_nPedFlags.b71;
	playerSyncData->m_nPedFlags.b72 = spd.m_nPedFlags.b72;

	playerSyncData->m_nPedFlags.b73 = spd.m_nPedFlags.b73;
	playerSyncData->m_nPedFlags.b74 = spd.m_nPedFlags.b74;
	playerSyncData->m_nPedFlags.b75 = spd.m_nPedFlags.b75;
	playerSyncData->m_nPedFlags.b76 = spd.m_nPedFlags.b76;
	playerSyncData->m_nPedFlags.b77 = spd.m_nPedFlags.b77;
	playerSyncData->m_nPedFlags.bMiamiViceCop = spd.m_nPedFlags.bMiamiViceCop;
	playerSyncData->m_nPedFlags.bMoneyHasBeenGivenByScript = spd.m_nPedFlags.bMoneyHasBeenGivenByScript;
	playerSyncData->m_nPedFlags.bHasBeenPhotographed = spd.m_nPedFlags.bHasBeenPhotographed;

	playerSyncData->m_nPedFlags.bIsDrowning = spd.m_nPedFlags.bIsDrowning;
	playerSyncData->m_nPedFlags.bDrownsInWater = spd.m_nPedFlags.bDrownsInWater;
	playerSyncData->m_nPedFlags.bStayBehindIfLeaderTooFar = spd.m_nPedFlags.bStayBehindIfLeaderTooFar;
	playerSyncData->m_nPedFlags.bUsingExitCarTimer = spd.m_nPedFlags.bUsingExitCarTimer;
	playerSyncData->m_nPedFlags.bScriptPedIsPlayerAlly = spd.m_nPedFlags.bScriptPedIsPlayerAlly;
	playerSyncData->m_nPedFlags.bKnockedUpIntoAir = spd.m_nPedFlags.bKnockedUpIntoAir;
	playerSyncData->m_nPedFlags.bDeadPedInFrontOfCar = spd.m_nPedFlags.bDeadPedInFrontOfCar;
	playerSyncData->m_nPedFlags.bStayInCarOnJack = spd.m_nPedFlags.bStayInCarOnJack;

	playerSyncData->m_nPedFlags.b89 = spd.m_nPedFlags.b89;
	playerSyncData->m_nPedFlags.bDoomAim = spd.m_nPedFlags.bDoomAim;
	playerSyncData->m_nPedFlags.bCanBeShotInVehicle = spd.m_nPedFlags.bCanBeShotInVehicle;
	playerSyncData->m_nPedFlags.b92 = spd.m_nPedFlags.b92;
	playerSyncData->m_nPedFlags.b93 = spd.m_nPedFlags.b93;
	playerSyncData->m_nPedFlags.b94 = spd.m_nPedFlags.b94;
	playerSyncData->m_nPedFlags.b95 = spd.m_nPedFlags.b95;
	playerSyncData->m_nPedFlags.b96 = spd.m_nPedFlags.b96;

	playerSyncData->m_nPedFlags.b97 = spd.m_nPedFlags.b97;
	playerSyncData->m_nPedFlags.bCrouchWhenScared = spd.m_nPedFlags.bCrouchWhenScared;
	playerSyncData->m_nPedFlags.b99 = spd.m_nPedFlags.b99;
	playerSyncData->m_nPedFlags.b100 = spd.m_nPedFlags.b100;
	playerSyncData->m_nPedFlags.b101 = spd.m_nPedFlags.b101;
	playerSyncData->m_nPedFlags.b102 = spd.m_nPedFlags.b102;
	playerSyncData->m_nPedFlags.bPedWasSetOutOfCollision = spd.m_nPedFlags.bPedWasSetOutOfCollision;
	playerSyncData->m_nPedFlags.bGangMemberReturnsFire = spd.m_nPedFlags.bGangMemberReturnsFire;

	playerData[id] = *playerSyncData;
	librg_message_send_to(&gServerNetwork->ctx, VCOOP_RECEIVE_SPD_UPDATE, playerEntity->client_peer, playerSyncData, sizeof(PlayerSyncData));
	
	if (controlEntity)
	{
		librg_entity_control_set(&gServerNetwork->ctx, id, controlEntity->client_peer);
	}
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
	char msg1[256], name[25], cmd[31];
	librg_data_rptr(msg->data, &msg1, sizeof(msg1));
	sscanf(msg1, "%[^:]: %[^\n]", &name, &cmd);

	std::string str = cmd;
	std::istringstream buf(str);
	std::istream_iterator<std::string> beg(buf), end;
	std::vector<std::string> tokens(beg, end); 
	
	if (tokens.begin()->c_str()[0] == '/')
	{
		/* Command Message */
		gGamemodeScript->Call("onPlayerCommand", "is", librg_entity_find(msg->ctx, msg->peer)->id, cmd);
	}
	else
	{
		/* Normal Chat Message */
		librg_message_send_all(&ctx, VCOOP_RECEIVE_MESSAGE, &msg1, sizeof(msg1));

		gGamemodeScript->Call("onPlayerMessage", "is", librg_entity_find(msg->ctx, msg->peer)->id, cmd);
	}
}
void CServerNetwork::PedCreateEvent(librg_message_t *msg)
{
	librg_entity_t* entity = librg_entity_create(&ctx, VCOOP_PED);
	librg_entity_control_set(&ctx, entity->id, msg->peer);
	
	// crate our custom data container for ped
	entity->user_data = new PedSyncData();
	pedData[entity->id] = *(PedSyncData*)entity->user_data;

	// spawn a ped at player's position
	entity->position = librg_entity_find(msg->ctx, msg->peer)->position;

	otherEntities[entity->id] = entity;
	entityCount++;
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
void CServerNetwork::ObjectCreateEvent(librg_message_t *msg)
{
	librg_entity_t* entity = librg_entity_create(&ctx, VCOOP_OBJECT);
	librg_entity_control_set(&ctx, entity->id, msg->peer);

	// crate our custom data container for ped
	entity->user_data = new ObjectSyncData();

	// spawn a ped at player's position
	entity->position = librg_entity_find(msg->ctx, msg->peer)->position;

	otherEntities[entity->id] = entity;
	entityCount++;
	gLog->Log("[CServerNetwork] Object created. (%d)\n", entity->id);

	gGamemodeScript->Call("onObjectCreated", "i", librg_entity_find(msg->ctx, msg->peer)->id);
}
void CServerNetwork::HandShakeIsDone(librg_message_t *msg)
{
	char name[25];
	librg_data_rptr(msg->data, (void*)&name, 25);
	librg_entity_t * entity = librg_entity_find(msg->ctx, msg->peer);
	strcpy(playerNames[entity->id], name);

	for (int i = 0; i < MAX_PLAYERS; i++)	{
		if (strstr(playerNames[i], name) && librg_entity_fetch(&gServerNetwork->ctx, i) != nullptr && i != entity->id)		{
			gLog->Log("Name already used! %s\n", playerNames[i]);
			gGamemodeScript->Call("onPlayerDisconnect", "iss", entity->id, "Name already used", name);
			librg_network_kick(&gServerNetwork->ctx, entity->client_peer);
		}
	}
	
	//inform everyone we are connected
	connectData cData;
	sprintf(cData.name, playerNames[entity->id]);
	cData.playerId = entity->id;
	librg_message_send_except(msg->ctx, VCOOP_CONNECT, entity->client_peer, &cData, sizeof(connectData));

	// insert data into data pool..
	playerData[entity->id] = PlayerSyncData();

	//loop through connected players and send it to this guy
	for (int i = 0; i < MAX_PLAYERS; i++)	{
		librg_entity_t* entityPtr = librg_entity_fetch(msg->ctx, i);
		if (entityPtr != nullptr && entityPtr->type == VCOOP_PLAYER && entity->id != i)		{
			sprintf(cData.name, playerNames[i]);
			cData.playerId = i;
			librg_message_send_to(msg->ctx, VCOOP_CONNECT, entity->client_peer, &cData, sizeof(connectData));
		}
	}

	// call 'onPlayerConnect'..
	gGamemodeScript->Call("onPlayerConnect", "i", entity->id);
}
void CServerNetwork::on_connect_request(librg_event_t *event) 
{
	// Player Name
	char name[25];
	librg_data_rptr(event->data, (void*)&name, 25);

	u32 secret = librg_data_ru32(event->data);
	gLog->Log("[CServerNetwork][CLIENT REQUEST] Network entity with name %s is requesting to connect\n", name);

#ifdef VCCOOP_VERBOSE_LOG
	gLog->Log("[CServerNetwork] Received CRC: %d\n", secret);
#endif

	if (secret != gServerNetwork->ServerSecret) 
	{
		gLog->Log("[CServerNetwork] Invalid CRC for main.scm received. Custom SCM is not supported yet.\n");
		librg_event_reject(event);
	}
}
void CServerNetwork::on_connect_accepted(librg_event_t *event) 
{
	// initialize sync data and set entity control of the new client
	event->entity->user_data = new PlayerSyncData();
	librg_entity_control_set(event->ctx, event->entity->id, event->entity->client_peer);

	// push back the entity into the entities vector
	playerEntities[event->entity->id] = event->entity;
	playerCount++;
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
	else if (event->entity->type == VCOOP_OBJECT)
	{
		librg_data_wptr(event->data, event->entity->user_data, sizeof(ObjectSyncData));
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
		pedData[event->entity->id] = *(PedSyncData*)event->entity->user_data;

		CVector ped_pos;
		zplm_vec3_t player_pos;
		int currentPedID = 0, currentPlayerID = 0, closestPlayerID = -1, totalPlayersChecked = 0;
		double closestDistance = 0, currentDistance = 0;

		PedSyncData* spd = reinterpret_cast<PedSyncData*>(event->entity->user_data);
		for (auto player : playerEntities) {
			if (player && player->type == VCOOP_PLAYER) {
				
				CVector ped_pos = { event->entity->position.x,  event->entity->position.y, event->entity->position.z };
				player_pos = player->position;

				currentPlayerID = player->id;
				currentDistance = hypot(hypot(ped_pos.x - player_pos.x, ped_pos.y - player_pos.y), ped_pos.z - player_pos.z);

				if ((closestDistance == 0 && totalPlayersChecked == 0)) {
					closestDistance = currentDistance;
					closestPlayerID = currentPlayerID;
				}
				else if (closestDistance > currentDistance) {
					closestDistance = currentDistance;
					closestPlayerID = currentPlayerID;
				}
				totalPlayersChecked++;
			}
		}
		if (closestPlayerID != -1 && totalPlayersChecked != 0) {
			librg_peer_t* controlPeer = librg_entity_control_get(&gServerNetwork->ctx, event->entity->id);
			librg_peer_t* newControlPeer = librg_entity_control_get(&gServerNetwork->ctx, closestPlayerID);

			if (controlPeer != newControlPeer && (newControlPeer)) {
				librg_entity_control_set(&gServerNetwork->ctx, event->entity->id, newControlPeer);
			}
		}
	}
	else if (event->entity->type == VCOOP_VEHICLE)
	{
		librg_data_wptr(event->data, event->entity->user_data, sizeof(VehicleSyncData));

		if (reinterpret_cast<VehicleSyncData*>(event->entity->user_data)->driver == 0)
		{
			librg_peer_t * peer = librg_entity_control_get(event->ctx, event->entity->id);
			librg_entity_control_remove(event->ctx, event->entity->id);
		}
		else if (reinterpret_cast<VehicleSyncData*>(event->entity->user_data)->driver == -1)
		{
			librg_peer_t * peer = librg_entity_control_get(event->ctx, event->entity->id);
			if (!peer) librg_entity_control_set(event->ctx, event->entity->id, event->peer);
		}
		else
		{
			if (librg_entity_fetch(event->ctx, reinterpret_cast<VehicleSyncData*>(event->entity->user_data)->driver) == nullptr)
				return;

			librg_peer_t* currPeer = librg_entity_control_get(event->ctx, event->entity->id);
			librg_peer_t* peer = librg_entity_fetch(event->ctx, reinterpret_cast<VehicleSyncData*>(event->entity->user_data)->driver)->client_peer;			

			if (peer != currPeer)			{
				librg_entity_control_remove(event->ctx, event->entity->id);
				librg_entity_control_set(event->ctx, event->entity->id, librg_entity_fetch(event->ctx, reinterpret_cast<VehicleSyncData*>(event->entity->user_data)->driver)->client_peer);
			}
		}
	}
	else if (event->entity->type == VCOOP_OBJECT)
	{
		librg_data_wptr(event->data, event->entity->user_data, sizeof(ObjectSyncData));
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
		if(librg_entity_control_get(event->ctx, event->entity->id) == event->peer) librg_entity_control_remove(event->ctx, event->entity->id);
	}
	else if (event->entity->type == VCOOP_OBJECT)
	{
		if (librg_entity_control_get(event->ctx, event->entity->id) == event->peer) librg_entity_control_remove(event->ctx, event->entity->id);
	}
}
void CServerNetwork::on_stream_update(librg_event_t *event) 
{
	if (!event->entity->user_data)
		return;

	if (event->entity->type == VCOOP_PLAYER)
	{
		librg_data_rptr(event->data, event->entity->user_data, sizeof(PlayerSyncData));
	}
	else if (event->entity->type == VCOOP_PED)
	{
		librg_data_rptr(event->data, event->entity->user_data, sizeof(PedSyncData));
	}
	else if (event->entity->type == VCOOP_OBJECT)
	{
		librg_data_rptr(event->data, event->entity->user_data, sizeof(ObjectSyncData));
	}
	else if (event->entity->type == VCOOP_VEHICLE)
	{
		librg_data_rptr(event->data, event->entity->user_data, sizeof(VehicleSyncData));

		u32 playerid = reinterpret_cast<VehicleSyncData*>(event->entity->user_data)->driver;
		if (librg_entity_find(event->ctx, librg_entity_control_get(event->ctx, event->entity->id))->id != playerid)		{
			CVector ped_pos;
			zplm_vec3_t player_pos;
			int currentPedID = 0, currentPlayerID = 0, closestPlayerID = -1, totalPlayersChecked = 0;
			double closestDistance = 0, currentDistance = 0;

			VehicleSyncData* spd = reinterpret_cast<VehicleSyncData*>(event->entity->user_data);
			for (auto player : playerEntities) {
				if (player && player->type == VCOOP_PLAYER) {
					ped_pos = spd->vehiclePos;
					player_pos = player->position;

					currentPlayerID = player->id;
					currentDistance = hypot(hypot(ped_pos.x - player_pos.x, ped_pos.y - player_pos.y), ped_pos.z - player_pos.z);

					if ((closestDistance == 0 && totalPlayersChecked == 0)) {
						closestDistance = currentDistance;
						closestPlayerID = currentPlayerID;
					}
					else if (closestDistance > currentDistance) {
						closestDistance = currentDistance;
						closestPlayerID = currentPlayerID;
					}
					totalPlayersChecked++;
				}
			}
			if (closestPlayerID != -1 && totalPlayersChecked != 0) {
				librg_peer_t* controlPeer = librg_entity_control_get(&gServerNetwork->ctx, event->entity->id);
				librg_peer_t* newControlPeer = librg_entity_control_get(&gServerNetwork->ctx, closestPlayerID);

				if (controlPeer != newControlPeer && (newControlPeer))				{
					librg_entity_control_set(&gServerNetwork->ctx, event->entity->id, newControlPeer);
				}
			}
			//gLog->Log("Closest for V#%d is P%d with %f\n", event->entity->id, closestPlayerID, closestDistance);
		}

		if (playerid != -1)
		{
			if (librg_entity_fetch(event->ctx, playerid) == nullptr)
				return;


			if (librg_entity_fetch(event->ctx, playerid)->client_peer != nullptr)
			{
				librg_peer_t * owner = librg_entity_control_get(event->ctx, event->entity->id);
				librg_peer_t * driver = librg_entity_fetch(event->ctx, playerid)->client_peer;

				if (playerid != librg_entity_find(event->ctx, owner)->id)
					librg_entity_control_set(event->ctx, event->entity->id, librg_entity_fetch(event->ctx, playerid)->client_peer);
			}
		}
	}
}

void CServerNetwork::on_disconnect(librg_event_t* event)
{
	librg_message_send_except(&ctx, VCOOP_DISCONNECT, event->peer, &event->entity->id, sizeof(u32));

	gLog->Log("[ID#%d] Disconnected from server.\n", event->entity->id);

	librg_entity_id *entities;
	usize amount = librg_entity_query(event->ctx, event->entity->id, &entities);

	if (event->entity->type == VCOOP_PLAYER) {
		strcpy(playerNames[event->entity->id], "");
	}

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

				if(!GetPedSyncData(entity->id)->DontDestroy)
					librg_entity_destroy(event->ctx, entity->id);
			}
		}
	}

	librg_entity_control_remove(event->ctx, event->entity->id);

	gGamemodeScript->Call("onPlayerDisconnect", "is", event->entity->id, "Quit");

	playerData[event->entity->id]		= PlayerSyncData();
	playerEntities[event->entity->id]	= nullptr;
	playerCount--;

	delete event->entity->user_data;
}

void CServerNetwork::measure(void *userptr) {
#if !defined(VCCOOP_DEBUG) && defined(_MSC_VER)
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

#if !defined(VCCOOP_DEBUG) && defined(_MSC_VER)
	std::string buf("[");
	buf.append(time_stamp(LOGGER_TIME_FORMAT));
	buf.append("][" VCCOOP_NAME "][CServerNetwork]");
	printf("%s Server Port: %d | Players: %d/%d | Entities: %d/%d\n%s took %f ms. Used bandwidth D/U: (%f / %f) mbps.\n", buf.c_str(), ServerPort, playerCount, MAX_PLAYERS, entityCount, MAX_ENTITIES, buf.c_str(), ctx->last_update, dl, up);
#endif
}

#if defined (_MSC_VER)
void CServerNetwork::server_thread()
#else 
void *CServerNetwork::server_thread(void* p)
#endif
{
	ctx.world_size			= zplm_vec3(5000.0f, 5000.0f, 5000.0f);
	ctx.mode				= LIBRG_MODE_SERVER;
	ctx.tick_delay			= VCCOOP_SERVER_TICK_DELAY;
	ctx.max_connections		= (MAX_PLAYERS*2);
	ctx.max_entities		= (MAX_ENTITIES+MAX_PLAYERS);
	librg_init(&ctx);
	
	librg_event_add(&ctx,	LIBRG_CONNECTION_REQUEST,		on_connect_request);
	librg_event_add(&ctx,	LIBRG_CONNECTION_ACCEPT,		on_connect_accepted);
	librg_event_add(&ctx,	LIBRG_CONNECTION_DISCONNECT,	on_disconnect);

	librg_event_add(&ctx,	LIBRG_ENTITY_CREATE,			on_creating_entity);
	librg_event_add(&ctx,	LIBRG_ENTITY_UPDATE,			on_entity_update);
	librg_event_add(&ctx,	LIBRG_ENTITY_REMOVE,			on_entity_remove);

	librg_network_add(&ctx, VCOOP_CREATE_PED,				PedCreateEvent);
	librg_network_add(&ctx, VCOOP_CREATE_VEHICLE,			VehCreateEvent);
	librg_network_add(&ctx, VCOOP_CREATE_OBJECT,			ObjectCreateEvent);
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

/*#if defined(_MSC_VER)
#ifndef VCCOOP_VERBOSE_LOG	
	zpl_timer_t *tick_timer = zpl_timer_add(ctx.timers);
	tick_timer->user_data = (void *)&ctx; 
	zpl_timer_set(tick_timer, 1000 * 1000, -1, measure);
	zpl_timer_start(tick_timer, 1000);
#endif 
#endif*/

	// Auto-detect all client scripts
	gDataMgr->LoadScripts();
	
	while (server_running) {
		if (!gGamemodeScript->GetServerStartStatus())		{
			gGamemodeScript->Call("onServerStart");
			gGamemodeScript->SetServerStartStatus(true);
		}
		librg_tick(&ctx);
	}

	librg_network_stop(&ctx);
	librg_free(&ctx);

	server_running = false;
}