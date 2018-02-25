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

	Call("onServerStart");
}
void CLuaScript::CreateLuaThread()
{
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&LuaThread, this, 0, NULL);
}
void CLuaScript::LuaThread(LPVOID lParam)
{
	CLuaScript* instance = (CLuaScript*)lParam;
	lua_State* lState	 = luaL_newstate();

	if (lState == nullptr || instance == nullptr)
		return;

	luaL_openlibs(lState);
	lua_getglobal(lState, "_G");
	luaL_setfuncs(lState, vccooplib, 0);
	lua_pop(lState, 1);
	
	if (luaL_loadbuffer(lState, instance->GetData()->GetData(), instance->GetData()->GetSize() - 1, instance->GetData()->GetName().c_str()) || lua_pcall(lState, 0, 0, 0))
	{
		gLog->Log("[CLuaScript] Could not load script buffer when calling callback %s.\n", instance->GetCallbackName().c_str());
		lua_close(lState);
		return;
	}
	else
	{
		lua_getglobal(lState, instance->GetCallbackName().c_str());
		if (!lua_isfunction(lState, -1))
		{
			lua_pop(lState, 1);
			gLog->Log("[CLuaScript] Could not find %s callback.\n", instance->GetCallbackName().c_str());
			return;
		}
		if (instance->GetArguments() == 0)
		{
			if (lua_pcall(lState, 0, 0, 0) != 0) {
				gLog->Log("[CLuaScript] Error running callback `%s': %s\n", instance->GetCallbackName().c_str(), lua_tostring(lState, -1));
				return;
			}
			else
			{
#ifdef VCCOOP_VERBOSE_LOG
				gLog->Log("[CLuaScript] Call to %s callback successful.\n", instance->GetCallbackName().c_str());
#endif
			}
		}
		else
		{
			for (auto i : instance->m_ArgList)
			{
				lua_pushstring(lState, i.c_str());
			}
			if (lua_pcall(lState, instance->GetArguments(), 0, 0) != 0) {
				gLog->Log("[CLuaScript] Error running callback `%s': %s\n", instance->GetCallbackName().c_str(), lua_tostring(lState, -1));
				return;
			}
			else
			{
#ifdef VCCOOP_VERBOSE_LOG
				gLog->Log("[CLuaScript] Call to %s callback successful.\n", instance->GetCallbackName().c_str());
#endif
			}
		}
	}
}
void CLuaScript::Call(std::string callback, int args, ...)
{
	m_CallBackName	= callback;
	m_Args			= args;

	va_list arguments;
	va_start(arguments, args);
	
	for (int x = 0; x < m_Args; x++)
		m_ArgList.push_back(va_arg(arguments, char*));

	va_end(arguments);

	CreateLuaThread();
}
void CLuaScript::InitializeLua()
{

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
