#include "server.h"

const char					*CWebManagementServer::ServerPort = VCCOOP_DEFAULT_WEBSERVER_PORT;
struct mg_serve_http_opts	CWebManagementServer::ServerOptions;

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
		mg_mgr_poll(&gWebServer->GetManager(), 1000);
	}
}
CWebManagementServer::CWebManagementServer()
{
	mg_mgr_init(&mgr, NULL);
	gLog->Log("[CWebManagementServer] Starting web server on port %s\n", ServerPort);
	nc = mg_bind(&mgr, ServerPort, EventHandler);

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
