#include "server.h"

static const struct luaL_Reg vccooplib[] = {
	{ "print",				&CLuaScript::lua_Log },
	{ "sleep",				&CLuaScript::lua_Sleep },

	{ "SendGlobalMessage",	&CLuaScript::lua_SendGlobalMessage },

	{ "GetPlayerPos",		&CLuaScript::lua_GetPlayerPos },

	{ "GetPlayerHealth",	&CLuaScript::lua_GetPlayerHealth },

	{ "GetRandomModel",		&CLuaScript::lua_GetRandomModel },
	{ "GetPlayerName",		&CLuaScript::lua_GetPlayerName },

	{ "AddObject",			&CLuaScript::lua_AddObject },
	{ "AddVehicle",			&CLuaScript::lua_AddVehicle },
	{ "AddPed",				&CLuaScript::lua_AddPed },

	{ NULL, NULL }
};

CLuaScript::CLuaScript(CCustomData* ptr) 
{
	if (!ptr)
		return;	

	m_Data = ptr;
}
int CLuaScript::lua_SendGlobalMessage(lua_State* L)
{
	int nargs = lua_gettop(L);
	if (nargs < 1 || nargs == 0)
		return 0;
	
	char buffer[256];
	sprintf(buffer, "[Server] %s", lua_tolstring(L, 1, NULL));
	librg_message_send_all(&gServerNetwork->ctx, VCOOP_RECEIVE_MESSAGE, buffer, sizeof(buffer));
	return 0;
}
int CLuaScript::lua_GetRandomModel(lua_State* L)
{
	if (lua_gettop(L) == 1)
	{
		if (strstr(lua_tostring(L, 1), "vehicle")) {
			lua_pushnumber(L, CModelIDs::GetRandomModel(MODEL_VEH));
			return 1;
		}
		else if (strstr(lua_tostring(L, 1), "object")) {
			lua_pushnumber(L, CModelIDs::GetRandomModel(MODEL_OBJ));
			return 1;
		}
	}
	else
	{
		lua_pushnumber(L, CModelIDs::GetRandomModel(MODEL_PED));
		return 1;
	}
	return 0;
}
int CLuaScript::lua_GetPlayerHealth(lua_State* L)
{
	if (lua_gettop(L) == 1) {
		librg_entity_t* entity = librg_entity_fetch(&gServerNetwork->ctx, lua_tonumber(L, 1));
		if (entity && entity->type == VCOOP_PLAYER) {
			lua_pushnumber(L, ((*(PlayerSyncData*)entity->user_data).Health));
			return 1;
		}
	}
	return 0;
}
int CLuaScript::lua_GetPlayerPos(lua_State* L)
{
	if (lua_gettop(L) == 1) {
		librg_entity_t* entity = librg_entity_fetch(&gServerNetwork->ctx, lua_tonumber(L, 1));

		if (entity && entity->type == VCOOP_PLAYER) {
			lua_pushnumber(L, entity->position.x);
			lua_pushnumber(L, entity->position.y);
			lua_pushnumber(L, entity->position.z);
			return 3;
		}
		return 0;
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
	lua_pushnumber(L, entity->id);
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

	entity->position.x = position.x;
	entity->position.y = position.y;
	entity->position.z = position.z;

	gLog->Log("[PedCreate] Created ped with ID: %d\n", entity->id);

	lua_pushnumber(L, entity->id);

	return 1;
}
int CLuaScript::lua_AddVehicle(lua_State* L)
{
	int nargs = lua_gettop(L);
	if (nargs < 4 || nargs > 4)
		return 0;

	CVector position;
	float x, y, z;
	int modelID;

	modelID		= lua_tointeger(L, 1);
	if (!CModelIDs::IsValidVehicleModel(modelID))	{
		gLog->Log("[CLuaScript] %d is an invalid vehicle model ID!\n", modelID);
		return 0;
	}

	x			= lua_tonumber(L, 2);
	y			= lua_tonumber(L, 3);
	z			= lua_tonumber(L, 4);
	position	= CVector(x, y, z);

	librg_entity_t* entity = librg_entity_create(&gServerNetwork->ctx, VCOOP_VEHICLE);
	entity->user_data = new VehicleSyncData();
	((VehicleSyncData*)entity->user_data)->driver = -1;
	((VehicleSyncData*)entity->user_data)->vehicleID = -1;
	((VehicleSyncData*)entity->user_data)->modelID = modelID;

	entity->position.x = position.x;
	entity->position.y = position.y;
	entity->position.z = position.z;

	gLog->Log("[VehCreate] Created vehicle with ID: %d\n", entity->id);
	
	lua_pushnumber(L, entity->id);

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