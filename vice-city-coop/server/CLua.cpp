#include "server.h"

std::string CLua::compiledScriptOutput;

CLua::CLua(std::string scriptName, std::string scriptBuffer, int size)
{
	if (scriptBuffer.empty() || scriptName.empty() || size <=5)
		return;

	this->lState = luaL_newstate();	
	if (this->lState == nullptr)
		return;
	
	luaL_openlibs(this->lState);
	lua_getglobal(this->lState, "_G");
	luaL_loadstring(this->lState, scriptBuffer.c_str());

	if (lua_dump(this->lState, CLua::luaWriter, NULL, 0) == 0)	
	{
		this->scriptOutputSize	= compiledScriptOutput.size();
		this->luaFinished		= true;
	}
	else
	{
		gLog->Log("[CLua][%s] An error occurred while dumping the script.\n", scriptName.c_str());
	}
}
CLua::~CLua()
{
}
int CLua::luaWriter(lua_State* L, const void* p, size_t size, void* u)
{
	return (compiledScriptOutput.append((char*)p, size)).empty();
}	