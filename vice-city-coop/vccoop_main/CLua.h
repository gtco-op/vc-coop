#pragma once
class CLua
{
public:
	char*	mainScript;
	double		mainScriptSize;

	lua_State * GetState()			{ return lState; }
	bool IsLuaActive()				{ return bActive; }
	void SetLuaStatus(bool Status)	{ bActive = Status; }

	void Call(std::string callback, char *fmt = { 0 }, ...);
	
	static int lua_DrawText(lua_State* L);
	static int lua_FindLocalPed(lua_State* L);
	static int lua_Log(lua_State* L);
	static int lua_ann(lua_State* L);
	
	void CreateLuaThread();
	static void	LuaThread(LPVOID lParam);
public:
	CLua();
	~CLua();

protected:
	lua_State *						lState;
	bool							bActive;
	CCustomData*					m_Data;
	int								m_Args;
	std::vector<std::string>		m_ArgList;
	std::string						m_CallBackName;
	int								m_Type;
};

