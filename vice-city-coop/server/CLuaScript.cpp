#include "server.h"

static const struct luaL_Reg vccooplib[] = {
	{ "print", &CLuaScript::lua_Log },
	{ "sleep", &CLuaScript::lua_Sleep },
{ NULL, NULL }
};

CLuaScript::CLuaScript(CCustomData* ptr) 
{
	if (!ptr)
		return;

	m_Data = ptr;
	
	InitializeLua();

	CallCallback("onServerStart");
}
void CLuaScript::CallCallback(std::string callback, int args, ...)
{
	if (!m_lState)
	{
		InitializeLua();
	}

	m_Args = args;

	if (luaL_loadbuffer(m_lState, m_Data->GetData(), m_Data->GetSize() - 1, "vccoop_server_gamemode") || lua_pcall(m_lState, 0, 0, 0))
	{
		gLog->Log("[CLuaScript] Could not load script buffer when calling callback %s.\n", callback.c_str());
		lua_close(m_lState);
		return;
	}
	else
	{
		lua_getglobal(m_lState, callback.c_str());
		if (!lua_isfunction(m_lState, -1))
		{
			lua_pop(m_lState, 1);
			gLog->Log("[CLuaScript] Could not find %s callback.\n", callback.c_str());
			return;
		}
		if (args == 0)
		{
			if (lua_pcall(m_lState, 0, 0, 0) != 0) {
				gLog->Log("[CLuaScript] Error running callback `%s': %s\n", callback.c_str(), lua_tostring(m_lState, -1));
				return;
			}
			else
			{
#ifdef VCCOOP_VERBOSE_LOG
				gLog->Log("[CLuaScript] Call to %s callback successful.\n", callback.c_str());
#endif
			}
		}
		else
		{
			va_list arguments;        
			va_start(arguments, args);           
			for (int x = 0; x < args; x++)
			{
				lua_pushstring(m_lState, va_arg(arguments, char*));
			}
			va_end(arguments);                  

			if (lua_pcall(m_lState, args, 0, 0) != 0) {
				gLog->Log("[CLuaScript] Error running callback `%s': %s\n", callback.c_str(), lua_tostring(m_lState, -1));
				return;
			}
			else
			{
#ifdef VCCOOP_VERBOSE_LOG
				gLog->Log("[CLuaScript] Call to %s callback successful.\n", callback.c_str());
#endif
			}
		}
	}
}
void CLuaScript::InitializeLua()
{
	lua_State* lState = luaL_newstate();
	if (lState == nullptr)
		return;

	luaL_openlibs(lState);
	lua_getglobal(lState, "_G");
	luaL_setfuncs(lState, vccooplib, 0);
	lua_pop(lState, 1);

	m_lState = lState;
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
	Sleep(ms);
	return 1;
}
