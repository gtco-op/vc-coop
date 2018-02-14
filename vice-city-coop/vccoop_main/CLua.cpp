#include "main.h"

static const struct luaL_Reg printlib[] = {
	{ "print", CLua::lua_Log },
	{ NULL, NULL }
};

CLua::CLua()
{
	gLog->Log("[CLua] Initializing CLua\n");

	this->bActive	= false;
	this->lState	= luaL_newstate();

	luaL_openlibs(this->lState);
	lua_getglobal(this->lState, "_G");
	luaL_setfuncs(this->lState, printlib, 0);
	lua_pop(this->lState, 1);

	lua_register(this->lState, "FindPlayerPed",	CLua::lua_FindPlayerPed);

	gLog->Log("[CLua] Lua state initialized\n");
}
CLua::~CLua()
{
	gLog->Log("[CLua] CLua shutting down\n");
}
void CLua::LuaThread(LPVOID lParam)
{
	CLua* state = (CLua*)lParam;

	if (state == nullptr)
		return;

	luaL_dostring(state->GetState(), state->mainScript);
	state->SetLuaStatus(FALSE);

	lua_close(state->GetState());
	gLog->Log("[CLua] Lua script finished\n");

	delete state;
}
void CLua::CreateLuaThread()
{
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&LuaThread, this, 0, NULL);
}
int CLua::lua_Log(lua_State* L) {
	int nargs = lua_gettop(L);

	std::string buffer("[CLua][Script Output] ");
	for (int i = 1; i <= nargs; ++i) {
		buffer.append((char*)lua_tostring(L, i));
	}
	buffer.append("\n");
	gLog->Log((char*)buffer.c_str());
	return 0;
}
int CLua::lua_FindPlayerPed(lua_State* L)
{
	bool ret = false;

	if (FindPlayerPed())
		ret = true;

	lua_pushboolean(L, ret);
	return 1;
}