#include "server.h"

std::string CLua::scriptOutput;

CLua::CLua(std::string scriptName, std::string scriptBuffer, int size)
{
	if (scriptBuffer.empty() || scriptName.empty() || size <=5)
		return;

	this->lState = luaL_newstate();	
	if (this->lState == nullptr)
		return;
	
	luaL_openlibs(this->lState);
	lua_getglobal(this->lState, "_G");

	gLog->Log("[CLua][%s] Dumping script with size %d\n", scriptName.c_str(), size);
	luaL_loadstring(this->lState, scriptBuffer.c_str() + 3);

	if (lua_dump(this->lState, this->luaWriter, NULL, 0) == 0)	{
		gLog->Log("[CLua][%s] Dumped script size: %d\n", scriptName.c_str(), scriptOutput.size());

		this->scriptOutput		= scriptOutput;
		this->scriptOutputSize	= scriptOutput.size();
		this->luaFinished = true;
	}
	else
	{
		gLog->Log("[CLua][%s] An error occurred while dumping the script.\n", scriptName.c_str());
	}
}
CLua::~CLua()
{
	gLog->Log("[CLua] CLua shutting down.\n");
}
int CLua::luaWriter(lua_State* L, const void* p, size_t size, void* u)
{
	return (scriptOutput.append((char*)p, size)).empty();
}