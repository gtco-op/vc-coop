#pragma once

static char Nickname[25] = { 0 };
static char IP[15]	= VCCOOP_DEFAULT_SERVER_ADDRESS;
static int Port		= VCCOOP_DEFAULT_SERVER_PORT;

class CClientNetwork
{
public:
	CClientNetwork();
	~CClientNetwork();

	void InitializeClient();
	void UpdateNetwork();
	void Connect(const char* Host, unsigned short Port, const char* Password);
	void Disconnect();

	void SetCanSpawn(bool bStatus);

	static void NetworkThread(LPVOID param);

	char	*ServerAddress;
	int		ServerPort;

	static std::vector<std::pair<CPed*, int>> players;
	
	static bool client_running;
	static bool client_connected;
	static bool connected;	
	static bool initialized;
};