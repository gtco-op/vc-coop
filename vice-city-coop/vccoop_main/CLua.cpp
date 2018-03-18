#include "main.h"

static const struct luaL_Reg vccooplib[] = {
	{ "print", CLua::lua_Log },
	{ "FindLocalPed", CLua::lua_FindLocalPed },
	{ "DrawText", CLua::lua_DrawText },
	{"Announce", CLua::lua_ann},
	{ NULL, NULL }
};

CLua::CLua()
{
	gLog->Log("[CLua] Initializing CLua\n");

	this->bActive	= false;
	this->lState	= luaL_newstate();

	luaL_openlibs(this->lState);
	lua_getglobal(this->lState, "_G");
	luaL_setfuncs(this->lState, vccooplib, 0);
	lua_pop(this->lState, 1);

}
CLua::~CLua()
{
	gLog->Log("[CLua] CLua shutting down\n");
}
void CLua::Call(std::string callback, char *fmt, ...)
{
	m_CallBackName = callback;
	if (m_CallBackName.empty())
		return;

	lState = luaL_newstate();
	if (lState == nullptr)
		return;

	m_Type = 1;

	luaL_openlibs(lState);
	lua_getglobal(lState, "_G");
	luaL_setfuncs(lState, vccooplib, 0);
	lua_pop(lState, 1);

	if (luaL_loadbuffer(lState, m_Data->GetData(), m_Data->GetSize() - 1, m_Data->GetName().c_str()) || lua_pcall(lState, 0, 0, 0))
	{
#ifdef VCCOOP_LUA_VERBOSE_LOG
		gLog->Log("[CLuaScript] Could not load script buffer when calling callback %s.\n", GetCallbackName().c_str());
#endif
		lua_close(lState);
		return;
	}
	else
	{
		lua_getglobal(lState, m_CallBackName.c_str());
		if (!lua_isfunction(lState, -1))
		{
			lua_pop(lState, 1);
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
				lua_pushinteger(lState, i);
				m_Args++;
			}
			else if (*fmt == 's' || *fmt == 'c') {
				char * s = va_arg(args, char*);
				lua_pushstring(lState, s);
				m_Args++;
			}
			else if (*fmt == 'f') {
				float f = va_arg(args, float);
				lua_pushnumber(lState, f);
				m_Args++;
			}
			++fmt;
		}
		va_end(args);
	}

	CreateLuaThread();
}
void CLua::LuaThread(LPVOID lParam)
{
	CLua* state = (CLua*)lParam;

	if (state == nullptr)
		return;

	if (state->m_Type == 1)
	{
		lua_State* lState = state->lState;

		if (lua_pcall(lState, state->m_Args, 0, 0) != 0) {
#ifdef VCCOOP_LUA_VERBOSE_LOG
			gLog->Log("[CLuaScript] Error running callback `%s': %s\n", state->m_CallBackName.c_str(), lua_tostring(lState, -1));
#endif
			return;
		}
		else
		{
#ifdef VCCOOP_LUA_VERBOSE_LOG
			gLog->Log("[CLuaScript] Call to %s callback successful.\n", state->m_CallBackName.c_str());
#endif
		}

		state->m_Args = 0;
	}
	else
	{
		lua_State* tmp = state->GetState();

		luaL_loadbuffer(tmp, state->mainScript, (size_t)state->mainScriptSize, "vccoop_lua_script");
		lua_pcall(tmp, NULL, 0, NULL);

		state->SetLuaStatus(FALSE);
		lua_close(state->GetState());
	}

	delete state;
}
void CLua::CreateLuaThread()
{
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&LuaThread, this, 0, NULL);
}
int CLua::lua_DrawText(lua_State* L)
{
	int nargs = lua_gettop(L);
	if (nargs < 3)
		return 0;

	std::string buffer("");
	buffer.append((char*)lua_tostring(L, 1));
	
	int x = lua_tonumber(L, 2), y = lua_tonumber(L, 3);
	if (!buffer.empty())	{
		RECT rect;
		rect.top = x;
		rect.left = y;		
		gRender->RenderText(buffer.c_str(), rect, 0xFFFFFF);
	}

	return 0;
}
int CLua::lua_Log(lua_State* L) {
	int nargs = lua_gettop(L);

	std::string buffer("[Server] ");
	for (int i = 1; i <= nargs; ++i) {
		buffer.append((char*)lua_tostring(L, i));
	}
	buffer.append("\n");
	gLog->Log((char*)buffer.c_str());
	return 0;
}
int CLua::lua_FindLocalPed(lua_State* L)
{
	bool ret = false;

	if (LocalPlayer())
		ret = true;

	lua_pushboolean(L, ret);
	return 1;
}
int CLua::lua_ann(lua_State* L) {
	int nargs = lua_gettop(L);
	if (nargs < 2) return 0;
	int ann_type(lua_tointeger(L, 1));
	std::string mess(lua_tostring(L, 2));

	switch (ann_type) {
	case 0:
		CHud::SetHelpMessage((char*)mess.c_str(), 0, 0, 0);
		break;
	case 2:
		CHud::SetVehicleName((char*)mess.c_str());
		break;
	case 3:
		CHud::SetZoneName((char*)mess.c_str());
		break;
	default:
		gLog->Log("[SCRIPT] Error Choose a specific ID [0 to 3]");
	}


	return 1;
}