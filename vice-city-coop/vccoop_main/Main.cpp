/*
	Vice City CO-OP Main ASI
	Author(s) Zeyad Ahmed
	Copyrights (c) 2017-2018 VC:CO-OP Team
*/
#define LIBRG_DEBUG
#define LIBRG_IMPLEMENTATION

#include "librg/librg.h"
#include "plugin_vc.h"
#include "includes.h"

librg_entity_id player;
using namespace plugin;
class vccoop {
public:
		void on_connect_request(librg_event_t *event) {
			librg_log("[CLIENT] Requesting to connect to server...");
		}		
		void on_connect_accept(librg_event_t *event) {
			librg_log("[CLIENT] Connection Accepted.");
			player = event->entity->id;
			librg_log("[CLIENT] Spawned with id: %u\n", player);
		}
		void on_connect_refused(librg_event_t *event) {
			librg_log("[CLIENT] Connection Refused.");
		}
		void on_entity_create(librg_event_t *event) {

		}
    vccoop() { //when ASI Attaches
		Events::gameProcessEvent += [] { 
		};
    }
} myPlugin;