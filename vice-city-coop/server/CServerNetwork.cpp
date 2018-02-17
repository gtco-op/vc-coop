#include "server.h"
#define Log(fmt, ...) gLog->Log("[CServerNetwork] " fmt "\n", __VA_ARGS__)

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
// Loads a script into memory, compiles to bytecode and returns a std::pair containing data and size
// TODO: Support for Linux
std::pair<char*,int> CServerNetwork::LoadScript(std::string filename)
{
	// load the data into memory
	std::string path = GetExecutablePath();
	path.append("\\" VCCOOP_SERVER_SCRIPTS_DIR "\\");
	path.append(filename);

	std::ifstream t(path);
	std::stringstream buffer;
	buffer << t.rdbuf();

	// allocate data
	std::string dataData = buffer.str();
	int dataLen = buffer.str().size() + 1;

#ifdef VCCOOP_DEBUG
	Log("[CServerNetwork] Loaded data with size %d", dataLen);
	Log("[CServerNetwork] Total data Size: %d", sizeof(dataLen) + (buffer.str().size() + 1));
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