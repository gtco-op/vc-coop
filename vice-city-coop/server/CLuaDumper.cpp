#include "server.h"

std::string CLuaDumper::compiledScriptOutput;

CLuaDumper::CLuaDumper(std::string scriptName)
{
	this->lState = luaL_newstate();	
	if (this->lState == nullptr)
		return;
	
	luaL_openlibs(this->lState);
	lua_getglobal(this->lState, "_G");
	luaL_loadfile(this->lState, scriptName.c_str());

	if (lua_dump(this->lState, CLuaDumper::luaWriter, NULL, 0) == 0)	
	{
		this->scriptOutputSize	= compiledScriptOutput.size();
		this->luaFinished		= true;
	}
	else
	{
		gLog->Log("[CLuaDumper][%s] An error occurred while dumping the script.\n", scriptName.c_str());
	}
}
int CLuaDumper::luaWriter(lua_State* L, const void* p, size_t size, void* u)
{
	return (compiledScriptOutput.append((char*)p, size)).empty();
}	