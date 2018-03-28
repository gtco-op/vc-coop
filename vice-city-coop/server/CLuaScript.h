#pragma once

class CLuaScript
{
public:
	CLuaScript(CCustomData* ptr);
	~CLuaScript() { lua_close(m_lState); };

	void Call(std::string callback, char *fmt = { 0 }, ...);
#if defined(_MSC_VER)
	static void LuaThread(LPVOID lParam); 
#else
	static void *LuaThread(LPVOID lParam);
#endif
	void CreateLuaThread();

	static int lua_Log(lua_State* L);
	static int lua_Sleep(lua_State* l);
	
	static int lua_SendGlobalMessage(lua_State* L);

	static int lua_IsEntityValid(lua_State* L);

	static int lua_GetEntityType(lua_State* L);

	static int lua_GetEntityPos(lua_State* L);
	static int lua_SetEntityPos(lua_State* L);

	static int lua_GetEntityControlPeer(lua_State* L);
	static int lua_SetEntityControlPeer(lua_State* L);
	static int lua_RemoveEntityControl(lua_State* L);

	static int lua_GetEntityOrientation(lua_State* L);
	static int lua_SetEntityOrientation(lua_State* L);

	static int lua_GivePlayerWeapon(lua_State* L);

	static int lua_GetPlayerModel(lua_State* L);
	static int lua_SetPlayerModel(lua_State* L);

	static int lua_GetPlayerHealth(lua_State* L);
	static int lua_SetPlayerHealth(lua_State* L);

	static int lua_GetPlayerArmour(lua_State* L);
	static int lua_SetPlayerArmour(lua_State* L);

	static int lua_GetVehicleHealth(lua_State* L);
	static int lua_SetVehicleHealth(lua_State* L);
	
	static int lua_GetRandomModel(lua_State* L);
	static int lua_GetRandomVehicleColor(lua_State* L);

	static int lua_GetPlayerName(lua_State* L);
	
	static int lua_StartMissionScript(lua_State* L);

	static int lua_AddObject(lua_State* L);
	static int lua_AddVehicle(lua_State* L);
	static int lua_AddPed(lua_State* L);

	virtual std::string GetCallbackName()			{ return m_CallBackName; }
	virtual int GetArguments()						{ return m_Args; }
	virtual lua_State* GetState()					{ return m_lState; }
	virtual CCustomData* GetData()					{ return m_Data; }
	virtual bool GetServerStartStatus()				{ return m_bHasServerStartRun; }
	virtual void SetServerStartStatus(bool status)  { m_bHasServerStartRun = status; }

protected:
	CCustomData*					m_Data;
	lua_State*						m_lState;
	int								m_Args;
	std::vector<std::string>		m_ArgList;
	std::string						m_CallBackName;
	bool							m_bHasServerStartRun;
};