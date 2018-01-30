/*
	Vice City CO-OP Server

	Author(s)	Lemonhaze
				Zeyad Ahmed

	Copyright (c) 2017-2018 VC:CO-OP Team
*/
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
#include <vector>

using namespace std;

struct PlayerInfo {
	// PLAYER INFO
	float Health;
	float Armour;
	int iModelIndex;
	float Rotation;
	int iInteriorID;
	int iCurrentAnimID;
	unsigned int WeaponType;
	unsigned int Ammo;
	// PLAYER VEHICLE INFO
};
HANDLE server_handle = NULL;
bool server_running = false, console_active = false;
std::vector<librg_entity_t*> entities;
librg_ctx_t ctx = { 0 };

void on_connect_request(librg_event_t *event) {
	cout << "[CLIENT REQUEST] Some one is requesting to connect\n";
}
void on_connect_accepted(librg_event_t *event) {
	printf("[CLIENT CONNECTION] Player %d Connected\n", event->entity->id);
	PlayerInfo *plrinfo = (PlayerInfo *)event->entity->user_data;
	librg_entity_t *entity = event->entity;
	entities.push_back(entity);
	librg_entity_control_set(event->ctx, entity->id, entity->client_peer);
}
void on_creating_entity(librg_event_t *event) {
	printf("Entity creating\n");
	

}
void on_entity_update(librg_event_t *event) {
	printf("Entity updating\n");

}
void on_disconnect(librg_event_t* event)
{
	auto it = std::find(entities.begin(), entities.end(), event->entity);
	if (it != entities.end()) {
		entities.erase(it);
		printf("Deleting item from entities vector..\n");
	}
}
void server_thread()
{
	ctx.world_size = zplm_vec3(5000.0f, 5000.0f, 5000.0f);
	ctx.mode = LIBRG_MODE_SERVER;
	ctx.tick_delay = 32; // 32ms delay, is around 30hz, quite fast
	librg_init(&ctx);
	librg_event_add(&ctx, LIBRG_CONNECTION_REQUEST, on_connect_request);
	librg_event_add(&ctx, LIBRG_CONNECTION_ACCEPT, on_connect_accepted);
	librg_event_add(&ctx, LIBRG_ENTITY_UPDATE, on_entity_update);
	printf("Server thread initialized\n");

	librg_address_t addr = { 23546 };
	librg_network_start(&ctx, addr);
	printf("Server starting on port %d\n", addr.port);
	
	while (server_running) {
		for (auto it : entities)
		{
			printf("Entity ID# %d : Pos X: %.f Pos Y: %.f Pos Z: %.f \n", it->id, it->position.x, it->position.y, it->position.z);
		}
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