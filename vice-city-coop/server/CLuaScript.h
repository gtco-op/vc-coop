#pragma once
class CLuaScript
{
public:
	CLuaScript(CCustomData* ptr);
	~CLuaScript() { lua_close(m_lState); };

	void InitializeLua();
	void CallCallback(std::string callback, int args = 0, ...);

	static int lua_Log(lua_State* L);
	static int lua_Sleep(lua_State* l);

	virtual int GetArguments() { return m_Args; }
	virtual lua_State* GetState() { return m_lState; }
	virtual CCustomData* GetData() { return m_Data; }
protected:
	CCustomData* m_Data;
	lua_State* m_lState;
	int m_Args;
};

