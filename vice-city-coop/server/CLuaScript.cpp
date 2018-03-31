#include "server.h"

static const struct luaL_Reg vccooplib[] = {
	{ "print",					&CLuaScript::lua_Log },
	{ "sleep",					&CLuaScript::lua_Sleep },

	{ "SendGlobalMessage",		&CLuaScript::lua_SendGlobalMessage },
	
	{ "IsEntityValid",			&CLuaScript::lua_IsEntityValid },

	{ "GetEntityType",			&CLuaScript::lua_GetEntityType },

	{ "GetEntityPos",			&CLuaScript::lua_GetEntityPos },
	{ "SetEntityPos",			&CLuaScript::lua_SetEntityPos },

	{ "GetEntityOrientation",	&CLuaScript::lua_GetEntityOrientation },
	{ "SetEntityOrientation",	&CLuaScript::lua_SetEntityOrientation },

	{ "GetEntityControlPeer",	&CLuaScript::lua_GetEntityControlPeer },
	{ "SetEntityControlPeer",	&CLuaScript::lua_SetEntityControlPeer },
	{ "RemoveEntityControl",	&CLuaScript::lua_RemoveEntityControl },

	{ "GivePlayerWeapon",		&CLuaScript::lua_GivePlayerWeapon },

	{ "GetPlayerModel",			&CLuaScript::lua_GetPlayerModel },
	{ "SetPlayerModel",			&CLuaScript::lua_SetPlayerModel },

	{ "GetPlayerHealth",		&CLuaScript::lua_GetPlayerHealth },
	{ "SetPlayerHealth",		&CLuaScript::lua_SetPlayerHealth },

	{ "GetPlayerArmour",		&CLuaScript::lua_GetPlayerArmour },
	{ "SetPlayerArmour",		&CLuaScript::lua_SetPlayerArmour },

	{ "GetVehicleHealth",		&CLuaScript::lua_GetVehicleHealth },
	{ "SetVehicleHealth",		&CLuaScript::lua_SetVehicleHealth },

	{ "GetRandomModel",			&CLuaScript::lua_GetRandomModel },
	{ "GetRandomVehicleColor",	&CLuaScript::lua_GetRandomVehicleColor },

	{ "GetPlayerName",			&CLuaScript::lua_GetPlayerName },

	{ "StartMissionScript",		&CLuaScript::lua_StartMissionScript },

	{ "AddObject",				&CLuaScript::lua_AddObject },
	{ "AddVehicle",				&CLuaScript::lua_AddVehicle },
	{ "AddPed",					&CLuaScript::lua_AddPed },

	{ NULL, NULL }
};

CLuaScript::CLuaScript(CCustomData* ptr) 
{
	if (!ptr)
		return;	

	m_Data = ptr;
}
int CLuaScript::lua_GivePlayerWeapon(lua_State* L)
{
	if (lua_gettop(L) == 3) {
		librg_entity_t* entity = librg_entity_fetch(&gServerNetwork->ctx, lua_tointeger(L, 1));
		if (entity && entity->type == VCOOP_PLAYER && CServerNetwork::GetPlayerSyncData(entity->id) != nullptr) {
			int model = lua_tointeger(L, 2);
			int ammo = lua_tointeger(L, 3);

			CServerNetwork::GetPlayerSyncData(entity->id)->CurrWep = model;
			CServerNetwork::GetPlayerSyncData(entity->id)->Ammo = ammo;
			CServerNetwork::GetPlayerSyncData(entity->id)->WepModelIndex = CModelIDs::GetWeaponModelFromType(model);
			CServerNetwork::SetPlayerSyncData(entity->id, *CServerNetwork::GetPlayerSyncData(entity->id));
		}
	}
	return 0;
}
int CLuaScript::lua_IsEntityValid(lua_State* L)
{
	if (lua_gettop(L) == 1)	{
		librg_entity_t* entity = librg_entity_fetch(&gServerNetwork->ctx, lua_tointeger(L, 1));
		if (entity) {
			lua_pushboolean(L, true);
		}
		else		{
			lua_pushboolean(L, false);
		}
		return 1;
	}
	else return 0;
}

int CLuaScript::lua_GetEntityControlPeer(lua_State* L)
{
	if (lua_gettop(L) == 1)	{
		librg_entity_t* entity = librg_entity_fetch(&gServerNetwork->ctx, lua_tonumber(L, 1));
		if (entity) {
			librg_peer_t* controlPeer = librg_entity_control_get(&gServerNetwork->ctx, entity->id);
			librg_entity_t* controlEntity = librg_entity_find(&gServerNetwork->ctx, controlPeer);
			if (controlEntity) {
				lua_pushnumber(L, controlEntity->id);
				return 1;
			}
			else return 0;
		}
		else return 0;
	}
	else return 0;
}
int CLuaScript::lua_SetEntityControlPeer(lua_State* L)
{
	if (lua_gettop(L) == 2)	{
		librg_entity_t* entity = librg_entity_fetch(&gServerNetwork->ctx, lua_tonumber(L, 1));
		librg_entity_t* controlEntity = librg_entity_fetch(&gServerNetwork->ctx, lua_tonumber(L, 2));
		if (entity && controlEntity)		{
			librg_entity_control_set(&gServerNetwork->ctx, entity->id, controlEntity->client_peer);
		}
	}
	return 0;
}
int CLuaScript::lua_RemoveEntityControl(lua_State* L)
{
	if (lua_gettop(L) == 1)	{
		librg_entity_t* entity = librg_entity_fetch(&gServerNetwork->ctx, lua_tonumber(L, 1));
		if (entity) {
			librg_entity_control_remove(&gServerNetwork->ctx, entity->id);
		}
	}
	return 0;
}
int CLuaScript::lua_GetEntityType(lua_State *L)
{
	if (lua_gettop(L) == 1)
	{
		librg_entity_t* entity = librg_entity_fetch(&gServerNetwork->ctx, lua_tonumber(L, 1));
		if (entity && (entity->type == VCOOP_PLAYER || entity->type == VCOOP_VEHICLE || entity->type == VCOOP_PED || entity->type == VCOOP_OBJECT))		{
			switch (entity->type)			{
			case VCOOP_PLAYER:
				lua_pushinteger(L, 1);
				if (librg_entity_control_get(&gServerNetwork->ctx, entity->id))				{
					librg_entity_t* control_entity = librg_entity_find(&gServerNetwork->ctx, librg_entity_control_get(&gServerNetwork->ctx, entity->id));
					if (control_entity)					{
						lua_pushinteger(L, control_entity->id);
					} else	{
						lua_pushinteger(L, -1);
					}
					return 2;
				}
				else return 1;
				break;
			case VCOOP_PED:
				lua_pushinteger(L, 2);
				if (librg_entity_control_get(&gServerNetwork->ctx, entity->id)) {
					librg_entity_t* control_entity = librg_entity_find(&gServerNetwork->ctx, librg_entity_control_get(&gServerNetwork->ctx, entity->id));
					if (control_entity) {
						lua_pushinteger(L, control_entity->id);
					}
					else {
						lua_pushinteger(L, -1);
					}
					return 2;
				}
				else return 1;
				break;
			case VCOOP_VEHICLE:
				lua_pushinteger(L, 3);
				if (librg_entity_control_get(&gServerNetwork->ctx, entity->id)) {
					librg_entity_t* control_entity = librg_entity_find(&gServerNetwork->ctx, librg_entity_control_get(&gServerNetwork->ctx, entity->id));
					if (control_entity) {
						lua_pushinteger(L, control_entity->id);
					}
					else {
						lua_pushinteger(L, -1);
					}
					return 2;
				}
				else return 1;
				break;
			case VCOOP_OBJECT:
				lua_pushinteger(L, 4);
				if (librg_entity_control_get(&gServerNetwork->ctx, entity->id)) {
					librg_entity_t* control_entity = librg_entity_find(&gServerNetwork->ctx, librg_entity_control_get(&gServerNetwork->ctx, entity->id));
					if (control_entity) {
						lua_pushinteger(L, control_entity->id);
					}
					else {
						lua_pushinteger(L, -1);
					}
					return 2;
				}
				else return 1;
				break;
			}
			return 0;
		}
		else return 0;
	}
	else return 0;
}
int CLuaScript::lua_GetEntityPos(lua_State* L)
{
	if (lua_gettop(L) == 1) {
		librg_entity_t* entity = librg_entity_fetch(&gServerNetwork->ctx, lua_tonumber(L, 1));
		if (entity && (entity->type == VCOOP_PLAYER || entity->type == VCOOP_VEHICLE || entity->type == VCOOP_PED || entity->type == VCOOP_OBJECT)) {
			lua_pushnumber(L, entity->position.x);
			lua_pushnumber(L, entity->position.y);
			lua_pushnumber(L, entity->position.z);
			return 3;
		}
		return 0;
	}
	return 0;
}
int CLuaScript::lua_SetEntityPos(lua_State* L)
{
	if (lua_gettop(L) == 4) {
		librg_entity_t* entity = librg_entity_fetch(&gServerNetwork->ctx, lua_tonumber(L, 1));
		float X = lua_tonumber(L, 2), Y = lua_tonumber(L, 3), Z = lua_tonumber(L, 4);
		if (entity && (entity->type == VCOOP_PLAYER || entity->type == VCOOP_VEHICLE || entity->type == VCOOP_PED || entity->type == VCOOP_OBJECT))		{
			if (librg_entity_control_get(&gServerNetwork->ctx, lua_tonumber(L, 1)) != nullptr)
				librg_entity_control_remove(&gServerNetwork->ctx, entity->id);
			entity->position.x = X;
			entity->position.y = Y;
			entity->position.z = Z;
		}
		return 0;
	}
	return 0;
}
int CLuaScript::lua_GetEntityOrientation(lua_State* L)
{
	if (lua_gettop(L) == 1)	{
		librg_entity_t* entity = librg_entity_fetch(&gServerNetwork->ctx, lua_tonumber(L, 1));
		if (entity && ((entity->type == VCOOP_PLAYER || entity->type == VCOOP_VEHICLE || entity->type == VCOOP_PED))) {
			switch (entity->type) {
				case VCOOP_PLAYER: 
				{
					PlayerSyncData spd = *(PlayerSyncData*)entity->user_data;

					lua_pushnumber(L, spd.OrientX);
					lua_pushnumber(L, spd.OrientY);
					lua_pushnumber(L, spd.OrientZ);
					break;
				}
				case VCOOP_PED: 
				{
					PedSyncData spd = *(PedSyncData*)entity->user_data;

					lua_pushnumber(L, spd.OrientX);
					lua_pushnumber(L, spd.OrientY);
					lua_pushnumber(L, spd.OrientZ);
					break;
				}
				case VCOOP_VEHICLE:
				{
					VehicleSyncData spd = *(VehicleSyncData*)entity->user_data;

					lua_pushnumber(L, spd.OrientX);
					lua_pushnumber(L, spd.OrientY);
					lua_pushnumber(L, spd.OrientZ);
					break;
				}
			}
			return 3;
		}
		else return 0;
	}
	else return 0;
}
int CLuaScript::lua_SetEntityOrientation(lua_State* L)
{
	if (lua_gettop(L) == 4) {
		librg_entity_t* entity = librg_entity_fetch(&gServerNetwork->ctx, lua_tonumber(L, 1));
		if (entity && ((entity->type == VCOOP_PLAYER || entity->type == VCOOP_VEHICLE || entity->type == VCOOP_PED))) {
			if (librg_entity_control_get(&gServerNetwork->ctx, entity->id))
				if (librg_entity_find(&gServerNetwork->ctx, librg_entity_control_get(&gServerNetwork->ctx, entity->id)))
					librg_entity_control_remove(&gServerNetwork->ctx, entity->id);

			float	X = lua_tonumber(L, 2), 
					Y = lua_tonumber(L, 3), 
					Z = lua_tonumber(L, 4);

			switch (entity->type) {
				case VCOOP_PLAYER:
				{
					PlayerSyncData spd = *(PlayerSyncData*)entity->user_data;

					spd.OrientX = X;
					spd.OrientY = Y;
					spd.OrientZ = Z;
					break;
				}
				case VCOOP_PED: 
				{
					PedSyncData spd = *(PedSyncData*)entity->user_data;

					spd.OrientX = X;
					spd.OrientY = Y;
					spd.OrientZ = Z;
					break;
				}
				case VCOOP_VEHICLE:
				{
					VehicleSyncData spd = *(VehicleSyncData*)entity->user_data;

					spd.OrientX = X;
					spd.OrientY = Y;
					spd.OrientZ = Z;
					break;
				}
			}
		}
	}
	return 0;
}
int CLuaScript::lua_SendGlobalMessage(lua_State* L)
{
	int nargs = lua_gettop(L);
	if (nargs < 1 || nargs == 0)
		return 0;
	
	char buffer[256];
	sprintf(buffer, "%s", lua_tolstring(L, 1, NULL));
	librg_message_send_all(&gServerNetwork->ctx, VCOOP_RECEIVE_MESSAGE, buffer, sizeof(buffer));
	return 0;
}
int CLuaScript::lua_GetRandomModel(lua_State* L)
{
	if (lua_gettop(L) == 1)
	{
		if (strstr(lua_tostring(L, 1), "vehicle")) {
			lua_pushinteger(L, CModelIDs::GetRandomModel(MODEL_VEH));
			return 1;
		}
		else if (strstr(lua_tostring(L, 1), "object")) {
			lua_pushinteger(L, CModelIDs::GetRandomModel(MODEL_OBJ));
			return 1;
		}
	}
	else
	{
		lua_pushinteger(L, CModelIDs::GetRandomModel(MODEL_PED));
		return 1;
	}
	return 0;
}
int CLuaScript::lua_GetRandomVehicleColor(lua_State* L)
{
	lua_pushinteger(L, CModelIDs::GetRandomVehicleColor());
	return 1;
}
int CLuaScript::lua_GetPlayerModel(lua_State* L)
{
	if (lua_gettop(L) == 1) {
		librg_entity_t* entity = librg_entity_fetch(&gServerNetwork->ctx, lua_tonumber(L, 1));
		if (entity && entity->type == VCOOP_PLAYER) {
			lua_pushnumber(L, CServerNetwork::GetPlayerSyncData(entity->id)->iModelIndex);
			return 1;
		}
	}
	return 0;
}
int CLuaScript::lua_SetPlayerModel(lua_State* L)
{
	if (lua_gettop(L) == 2) {
		librg_entity_t* entity = librg_entity_fetch(&gServerNetwork->ctx, lua_tointeger(L, 1));
		if (entity && entity->type == VCOOP_PLAYER && CServerNetwork::GetPlayerSyncData(entity->id) != nullptr) {
			int model = lua_tointeger(L, 2);
			
			if (CModelIDs::IsValidPedModel(model))
			{
				CServerNetwork::GetPlayerSyncData(entity->id)->iModelIndex = model;
				CServerNetwork::SetPlayerSyncData(entity->id, *CServerNetwork::GetPlayerSyncData(entity->id));
			}
		}
	}
	return 0;
}
int CLuaScript::lua_GetPlayerHealth(lua_State* L)
{
	if (lua_gettop(L) == 1) {
		librg_entity_t* entity = librg_entity_fetch(&gServerNetwork->ctx, lua_tonumber(L, 1));
		if (entity && entity->type == VCOOP_PLAYER) {
			lua_pushnumber(L, CServerNetwork::GetPlayerSyncData(entity->id)->Health);
			return 1;
		}
	}
	return 0;
}
int CLuaScript::lua_SetPlayerHealth(lua_State* L)
{
	if (lua_gettop(L) == 2)	{
		librg_entity_t* entity = librg_entity_fetch(&gServerNetwork->ctx, lua_tointeger(L, 1));
		if (entity && entity->type == VCOOP_PLAYER && CServerNetwork::GetPlayerSyncData(entity->id) != nullptr)		{
			float health = lua_tonumber(L, 2);
			CServerNetwork::GetPlayerSyncData(entity->id)->Health = health;

			CServerNetwork::SetPlayerSyncData(entity->id, *CServerNetwork::GetPlayerSyncData(entity->id));
		}
	}
	return 0;
}
int CLuaScript::lua_GetPlayerArmour(lua_State* L)
{
	if (lua_gettop(L) == 1) {
		librg_entity_t* entity = librg_entity_fetch(&gServerNetwork->ctx, lua_tonumber(L, 1));
		if (entity && entity->type == VCOOP_PLAYER) {
			lua_pushnumber(L, CServerNetwork::GetPlayerSyncData(entity->id)->Armour);
			return 1;
		}
	}
	return 0;
}
int CLuaScript::lua_SetPlayerArmour(lua_State* L)
{
	if (lua_gettop(L) == 2) {
		librg_entity_t* entity = librg_entity_fetch(&gServerNetwork->ctx, lua_tointeger(L, 1));
		if (entity && entity->type == VCOOP_PLAYER && CServerNetwork::GetPlayerSyncData(entity->id) != nullptr) {
			float armour = lua_tonumber(L, 2);
			CServerNetwork::GetPlayerSyncData(entity->id)->Armour = armour;

			CServerNetwork::SetPlayerSyncData(entity->id, *CServerNetwork::GetPlayerSyncData(entity->id));
		}
	}
	return 0;
}
int CLuaScript::lua_GetVehicleHealth(lua_State* L)
{
	if (lua_gettop(L) == 1) {
		librg_entity_t* entity = librg_entity_fetch(&gServerNetwork->ctx, lua_tonumber(L, 1));
		if (entity && entity->type == VCOOP_VEHICLE) {
			lua_pushnumber(L, ((*(VehicleSyncData*)entity->user_data).Health));
			return 1;
		}
	}
	return 0;
}
int CLuaScript::lua_SetVehicleHealth(lua_State* L)
{
	if (lua_gettop(L) == 2) {
		librg_entity_t* entity = librg_entity_fetch(&gServerNetwork->ctx, lua_tointeger(L, 1));
		if (entity && entity->type == VCOOP_VEHICLE) {
			VehicleSyncData* spd = (VehicleSyncData*)entity->user_data;
			if (spd) {
				librg_peer_t* peer = librg_entity_control_get(&gServerNetwork->ctx, entity->id);
				if (peer)				{
					librg_entity_control_remove(&gServerNetwork->ctx, entity->id);
				}
				float health = lua_tonumber(L, 2);
				spd->Health = health;
			}
		}
	}
	return 0;
}
int CLuaScript::lua_StartMissionScript(lua_State* L)
{
	if (lua_gettop(L) == 0)	{
		librg_message_send_all(&gServerNetwork->ctx, VCOOP_START_MISSION_SCRIPT, 0, 0);
	}
	else if (lua_gettop(L) == 1)	{
		if(librg_entity_fetch(&gServerNetwork->ctx, lua_tonumber(L, 1))->client_peer)
			librg_message_send_to(&gServerNetwork->ctx, VCOOP_START_MISSION_SCRIPT, librg_entity_fetch(&gServerNetwork->ctx, lua_tonumber(L, 1))->client_peer, 0, 0);
	}
	return 0;
}
int CLuaScript::lua_AddObject(lua_State* L)
{
	int nargs = lua_gettop(L), objectID = -1;
	if (nargs < 4 || nargs > 4)
		return 0;

	CVector position;
	float x, y, z;

	objectID = lua_tointeger(L, 1);
	if (!CModelIDs::IsValidObjectModel(objectID)) {
		gLog->Log("[CLuaScript] %d is an invalid object model ID!\n", objectID);
		return 0;
	}

	x = lua_tonumber(L, 2);
	y = lua_tonumber(L, 3);
	z = lua_tonumber(L, 4);
	position = CVector(x, y, z);

	librg_entity_t* entity = librg_entity_create(&gServerNetwork->ctx, VCOOP_OBJECT);
	entity->user_data = new ObjectSyncData();
	((ObjectSyncData*)entity->user_data)->objectID = entity->id;
	((ObjectSyncData*)entity->user_data)->modelID = objectID;
	entity->position = *(zplm_vec3_t*)&position;

	gLog->Log("[ObjCreate] Created object with ID: %d\n", entity->id);
	
	lua_pushinteger(L, entity->id);
	otherEntities.push_back(entity);

	return 1;
}
int CLuaScript::lua_AddPed(lua_State* L)
{
	int nargs = lua_gettop(L);

	CVector position;
	float x, y, z;
	int modelID;
	bool wander = true;

	if (nargs == 3)
	{
		// only x,y,z was passed, so randomize the model
		x = lua_tonumber(L, 1);
		y = lua_tonumber(L, 2);
		z = lua_tonumber(L, 3);

		modelID = CModelIDs::GetRandomModel();
	}
	else
	{
		modelID = lua_tointeger(L, 1);
		if (!CModelIDs::IsValidPedModel(modelID)) {
			gLog->Log("[CLuaScript] %d is an invalid ped model ID!\n", modelID);
			return 0;
		}
		x = lua_tonumber(L, 2);
		y = lua_tonumber(L, 3);
		z = lua_tonumber(L, 4);

		if (nargs == 5)
			wander = false;
	}

	position = CVector(x, y, z);

	librg_entity_t* entity = librg_entity_create(&gServerNetwork->ctx, VCOOP_PED);
	entity->user_data = new PedSyncData();
	((PedSyncData*)entity->user_data)->iModelIndex	= modelID;
	((PedSyncData*)entity->user_data)->Wander		= wander;
	((PedSyncData*)entity->user_data)->Health = 100.f;

	entity->position.x = position.x;
	entity->position.y = position.y;
	entity->position.z = position.z;

	gLog->Log("[PedCreate] Created ped with ID: %d\n", entity->id);

	lua_pushinteger(L, entity->id);
	otherEntities.push_back(entity);

	return 1;
}
int CLuaScript::lua_AddVehicle(lua_State* L)
{
	int nargs = lua_gettop(L);
	if (nargs < 4 || nargs > 6)
		return 0;

	int modelID			= lua_tointeger(L, 1);
	CVector position	= CVector(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));

	if (!CModelIDs::IsValidVehicleModel(modelID))	{
		gLog->Log("[CLuaScript] %d is an invalid vehicle model ID!\n", modelID);
		return 0;
	}

	librg_entity_t* entity								= librg_entity_create(&gServerNetwork->ctx, VCOOP_VEHICLE);
	entity->user_data									= new VehicleSyncData();
	((VehicleSyncData*)entity->user_data)->driver		= -1;
	((VehicleSyncData*)entity->user_data)->vehicleID	= -1;
	((VehicleSyncData*)entity->user_data)->modelID		= modelID;

	if (nargs == 6)	{
		((VehicleSyncData*)entity->user_data)->nPrimaryColor	= lua_tonumber(L, 5);
		((VehicleSyncData*)entity->user_data)->nSecondaryColor	= lua_tonumber(L, 6);
	}

	entity->position.x = position.x;
	entity->position.y = position.y;
	entity->position.z = position.z;

	gLog->Log("[VehCreate] Created vehicle with ID: %d\n", entity->id);
	
	lua_pushinteger(L, entity->id);
	otherEntities.push_back(entity);

	return 1;
}

void CLuaScript::CreateLuaThread()
{
#if defined (_MSC_VER)
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&LuaThread, this, 0, NULL);
#else
	pthread_t threadId;
	pthread_create(&threadId, NULL, &CLuaScript::LuaThread, this);
#endif
}

#if defined (_MSC_VER)
void CLuaScript::LuaThread(LPVOID lParam)
#else
void *CLuaScript::LuaThread(void* lParam)
#endif
{
	CLuaScript* instance = (CLuaScript*)lParam;

	if (instance == nullptr || (instance->m_lState == nullptr))
#if !defined (_MSC_VER)
		return nullptr;
#else
		return;
#endif

	lua_State* lState = instance->m_lState;

	if (lua_pcall(lState, instance->GetArguments(), 0, 0) != 0) {
#ifdef VCCOOP_LUA_VERBOSE_LOG
		gLog->Log("[CLuaScript] Error running callback `%s': %s\n", instance->GetCallbackName().c_str(), lua_tostring(lState, -1));
#endif
#if !defined (_MSC_VER)
		return nullptr;
#else
		return;
#endif
	}
	else
	{
#ifdef VCCOOP_LUA_VERBOSE_LOG
		gLog->Log("[CLuaScript] Call to %s callback successful.\n", instance->GetCallbackName().c_str());
#endif
	}	

	instance->m_Args = 0;
}
void CLuaScript::Call(std::string callback, char *fmt, ...)
{
	m_CallBackName	= callback;
	if (m_CallBackName.empty())
		return;

	m_lState		= luaL_newstate();
	if (m_lState == nullptr)
		return;

	luaL_openlibs(m_lState);
	luaopen_base(m_lState);
	luaopen_table(m_lState);
	luaopen_io(m_lState);
	luaopen_string(m_lState);
	luaopen_math(m_lState);
	luaopen_debug(m_lState);
	lua_getglobal(m_lState, "_G");
	luaL_setfuncs(m_lState, vccooplib, 0);
	lua_pop(m_lState, 1);

	if (luaL_loadbuffer(m_lState, GetData()->GetData(), GetData()->GetSize() - 1, GetData()->GetName().c_str()) || lua_pcall(m_lState, 0, 0, 0))
	{
#ifdef VCCOOP_LUA_VERBOSE_LOG
		gLog->Log("[CLuaScript] Could not load script buffer when calling callback %s.\n", GetCallbackName().c_str());
#endif
		lua_close(m_lState);
		return;
	}
	else
	{
		lua_getglobal(m_lState, GetCallbackName().c_str());
		if (!lua_isfunction(m_lState, -1))
		{
			lua_pop(m_lState, 1);
#ifdef VCCOOP_LUA_VERBOSE_LOG
			gLog->Log("[CLuaScript] Could not find `%s' callback.\n", GetCallbackName().c_str());
#endif
			return;
		}
	}

	if (fmt != nullptr)
	{
		va_list args;
		va_start(args, fmt);
		while (*fmt != '\0') {
			if (*fmt == 'i' || *fmt == 'd') {
				int i = va_arg(args, int);
				lua_pushinteger(m_lState, i);
				m_Args++;
			}
			else if (*fmt == 's' || *fmt == 'c') {
				char * s = va_arg(args, char*);
				lua_pushstring(m_lState, s);
				m_Args++;
			}
			else if (*fmt == 'f') {
				float f = va_arg(args, float);
				lua_pushnumber(m_lState, f);
				m_Args++;
			}
			++fmt;
		}
		va_end(args);
	}

	CreateLuaThread();
}
int CLuaScript::lua_Log(lua_State* L) {
	int nargs = lua_gettop(L);

	std::string buffer("[Gamemode] ");
	for (int i = 1; i <= nargs; ++i) {
		buffer.append((char*)lua_tostring(L, i));
	}
	buffer.append("\n");
	gLog->Log((char*)buffer.c_str());
	return 0;
}
int CLuaScript::lua_Sleep(lua_State* l)
{
	int ms = 0;
	ms = lua_tointeger(l, 1);
	if (ms == 0) 
		return 0;

#if defined(_MSC_VER)
	Sleep(ms);
#else
	usleep(ms);
#endif

	return 1;
}
int CLuaScript::lua_GetPlayerName(lua_State* L)
{
	int args = lua_gettop(L);
	if (args < 0)	{
		return 0;
	}

	int playerID = 0;
	playerID = lua_tointeger(L, 1);
	
	if(playerNames[playerID] != nullptr)
		lua_pushstring(L, playerNames[playerID]);

	return 1;
}