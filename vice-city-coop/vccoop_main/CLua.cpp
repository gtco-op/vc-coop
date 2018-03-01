#include "main.h"

static const struct luaL_Reg vccooplib[] = {
	{ "print", CLua::lua_Log },
	{ "FindLocalPed", CLua::lua_FindLocalPed },
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
void CLua::LuaThread(LPVOID lParam)
{
	CLua* state = (CLua*)lParam;

	if (state == nullptr)
		return;

	lua_State* tmp = state->GetState();

	luaL_loadbuffer(tmp, state->mainScript, (size_t)state->mainScriptSize, "vccoop_lua_script");
	lua_pcall(tmp, NULL, 0, NULL);

	state->SetLuaStatus(FALSE);
	lua_close(state->GetState());

	delete state;
}
void CLua::CreateLuaThread()
{
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&LuaThread, this, 0, NULL);
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