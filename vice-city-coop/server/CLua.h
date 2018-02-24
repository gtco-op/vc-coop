#pragma once
class CLua
{
public:
	CLua(std::string scriptName, std::string scriptBuffer, int size);
	~CLua();
	
	std::string GetScriptData() { return compiledScriptOutput; }
	lua_State*	GetLuaState() { return lState; }
	bool		GetLuaStatus() { return luaFinished; }

	static std::string compiledScriptOutput;
	std::pair<char*, int> GetScript() { return std::pair<char*, int>((char*)this->GetScriptData().c_str(), this->scriptOutputSize); }

private:
	static int luaWriter(lua_State* L, const void* p, size_t size, void* u);

protected:
	size_t scriptOutputSize;
	lua_State * lState;
	bool luaFinished = false;
};

