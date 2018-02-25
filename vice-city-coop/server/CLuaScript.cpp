#include "server.h"

int lua_Log(lua_State* L) {
	int nargs = lua_gettop(L);

	std::string buffer("[Gamemode] ");
	for (int i = 1; i <= nargs; ++i) {
		buffer.append((char*)lua_tostring(L, i));
	}
	buffer.append("\n");
	gLog->Log((char*)buffer.c_str());
	return 0;
}

static const struct luaL_Reg printlib[] = {
	{ "print", lua_Log },
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
void CLuaScript::CallCallback(std::string callback)
{
	if (!m_lState)
	{
		InitializeLua();
	}

	if (luaL_loadbuffer(m_lState, m_Data->GetData(), m_Data->GetSize() - 1, "vccoop_server_gamemode") || lua_pcall(m_lState, 0, 0, 0))
	{
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
		if (lua_pcall(m_lState, 0, 0, 0) != 0) {
			gLog->Log("[CLuaScript] Error running callback `%s': %s\n", callback.c_str(), lua_tostring(m_lState, -1));
			return;
		}
		else
		{
			gLog->Log("[CLuaScript] Call to %s callback successful.\n", callback.c_str());
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
	luaL_setfuncs(lState, printlib, 0);
	lua_pop(lState, 1);

	m_lState = lState;
}