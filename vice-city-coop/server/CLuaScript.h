#pragma once
class CLuaScript
{
public:
	CLuaScript(CCustomData* ptr);
	~CLuaScript() { lua_close(m_lState); };

	void InitializeLua();
	void CallCallback(std::string callback, int args = 0, ...);

	virtual lua_State* GetState() { return m_lState; }
	virtual CCustomData* GetData() { return m_Data; }
protected:
	CCustomData* m_Data;
	lua_State* m_lState;
};

