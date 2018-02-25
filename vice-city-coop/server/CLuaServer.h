#pragma once
class CLuaServer
{
public:
	CLuaServer() {};
	~CLuaServer() {};

	void StartServer();
	void StartServer(CLuaScript* luaScript);
	void StopServer();

	CLuaScript* GetLuaScript() { return m_LuaScript; }
	bool GetServerStatus() { return m_Active; }
protected:
	bool m_Active;
	CLuaScript* m_LuaScript;
};

