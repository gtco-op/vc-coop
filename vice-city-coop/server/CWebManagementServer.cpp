#include "server.h"

const char					*CWebManagementServer::ServerPort = VCCOOP_DEFAULT_WEBSERVER_PORT;
struct mg_serve_http_opts	CWebManagementServer::ServerOptions;

std::string localIP;

std::string GetLocalIPAddress()
{
	std::string res;
	WSADATA WSAData;

	if (::WSAStartup(MAKEWORD(1, 0), &WSAData))
	{
		return res;
	}

	char szHostName[128] = "";
	if (::gethostname(szHostName, sizeof(szHostName)))	{
		return res;
	}

	struct sockaddr_in SocketAddress;
	struct hostent *pHost = 0;

	pHost = ::gethostbyname(szHostName);
	if (!pHost)	{
		return res;
	}

	char aszIPAddresses[10][16];
	for (int nCount = 0; ((pHost->h_addr_list[nCount]) && (nCount < 10)); ++nCount)
	{
		memcpy(&SocketAddress.sin_addr, pHost->h_addr_list[nCount], pHost->h_length);
		strcpy(aszIPAddresses[nCount], inet_ntoa(SocketAddress.sin_addr));
		
		res = aszIPAddresses[nCount];
	}

	WSACleanup();
	return res;
}

void CWebManagementServer::EventHandler(struct mg_connection *nc, int ev, void *p) 
{
	if (ev == MG_EV_HTTP_REQUEST) 
	{
		mg_serve_http(nc, (struct http_message *) p, ServerOptions);
	}
}
void CWebManagementServer::ServerThread()
{
	while (gWebServer->GetStatus()) 
	{
		// Perform actions before waiting for input..		
		int playerCount = 0;
		for (auto it : playerEntities) {
			if (it && it->client_peer != nullptr)
				playerCount++;
		}
		gActivePlayers = playerCount;

		if (gActivePlayers != NULL || gActivePlayers >= 0) {
			FILE* fptr = fopen("Management\\serverdata.html", "wb");
			fprintf(fptr, "<html><head><title>Vice City Co-Op Server Data</title></head><body><a href=\"vccoop:host=%s;port=%d\">Connect</a><br />Active Players: %d</body></html>", localIP.c_str(), gServerNetwork->ServerPort, gActivePlayers);
			fclose(fptr);
		}
		mg_mgr_poll(&gWebServer->GetManager(), 1000);
	}
}
CWebManagementServer::CWebManagementServer()
{
	mg_mgr_init(&mgr, NULL);
	gLog->Log("[CWebManagementServer] Starting web server on port %s\n", ServerPort);
	nc = mg_bind(&mgr, ServerPort, EventHandler);

	localIP = GetLocalIPAddress();
	gLog->Log("[CWebManagementServer] Found local IP address: %s\n", localIP.c_str());
	
	if (nc == NULL) 
	{
		gLog->Log("[CWebManagementServer] Failed to create listener\n");
		return;
	}

	mg_set_protocol_http_websocket(nc);

	ServerOptions.document_root				= VCCOOP_DEFAULT_WEBSERVER_DIR;  
	ServerOptions.enable_directory_listing	= (bDirectoryListing?"yes":"no");

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ServerThread, NULL, 0, NULL);
}
CWebManagementServer::~CWebManagementServer()
{
	bActive = false;
	gLog->Log("[CWebManagementServer] Stopping web server\n");
	mg_mgr_free(&mgr);
}
