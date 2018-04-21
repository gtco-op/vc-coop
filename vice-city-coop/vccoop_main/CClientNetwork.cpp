#define LIBRG_IMPLEMENTATION
#define LIBRG_DEBUG
#include "main.h"

librg_address_t						CClientNetwork::addr;
librg_ctx_t							CClientNetwork::ctx;
librg_entity_t *					CClientNetwork::local_player;

bool								CClientNetwork::client_running;
bool								CClientNetwork::connected;

librg_entity_t*	playerEntities[MAX_PLAYERS];
char			playerNames[MAX_PLAYERS][25];
librg_entity_t*	otherEntities[MAX_ENTITIES];

CClientNetwork::CClientNetwork()
{
	ctx = { 0 };
	local_player = nullptr;

	client_running = false;
	connected = false;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		playerEntities[i] = nullptr;
		sprintf(playerNames[i], "%c", '\0');
	}
	for (int i = 0; i < MAX_ENTITIES; i++)
	{
		otherEntities[i] = nullptr;
	}
}
CClientNetwork::~CClientNetwork()
{
	this->StopThread();
	gLog->Log("[CClientNetwork] CRichPresence shutting down\n");

	gLog->Log("[CClientNetwork] CClientNetwork shutting down\n");
	this->StopClientThread();

	networkEntities.clear();
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
	if (ent == nullptr)
		return -1;

	for (auto it : gNetwork->networkEntities)
	{
		if (it != nullptr && it->networkID >= 0 && it->GetEntity() == ent)
		{
			return it->networkID;
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
	
	std::string path = GetExecutablePath().append(VCCOOP_DEFAULT_MAIN_SCM_PATH);
	std::ostringstream buf; std::ifstream input(path.c_str()); buf << input.rdbuf();
	std::string buffer = buf.str();

	librg_data_wu32(event->data, gConfig->CRC32((unsigned char*)buffer.c_str()));
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

	// Run our 'OnConnected' internal callback
	gGame->OnConnected();
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
		zpl_sleep_ms(VCCOOP_CLIENT_TICK_DELAY);
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

		event->entity->user_data = new CClientPed(event->entity->id, spd.iModelIndex);
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

			vehicle = new CClientVehicle(event->entity->id, spd.modelID, spd.vehiclePos, spd.nPrimaryColor, spd.nSecondaryColor);
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

		if(!spd.isInVehicle) ped->m_placement.pos = (*(CVector *)&event->entity->position);

		player->SyncPlayer(spd);

	}
	else if(event->entity->type == VCOOP_PED)
	{
		PedSyncData spd;
		librg_data_rptr(event->data, &spd, sizeof(PedSyncData));

		auto pedestrian = (CClientPed *)event->entity->user_data;
		auto ped = pedestrian->ped;

		ped->m_placement.pos = (*(CVector *)&event->entity->position);

		pedestrian->SyncPed(spd);
	}
	else if (event->entity->type == VCOOP_VEHICLE)
	{
		VehicleSyncData spd;
		librg_data_rptr(event->data, &spd, sizeof(VehicleSyncData));

		auto vehicle = (CClientVehicle *)event->entity->user_data;
		auto veh = vehicle->veh;
		veh->m_placement.pos = (*(CVector *)&event->entity->position);
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
	if (entity)
	{
		CClientPlayer * player = (CClientPlayer *)entity->user_data;
		gLog->Log("[CClientNetwork] %s has disconnected.\n", player->szName);

		for (auto it = gNetwork->networkEntities.begin(); it != gNetwork->networkEntities.end(); ++it)		{
			if (player == (*it))			{
				gNetwork->networkEntities.erase(it);
				break;
			}
		}
		delete player;
		entity->user_data = NULL;
	}
	// else, a player was kicked...
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
void CClientNetwork::ClientStartMissionScript(librg_message_t* msg)
{
	if (!gGame->bMissionScriptStarted)
	{
		gGame->StartMissionScript();
		gGame->bMissionScriptStarted = true;
	}
}
void SyncLocalPlayer(PlayerSyncData spd)
{
	if (spd.iModelIndex != LocalPlayer()->m_nModelIndex && CModelIDs::IsValidPedModel(spd.iModelIndex)) {
		if (CStreaming::ms_aInfoForModel[spd.iModelIndex].m_nLoadState != LOADSTATE_LOADED){
			gGame->CustomModelLoad(spd.iModelIndex);

			while (CStreaming::ms_aInfoForModel[spd.iModelIndex].m_nLoadState != LOADSTATE_LOADED)
			{
				Sleep(100);
			}
		}
		LocalPlayer()->SetModelIndex(spd.iModelIndex);
	}
	else
	{
		if (CStreaming::ms_aInfoForModel[7].m_nLoadState != LOADSTATE_LOADED) {
			gGame->CustomModelLoad(7);

			while (CStreaming::ms_aInfoForModel[7].m_nLoadState != LOADSTATE_LOADED)
			{
				Sleep(100);
			}
		}
		LocalPlayer()->SetModelIndex(7);
	}
	if (spd.WepModelIndex != LocalPlayer()->m_dwWepModelID && spd.WepModelIndex > 200 && spd.CurrWep > 1 && spd.Ammo != -1)
	{
		if (CStreaming::ms_aInfoForModel[spd.WepModelIndex].m_nLoadState != LOADSTATE_LOADED)		{
			gGame->CustomModelLoad(spd.WepModelIndex);
		}

		switch ((eWeaponType)spd.CurrWep)
		{
			case eWeaponType::WEAPONTYPE_CHAINSAW:
			case eWeaponType::WEAPONTYPE_BASEBALLBAT:
			case eWeaponType::WEAPONTYPE_BRASSKNUCKLE:
			case eWeaponType::WEAPONTYPE_CAMERA:
			case eWeaponType::WEAPONTYPE_CLEAVER:
			case eWeaponType::WEAPONTYPE_DETONATOR:
			case eWeaponType::WEAPONTYPE_GOLFCLUB:
			case eWeaponType::WEAPONTYPE_HAMMER:
			case eWeaponType::WEAPONTYPE_KATANA:
			case eWeaponType::WEAPONTYPE_KNIFE:
			case eWeaponType::WEAPONTYPE_MACHETE:
			case eWeaponType::WEAPONTYPE_NIGHTSTICK:
			case eWeaponType::WEAPONTYPE_SCREWDRIVER:
			{
				LocalPlayer()->GiveWeapon((eWeaponType)spd.CurrWep, 1, true);
				LocalPlayer()->SetAmmo((eWeaponType)spd.CurrWep, 1);
				break;
			}
			default:
			{
				LocalPlayer()->GiveWeapon((eWeaponType)spd.CurrWep, spd.Ammo, true);
				LocalPlayer()->SetAmmo((eWeaponType)spd.CurrWep, spd.Ammo);
				break;
			}
		}
		if (LocalPlayer()->m_dwWepModelID != spd.WepModelIndex) LocalPlayer()->SetCurrentWeapon((eWeaponType)spd.CurrWep);
	}
	else
	{
		if (LocalPlayer()->m_dwWepModelID != -1) LocalPlayer()->SetCurrentWeapon(eWeaponType::WEAPONTYPE_UNARMED);
	}

	LocalPlayer()->m_dwObjective = spd.objective;

	//LocalPlayer()->m_dwAnimGroupId = spd.iCurrentAnimID;
	LocalPlayer()->m_fHealth = spd.Health;
	LocalPlayer()->m_fRotationCur = spd.Rotation;
	LocalPlayer()->m_fRotationDest = spd.Rotation;
	LocalPlayer()->m_fArmour = spd.Armour;

	LocalPlayer()->m_nPedFlags.bIsStanding = spd.m_nPedFlags.bIsStanding;
	LocalPlayer()->m_nPedFlags.bWasStanding = spd.m_nPedFlags.bWasStanding;
	LocalPlayer()->m_nPedFlags.b03 = spd.m_nPedFlags.b03;
	LocalPlayer()->m_nPedFlags.bIsPointingGunAt = spd.m_nPedFlags.bIsPointingGunAt;
	LocalPlayer()->m_nPedFlags.bIsLooking = spd.m_nPedFlags.bIsLooking;
	LocalPlayer()->m_nPedFlags.b06 = spd.m_nPedFlags.b06;
	LocalPlayer()->m_nPedFlags.bIsRestoringLook = spd.m_nPedFlags.bIsRestoringLook;
	LocalPlayer()->m_nPedFlags.bIsAimingGun = spd.m_nPedFlags.bIsAimingGun;

	LocalPlayer()->m_nPedFlags.bIsRestoringGun = spd.m_nPedFlags.bIsRestoringGun;
	LocalPlayer()->m_nPedFlags.bCanPointGunAtTarget = spd.m_nPedFlags.bCanPointGunAtTarget;
	LocalPlayer()->m_nPedFlags.bIsTalking = spd.m_nPedFlags.bIsTalking;
	LocalPlayer()->m_nPedFlags.bIsInTheAir = spd.m_nPedFlags.bIsInTheAir;
	LocalPlayer()->m_nPedFlags.bIsLanding = spd.m_nPedFlags.bIsLanding;
	LocalPlayer()->m_nPedFlags.bIsRunning = spd.m_nPedFlags.bIsRunning;
	LocalPlayer()->m_nPedFlags.b15 = spd.m_nPedFlags.b15;
	LocalPlayer()->m_nPedFlags.b16 = spd.m_nPedFlags.b16;

	LocalPlayer()->m_nPedFlags.bCanPedEnterSeekedCar = spd.m_nPedFlags.bCanPedEnterSeekedCar;
	LocalPlayer()->m_nPedFlags.bRespondsToThreats = spd.m_nPedFlags.bRespondsToThreats;
	LocalPlayer()->m_nPedFlags.bRenderPedInCar = spd.m_nPedFlags.bRenderPedInCar;
	LocalPlayer()->m_nPedFlags.b20 = spd.m_nPedFlags.b20;
	LocalPlayer()->m_nPedFlags.bUpdateAnimHeading = spd.m_nPedFlags.bUpdateAnimHeading;
	LocalPlayer()->m_nPedFlags.bRemoveHead = spd.m_nPedFlags.bRemoveHead;
	LocalPlayer()->m_nPedFlags.bFiringWeapon = spd.m_nPedFlags.bFiringWeapon;
	LocalPlayer()->m_nPedFlags.b24 = spd.m_nPedFlags.b24;

	LocalPlayer()->m_nPedFlags.b25a = spd.m_nPedFlags.b25a;
	LocalPlayer()->m_nPedFlags.b25b = spd.m_nPedFlags.b25b;
	LocalPlayer()->m_nPedFlags.bStopAndShoot = spd.m_nPedFlags.bStopAndShoot;
	LocalPlayer()->m_nPedFlags.bIsPedDieAnimPlaying = spd.m_nPedFlags.bIsPedDieAnimPlaying;
	LocalPlayer()->m_nPedFlags.b30 = spd.m_nPedFlags.b30;
	LocalPlayer()->m_nPedFlags.b31 = spd.m_nPedFlags.b31;
	LocalPlayer()->m_nPedFlags.b32 = spd.m_nPedFlags.b32;

	LocalPlayer()->m_nPedFlags.bKindaStayInSamePlace = spd.m_nPedFlags.bKindaStayInSamePlace;
	LocalPlayer()->m_nPedFlags.bBeingChasedByPolice = spd.m_nPedFlags.bBeingChasedByPolice;
	LocalPlayer()->m_nPedFlags.bNotAllowedToDuck = spd.m_nPedFlags.bNotAllowedToDuck;
	LocalPlayer()->m_nPedFlags.bCrouchWhenShooting = spd.m_nPedFlags.bCrouchWhenShooting;
	LocalPlayer()->m_nPedFlags.bIsDucking = spd.m_nPedFlags.bIsDucking;
	LocalPlayer()->m_nPedFlags.bGetUpAnimStarted = spd.m_nPedFlags.bGetUpAnimStarted;
	LocalPlayer()->m_nPedFlags.bDoBloodyFootprints = spd.m_nPedFlags.bDoBloodyFootprints;
	LocalPlayer()->m_nPedFlags.b40 = spd.m_nPedFlags.b40;

	LocalPlayer()->m_nPedFlags.b41 = spd.m_nPedFlags.b41;
	LocalPlayer()->m_nPedFlags.b42 = spd.m_nPedFlags.b42;
	LocalPlayer()->m_nPedFlags.b43 = spd.m_nPedFlags.b43;
	LocalPlayer()->m_nPedFlags.b44 = spd.m_nPedFlags.b44;
	LocalPlayer()->m_nPedFlags.b45 = spd.m_nPedFlags.b45;
	LocalPlayer()->m_nPedFlags.b46 = spd.m_nPedFlags.b46;
	LocalPlayer()->m_nPedFlags.b47 = spd.m_nPedFlags.b47;
	LocalPlayer()->m_nPedFlags.b48 = spd.m_nPedFlags.b48;

	LocalPlayer()->m_nPedFlags.b49 = spd.m_nPedFlags.b49;
	LocalPlayer()->m_nPedFlags.b50 = spd.m_nPedFlags.b50;
	LocalPlayer()->m_nPedFlags.b51 = spd.m_nPedFlags.b51;
	LocalPlayer()->m_nPedFlags.b52 = spd.m_nPedFlags.b52;
	LocalPlayer()->m_nPedFlags.b53 = spd.m_nPedFlags.b53;
	LocalPlayer()->m_nPedFlags.b54 = spd.m_nPedFlags.b54;
	LocalPlayer()->m_nPedFlags.bIsBeingJacked = spd.m_nPedFlags.bIsBeingJacked;
	LocalPlayer()->m_nPedFlags.bFadeOut = spd.m_nPedFlags.bFadeOut;

	LocalPlayer()->m_nPedFlags.b57 = spd.m_nPedFlags.b57;
	LocalPlayer()->m_nPedFlags.b58 = spd.m_nPedFlags.b58;
	LocalPlayer()->m_nPedFlags.b59 = spd.m_nPedFlags.b59;
	LocalPlayer()->m_nPedFlags.bClearObjective = spd.m_nPedFlags.bClearObjective;
	LocalPlayer()->m_nPedFlags.b61 = spd.m_nPedFlags.b61;
	LocalPlayer()->m_nPedFlags.b62 = spd.m_nPedFlags.b62;
	LocalPlayer()->m_nPedFlags.b63 = spd.m_nPedFlags.b63;
	LocalPlayer()->m_nPedFlags.b64 = spd.m_nPedFlags.b64;

	LocalPlayer()->m_nPedFlags.bShakeFist = spd.m_nPedFlags.bShakeFist;
	LocalPlayer()->m_nPedFlags.bNoCriticalHits = spd.m_nPedFlags.bNoCriticalHits;
	LocalPlayer()->m_nPedFlags.b67 = spd.m_nPedFlags.b67;
	LocalPlayer()->m_nPedFlags.bHasAlreadyBeenRecorded = spd.m_nPedFlags.bHasAlreadyBeenRecorded;
	LocalPlayer()->m_nPedFlags.bFallenDown = spd.m_nPedFlags.bFallenDown;
	LocalPlayer()->m_nPedFlags.bUpdateMatricesRequired = spd.m_nPedFlags.bUpdateMatricesRequired;
	LocalPlayer()->m_nPedFlags.b71 = spd.m_nPedFlags.b71;
	LocalPlayer()->m_nPedFlags.b72 = spd.m_nPedFlags.b72;

	LocalPlayer()->m_nPedFlags.b73 = spd.m_nPedFlags.b73;
	LocalPlayer()->m_nPedFlags.b74 = spd.m_nPedFlags.b74;
	LocalPlayer()->m_nPedFlags.b75 = spd.m_nPedFlags.b75;
	LocalPlayer()->m_nPedFlags.b76 = spd.m_nPedFlags.b76;
	LocalPlayer()->m_nPedFlags.b77 = spd.m_nPedFlags.b77;
	LocalPlayer()->m_nPedFlags.bMiamiViceCop = spd.m_nPedFlags.bMiamiViceCop;
	LocalPlayer()->m_nPedFlags.bMoneyHasBeenGivenByScript = spd.m_nPedFlags.bMoneyHasBeenGivenByScript;
	LocalPlayer()->m_nPedFlags.bHasBeenPhotographed = spd.m_nPedFlags.bHasBeenPhotographed;

	LocalPlayer()->m_nPedFlags.bIsDrowning = spd.m_nPedFlags.bIsDrowning;
	LocalPlayer()->m_nPedFlags.bDrownsInWater = spd.m_nPedFlags.bDrownsInWater;
	LocalPlayer()->m_nPedFlags.bStayBehindIfLeaderTooFar = spd.m_nPedFlags.bStayBehindIfLeaderTooFar;
	LocalPlayer()->m_nPedFlags.bUsingExitCarTimer = spd.m_nPedFlags.bUsingExitCarTimer;
	LocalPlayer()->m_nPedFlags.bScriptPedIsPlayerAlly = spd.m_nPedFlags.bScriptPedIsPlayerAlly;
	LocalPlayer()->m_nPedFlags.bKnockedUpIntoAir = spd.m_nPedFlags.bKnockedUpIntoAir;
	LocalPlayer()->m_nPedFlags.bDeadPedInFrontOfCar = spd.m_nPedFlags.bDeadPedInFrontOfCar;
	LocalPlayer()->m_nPedFlags.bStayInCarOnJack = spd.m_nPedFlags.bStayInCarOnJack;

	LocalPlayer()->m_nPedFlags.b89 = spd.m_nPedFlags.b89;
	LocalPlayer()->m_nPedFlags.bDoomAim = spd.m_nPedFlags.bDoomAim;
	LocalPlayer()->m_nPedFlags.bCanBeShotInVehicle = spd.m_nPedFlags.bCanBeShotInVehicle;
	LocalPlayer()->m_nPedFlags.b92 = spd.m_nPedFlags.b92;
	LocalPlayer()->m_nPedFlags.b93 = spd.m_nPedFlags.b93;
	LocalPlayer()->m_nPedFlags.b94 = spd.m_nPedFlags.b94;
	LocalPlayer()->m_nPedFlags.b95 = spd.m_nPedFlags.b95;
	LocalPlayer()->m_nPedFlags.b96 = spd.m_nPedFlags.b96;

	LocalPlayer()->m_nPedFlags.b97 = spd.m_nPedFlags.b97;
	LocalPlayer()->m_nPedFlags.bCrouchWhenScared = spd.m_nPedFlags.bCrouchWhenScared;
	LocalPlayer()->m_nPedFlags.b99 = spd.m_nPedFlags.b99;
	LocalPlayer()->m_nPedFlags.b100 = spd.m_nPedFlags.b100;
	LocalPlayer()->m_nPedFlags.b101 = spd.m_nPedFlags.b101;
	LocalPlayer()->m_nPedFlags.b102 = spd.m_nPedFlags.b102;
	LocalPlayer()->m_nPedFlags.bPedWasSetOutOfCollision = spd.m_nPedFlags.bPedWasSetOutOfCollision;
	LocalPlayer()->m_nPedFlags.bGangMemberReturnsFire = spd.m_nPedFlags.bGangMemberReturnsFire;
}
void CClientNetwork::ReceiveSPDUpdate(librg_message_t* msg)
{
	PlayerSyncData spd;
	librg_data_rptr(msg->data, &spd, sizeof(PlayerSyncData));

	SyncLocalPlayer(spd);
}
void CClientNetwork::AttemptConnect(char* szAddress, int iPort) 
{
	CHooks::InitPool(CPools::ms_pPedPool, MAX_PEDS);
	CHooks::InitPool(CPools::ms_pVehiclePool, MAX_VEHICLES);

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

	librg_network_add(&ctx, VCOOP_START_MISSION_SCRIPT, ClientStartMissionScript);
	librg_network_add(&ctx, VCOOP_RECEIVE_MESSAGE,		ClientReceiveMessage);
	librg_network_add(&ctx, VCOOP_RESPAWN_AFTER_DEATH,	PlayerSpawnEvent);
	librg_network_add(&ctx, VCOOP_GET_LUA_SCRIPT,		ClientReceiveScript);
	librg_network_add(&ctx, VCOOP_DISCONNECT,			ClientDisconnect);
	librg_network_add(&ctx, VCOOP_CONNECT,				ClientConnect);
	librg_network_add(&ctx, VCOOP_SPAWN_ALLOWED,		ClientSpawnAllowed);
	librg_network_add(&ctx, VCOOP_BULLET_SYNC,			BulletSyncEvent);
	librg_network_add(&ctx, VCOOP_RECEIVE_SPD_UPDATE,	ReceiveSPDUpdate);
	

	addr.host = szAddress;
	addr.port = iPort;

	gLog->Log("[CClientNetwork] Attempting to connect to %s:%d\n", addr.host, addr.port);
	librg_network_start(&ctx, addr);

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ClientConnectThread, NULL, 0, NULL);
}
