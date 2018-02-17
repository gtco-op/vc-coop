#include "server.h"
#define Log(fmt, ...) gLog->Log("[CServerNetwork] " fmt "\n", __VA_ARGS__)

/* Server Configuration */
int								CServerNetwork::ServerPort;
int								CServerNetwork::ServerSecret;
HANDLE							CServerNetwork::server_handle;
bool							CServerNetwork::server_running;

std::vector<std::pair<char*, int>> dataArray;


CServerNetwork::CServerNetwork()
{
	server_running = false;
}
CServerNetwork::~CServerNetwork()
{
	RakPeerInterface::DestroyInstance(peerInterface);
}
// Initializes RakNet server instance
void CServerNetwork::InitializeServer()
{
	peerInterface = RakPeerInterface::GetInstance();
	peerInterface->SetMaximumIncomingConnections(MAX_CONNECTIONS);
	peerInterface->Startup(MAX_CONNECTIONS, &SocketDescriptor(ServerPort, 0), 1);

	server_running = true;
	Log("Server initialized");
}
// Creates a server thread for handling messages, packets etc.
void CServerNetwork::CreateServerThread()
{
	server_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CServerNetwork::ServerThread, this, 0, NULL);
	Log("Created server thread");
}
// Server thread for handling messages, packets etc.
void CServerNetwork::ServerThread(LPVOID param)
{
	CServerNetwork* srvNetwork = (CServerNetwork*)param;
	if (!srvNetwork)
		return;
	if (!srvNetwork->server_running)
		return;

	Log("Server thread started on port %d", srvNetwork->ServerPort);
	Packet *packet;
	while (srvNetwork->server_running) {
		for (packet = srvNetwork->peerInterface->Receive(); packet; srvNetwork->peerInterface->DeallocatePacket(packet), packet = srvNetwork->peerInterface->Receive())
		{
			switch (packet->data[0])
			{
			case ID_NEW_INCOMING_CONNECTION:
				Log("A remote system has successfully connected.");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				Log("A remote system has disconnected.");
				break;
			case ID_CONNECTION_LOST:
				Log("A remote system lost the connection.");
				break;
			}
		}
	}
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