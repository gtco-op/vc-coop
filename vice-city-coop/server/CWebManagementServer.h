#pragma once
class CWebManagementServer
{
public:
	CWebManagementServer();
	~CWebManagementServer();
	
	static void ServerThread();
	
	bool bActive			= true;
	bool bDirectoryListing	= true;

	bool	GetStatus() { return bActive; }
	mg_mgr	GetManager() { return mgr; }

private:
	static void EventHandler(struct mg_connection *nc, int ev, void *p);

	static const char *ServerPort;
	static struct mg_serve_http_opts ServerOptions;

	struct mg_mgr mgr;
	struct mg_connection *nc;
};

