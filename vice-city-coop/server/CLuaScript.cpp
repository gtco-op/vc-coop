#include "server.h"

static const struct luaL_Reg vccooplib[] = {
	{ "print",			&CLuaScript::lua_Log },
	{ "sleep",			&CLuaScript::lua_Sleep },
	{ "GetPlayerName",	&CLuaScript::lua_GetPlayerName },
	{ NULL, NULL }
};

CLuaScript::CLuaScript(CCustomData* ptr) 
{
	if (!ptr)
		return;	

	m_Data = ptr;
	
	Call("onServerStart");
}
void CLuaScript::CreateLuaThread()
{
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&LuaThread, this, 0, NULL);
}
void CLuaScript::LuaThread(LPVOID lParam)
{
	CLuaScript* instance = (CLuaScript*)lParam;

	if (instance == nullptr || (instance->m_lState == nullptr))
		return;

	lua_State* lState = instance->m_lState;

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
		gLog->Log("[CLuaScript] Could not load script buffer when calling callback %s.\n", GetCallbackName().c_str());
		lua_close(m_lState);
		return;
	}
	else
	{
		lua_getglobal(m_lState, GetCallbackName().c_str());
		if (!lua_isfunction(m_lState, -1))
		{
			lua_pop(m_lState, 1);
			gLog->Log("[CLuaScript] Could not find `%s' callback.\n", GetCallbackName().c_str());
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
	Sleep(ms);
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