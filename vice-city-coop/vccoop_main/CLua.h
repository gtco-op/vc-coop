#pragma once
class CLua
{
public:
	char* mainScript;

	lua_State * GetState()			{ return lState; }
	bool IsLuaActive()				{ return bActive; }
	void SetLuaStatus(bool Status)	{ bActive = Status; }
	
	static int lua_FindPlayerPed(lua_State* L);
	static int lua_Log(lua_State* L);

	void CreateLuaThread();
	static void	LuaThread(LPVOID lParam);
public:
	CLua();
	~CLua();

protected:
	lua_State * lState;
	bool		bActive;
};

