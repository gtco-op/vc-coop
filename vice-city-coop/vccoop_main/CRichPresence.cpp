#include "main.h"

static char* APP_ID = "411105613288833034";
HANDLE g_ThreadHandle = nullptr;

CRichPresence::CRichPresence() 
{
	StartThread();

	gLog->Log("[CRichPresence] CRichPresence initialized\n");
}
CRichPresence::~CRichPresence() {
	gLog->Log("[CRichPresence] CRichPresence shutting down\n");

	Shutdown();
}
void CRichPresence::StartThread()
{
	this->threadActive = true;
	g_ThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&rpThread, this, 0, NULL);
}
void CRichPresence::StopThread()
{
	TerminateThread(g_ThreadHandle, 0);
}
void CRichPresence::rpThread(LPVOID lParam)
{
	CRichPresence* tmp = (CRichPresence*)lParam;
	if (tmp == nullptr)
		return;
	
	Discord_Initialize(APP_ID, NULL, 1, NULL);

	while (tmp->threadActive) {
		tmp->UpdateRichPresence(gNetwork->connected);
		Sleep(1);
	}

	tmp->Shutdown();
}
void CRichPresence::Shutdown() {
	StopThread();
	
	Discord_Shutdown();
}
void CRichPresence::UpdateRichPresence(bool IsConnected)
{	
	DiscordRichPresence dp;
	memset(&dp, 0, sizeof(dp));
	dp.largeImageText = "Grand Theft CO-OP: Vice City";
	dp.largeImageKey = "main";
	dp.startTimestamp = time(0);

	char* details = new char[256];
	char* state = new char[256];
	if (IsConnected) {
		sprintf(details, "In %s", gNetwork->ServerAddress);
		dp.details = details;
		
		sprintf(state, "Nickname: %s", (char *)gGame->Name.c_str());
		dp.state = state;
		dp.smallImageKey = "con"; //green circle
		dp.smallImageText = "Connected";
	}
	else {
		sprintf(details, "Not connected");
		dp.details = details;

		sprintf(state, "NickName: %s", (char *)gGame->Name.c_str());
		dp.smallImageKey = "discon"; //red circle
		dp.smallImageText = "Disconnected";
	}
	Discord_UpdatePresence(&dp);
}