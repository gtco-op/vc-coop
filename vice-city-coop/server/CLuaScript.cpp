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
	m_Data = ptr;
	
	lua_State* lState = luaL_newstate();
	if (lState == nullptr)
		return;

	luaL_openlibs(lState);
	lua_getglobal(lState, "_G");
	luaL_setfuncs(lState, printlib, 0);
	lua_pop(lState, 1);

	luaL_loadbuffer(lState, m_Data->GetData(), m_Data->GetSize()-1, "vccoop_server_gamemode");
	lua_pcall(lState, NULL, 0, NULL);

	lua_close(lState);
}