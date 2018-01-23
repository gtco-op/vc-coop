#define LIBRG_DEBUG
#define LIBRG_IMPLEMENTATION
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "librg\librg.h"
#include "windows.h"
#include <iostream>
using namespace std;
void on_connect_request(librg_event_t *event) {
	cout << "[CLIENT REQUEST] Some one is requesting to connect /n";

}
void on_connect_accepted(librg_event_t *event) {
	cout << "[CLIENT CONNECTION] Player Connected \n";

}
void on_creating_entity(librg_event_t *event) {

}

int main(int argc, char const *argv[]) {
	SetConsoleTitle(TEXT("Vice City CO-OP 0.1.0.a Server"));
	cout << "Vice City CO-OP 0.1.0.a Server \n If you want any help see our site www.vicecityco-op.ml \n";
	cout << "Initalizing The Server. \n";
		librg_ctx_t ctx = { 0 };
		ctx.mode = LIBRG_MODE_SERVER;
		ctx.tick_delay = 32;
		librg_init(&ctx);
	librg_event_add(&ctx, LIBRG_CONNECTION_REQUEST, on_connect_request);
	librg_event_add(&ctx, LIBRG_CONNECTION_ACCEPT, on_connect_accepted);

	

	

	librg_address_t addr = { 23546 };
	librg_network_start(&ctx, addr);
	cout << "Starting Server at port 23546. \n";
	bool running = true;
	while (running) {
		librg_tick(&ctx);
		
	}

	librg_network_stop(&ctx);
	librg_free(&ctx);
	return 0;
}