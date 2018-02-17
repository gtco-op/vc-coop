#include "main.h"

bool								CClientNetwork::client_connected;
bool								CClientNetwork::client_running;
bool								CClientNetwork::connected;
bool								CClientNetwork::initialized;
std::vector<std::pair<CPed*, int>>	CClientNetwork::players;
CClientPlayer*						networkPlayers[MAX_PLAYERS];

RakPeerInterface					*g_RakPeer;
RPC4								*g_RPC;

#define Log(fmt, ...) gLog->Log("[CClientNetwork] " fmt "\n", __VA_ARGS__)

CClientNetwork::CClientNetwork()
{
	client_running		= false;
	client_connected	= false;
	connected			= false;
	initialized			= false;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		networkPlayers[i] = NULL;
	}
	Log("CClientNetwork initialized");
}
CClientNetwork::~CClientNetwork()
{
	Log("CClientNetwork shutting down");
}
void CClientNetwork::InitializeClient()
{
	g_RakPeer = RakPeerInterface::GetInstance();
	g_RPC = RPC4::GetInstance();
	g_RakPeer->AttachPlugin(g_RPC);
	g_RakPeer->SetSplitMessageProgressInterval(100);
	g_RakPeer->Startup(1, &SocketDescriptor(), 1, THREAD_PRIORITY_NORMAL);

	initialized = true;
	
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CClientNetwork::NetworkThread, this, 0, NULL);
	Log("RakNet initialized");
}
void CClientNetwork::Connect(const char* Host, unsigned short Port, const char* Password)
{
	if (!initialized)
		return;

	gRender->bConnecting	= true;
	gRender->bGUI			= true;
	gRender->bAboutWindow	= false;
	gRender->bEscMenu		= false;

	Log("Attempting to connect to %s:%d", Host, Port);
	
	g_RakPeer->Connect(Host, Port, 0, 0);
}
void CClientNetwork::Disconnect()
{
	if (!initialized)
		return;
	if (!client_connected || !client_running)
		return;

	g_RakPeer->CloseConnection(g_RPC->GetMyGUIDUnified(), true);
}
void CClientNetwork::UpdateNetwork()
{
	Packet *g_Packet = NULL;

	while (g_Packet = g_RakPeer->Receive())
	{
		BitStream g_BitStream(g_Packet->data + 1, g_Packet->length + 1, false);

		switch (g_Packet->data[0])
		{
		case ID_UNCONNECTED_PONG:
		{
			break;
		}
		case ID_ADVERTISE_SYSTEM:
		{
			break;
		}
		case ID_DOWNLOAD_PROGRESS:
		{
			break;
		}
		case ID_IP_RECENTLY_CONNECTED:
		{
			Log("Failed to connect, recently connected");
			this->SetCanSpawn(FALSE);
			break;
		}
		case ID_INCOMPATIBLE_PROTOCOL_VERSION:
		{
			Log("Failed to connect, incompatible protocol version");
			this->SetCanSpawn(FALSE);
			break;
		}
		case ID_ALREADY_CONNECTED:
		{
			Log("Failed to connect, already connected");
			this->SetCanSpawn(FALSE);
			break;
		}
		case ID_NO_FREE_INCOMING_CONNECTIONS:
		{
			Log("Failed to connect, max client");
			this->SetCanSpawn(FALSE);
			break;
		}
		case ID_INVALID_PASSWORD:
		{
			Log("Failed to connect, invalid password");
			this->SetCanSpawn(FALSE);
			break;
		}
		case ID_CONNECTION_ATTEMPT_FAILED:
		{
			Log("Failed to connect, server not responding");
			this->SetCanSpawn(FALSE);
			break;
		}
		case ID_CONNECTION_BANNED:
		{
			Log("Failed to connect, banned");
			this->SetCanSpawn(FALSE);
			break;
		}
		case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			Log("Accepted connection request");
			this->SetCanSpawn(TRUE);
			break;
		}
		case ID_DISCONNECTION_NOTIFICATION:
		{
			Log("Client disconnected");
			this->SetCanSpawn(FALSE);
			break;
		}
		case ID_CONNECTION_LOST:
		{
			Log("Connection lost");
			this->SetCanSpawn(FALSE);
			break;
		}
		}
		g_RakPeer->DeallocatePacket(g_Packet);
	}
}
// Sets internal variables appropriately after a server connection state has been returned.
void CClientNetwork::SetCanSpawn(bool bStatus)
{
	if (bStatus)
	{
		connected = true;
		client_connected = true;
		client_running = true;

		gRender->bConnecting = false;
		gRender->bGUI = false;
		gRender->bAboutWindow = false;

		gGame->RestoreCamera();
		gGame->SetCameraBehindPlayer();
		gGame->EnableHUD();

		Log("Allowed player to spawn");
	}
	else
	{
		connected = false;
		client_connected = false;
		client_running = false;

		gRender->bConnecting = false;
		gRender->bGUI = true;

		gGame->DisableHUD();

		Log("Disallowed player from spawning");
	}
}
void CClientNetwork::NetworkThread(LPVOID param)
{
	CClientNetwork* network = (CClientNetwork*)param;
	
	if (!network)
		return;

	Log("Entering client network loop");

	while (network->initialized)
	{
		network->UpdateNetwork();
		Sleep(100);
	}
}