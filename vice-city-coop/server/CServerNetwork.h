#pragma once
class CServerNetwork
{
public:
	CServerNetwork();
	~CServerNetwork();

	/* Server Configuration */
	static int ServerPort;
	static int ServerSecret;

	//------------------------------------------------------------------------------------------------

	static HANDLE server_handle;
	static bool server_running, console_active;

	static std::pair<char*, int> LoadScript(std::string filename);
};

