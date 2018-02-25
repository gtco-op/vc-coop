#pragma once
class CLuaDumper
{
public:
	CLuaDumper(std::string scriptName);
	~CLuaDumper() {};
	
	std::string GetScriptData() { return compiledScriptOutput; }
	lua_State*	GetLuaState() { return lState; }
	bool		GetLuaStatus() { return luaFinished; }

	static std::string compiledScriptOutput;
	std::pair<char*, double> GetScript() { return std::pair<char*, double>((char*)this->GetScriptData().c_str(), this->scriptOutputSize); }

private:
	static int luaWriter(lua_State* L, const void* p, size_t size, void* u);

protected:
	double scriptOutputSize;
	lua_State * lState;
	bool luaFinished = false;
};

