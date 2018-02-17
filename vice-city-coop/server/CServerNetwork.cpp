#include "server.h"

/* Server Configuration */
int								CServerNetwork::ServerPort;
int								CServerNetwork::ServerSecret;
HANDLE							CServerNetwork::server_handle;
bool							CServerNetwork::server_running, CServerNetwork::console_active;

std::vector<std::pair<char*, int>> dataArray;

CServerNetwork::CServerNetwork()
{
	server_running = true;
}
CServerNetwork::~CServerNetwork()
{

}
std::pair<char*,int> CServerNetwork::LoadScript(std::string filename)
{
	// load the data into memory
	std::string path = GetExecutablePath();
	path.append("\\scripts\\");
	path.append(filename);

	std::ifstream t(path);
	std::stringstream buffer;
	buffer << t.rdbuf();

	// allocate data
	std::string dataData = buffer.str();
	int dataLen = buffer.str().size() + 1;

#ifdef VCCOOP_LIBRG_DEBUG
	gLog->Log("[CServerNetwork] Loaded data with size %d\n", dataLen);
	gLog->Log("[CServerNetwork] Total data Size: %d\n", sizeof(dataLen) + (buffer.str().size() + 1));
#endif

	// construct the dataset
	char* databuf = new char[buffer.str().size() + 1];
	memset(databuf, 0, (buffer.str().size() + 1));
	sprintf(databuf, "%s", dataData.c_str());

	// compile to bytecode
	CLua* gLua = new CLua(filename, databuf, buffer.str().size() +1);
	while(!gLua->GetLuaStatus()) { }
	delete[] databuf;

	databuf = new char[gLua->scriptOutput.size() + sizeof(dataLen)];
	dataLen = gLua->scriptOutput.size();
	sprintf(databuf, "%d", dataLen);
	memcpy(databuf+4, gLua->scriptOutput.c_str(), gLua->scriptOutput.size());

	return std::pair<char*, int>(databuf, dataLen);
}