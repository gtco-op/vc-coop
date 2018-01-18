#include "librg\librg.h"

void on_connect_request(librg_event_t *event) {
	librg_log("[CLIENT REQUEST] Some one is requesting to connect /n");

}
void on_connect_accepted(librg_event_t *event) {
	librg_log("[CLIENT CONNECTION] Player Connected");
	librg_log("spawning player %u at: %f %f %f\n",
		event->entity->id,
		event->entity->position.x,
		event->entity->position.y,
		event->entity->position.z
	);
	
}
int main() {


}