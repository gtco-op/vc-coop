#define LIBRG_DEBUG
#define LIBRG_IMPLEMENTATION
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define VCCOOP_SRVVER "0.1.0.a"

#include "librg\librg.h"
#include <Windows.h>
#include <iostream>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

HANDLE server_handle = NULL;
bool server_running = false, console_active = false;

void on_connect_request(librg_event_t *event) {
	cout << "[CLIENT REQUEST] Some one is requesting to connect\n";

}
void on_connect_accepted(librg_event_t *event) {
	cout << "[CLIENT CONNECTION] Player Connected\n";

}
void on_creating_entity(librg_event_t *event) {

}

void server_thread()
{
	librg_ctx_t ctx = { 0 };
	ctx.mode = LIBRG_MODE_SERVER;
	ctx.tick_delay = 32;
	librg_init(&ctx);
	librg_event_add(&ctx, LIBRG_CONNECTION_REQUEST, on_connect_request);
	librg_event_add(&ctx, LIBRG_CONNECTION_ACCEPT, on_connect_accepted);
	printf("Server thread initialized\n");

	librg_address_t addr = { 23546 };
	librg_network_start(&ctx, addr);
	printf("Server starting on port %d\n", addr.port);
	
	while (server_running) {
		librg_tick(&ctx);
	}

	librg_network_stop(&ctx);
	librg_free(&ctx);

	server_running = false;
}

int main(int argc, char const *argv[]) {
	SetConsoleTitle(TEXT("Vice City CO-OP " VCCOOP_SRVVER " Server"));
	printf("Vice City CO-OP " VCCOOP_SRVVER " Server\nIf you want any help see our site www.vicecityco-op.ml\nCreating server thread..\n");

	std::string input;
	console_active = true;
	server_running = true;
	server_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&server_thread, NULL, 0, NULL);

	while (console_active)
	{
		getline(cin, input);

		if (strstr(input.c_str(), "exit") || strstr(input.c_str(), "quit"))
		{
			console_active = false;
		}
		Sleep(10);
	}

	return 0;
}