#pragma once
class CServerNetwork
{
public:
	CServerNetwork();
	~CServerNetwork();

	/* Server Configuration */
	static int ServerPort;
	static int ServerSecret;

	void InitializeServer();
	static void ServerThread(LPVOID param);
	void CreateServerThread();
	//------------------------------------------------------------------------------------------------

	RakNet::RakPeerInterface *peerInterface;
	RakNet::RPC4 *RPC;

	static void OnClientSendMessage(BitStream *userData, Packet *packet);

	static HANDLE server_handle;
	static bool server_running;

	static std::pair<char*, int> LoadScript(std::string filename);

	static std::list<CServerPlayer*> NetworkPlayers;
};

