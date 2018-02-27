#include "main.h"

CRichPresence::~CRichPresence() {
	gLog->Log("Shutting down Rich Presence");
	Discord_Shutdown();
}
CRichPresence::CRichPresence() {
	Discord_RunCallbacks();
}
void CRichPresence::InitRichPresence() {

	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	Discord_Initialize((char *)411105613288833034, &handlers, 1, NULL);

}
void CRichPresence::Shutdown() {
	Discord_Shutdown();
}
//void CRichPresence::UpdateRichPresence(char* status, char* details, char* largeimagekey, char* largeimagetext, char* smallimagekey, char* smallimagetext, int countofplayers, int maxofplayers)

void CRichPresence::UpdateRichPresence(bool IsConnected)
{	
	DiscordRichPresence dp;
	memset(&dp, 0, sizeof(dp));
	dp.largeImageText = "Grand Theft CO-OP: Vice City";
	dp.largeImageKey = "main";
	char* details;
	char* state;
	if (IsConnected) {
		sprintf(details, "In %s", gNetwork->ServerAddress);
		dp.details = details;
		
		sprintf(state, "Nickname: %s", (char *)gGame->Name.c_str());
		dp.state = state;
		dp.smallImageKey = "con"; //green circle
		dp.smallImageText = "Connected";
	}
	else {
		sprintf(details, "Surfing Masterlist");
		dp.details = details;

		sprintf(state, "NickName: %s", (char *)gGame->Name.c_str());
		dp.smallImageText = "discon"; //red circle
		dp.smallImageText = "Disconnected";
	}
	Discord_UpdatePresence(&dp);
}