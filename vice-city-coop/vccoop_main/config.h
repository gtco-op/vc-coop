#pragma once

// Define this to have a console window spawn on boot
#define VCCOOP_DEBUG

// Define this for verbose log output
//#define VCCOOP_VERBOSE_LOG

// Temporary spawn position
#define VCCOOP_DEFAULT_SPAWN_POSITION	10.0f, 5.0f, 25.0f

// Default server address
#define VCCOOP_DEFAULT_SERVER_ADDRESS	"192.168.0.16"

// Default server port
#define VCCOOP_DEFAULT_SERVER_PORT		23546

//------------------------------------------------------------------------------------------

#ifndef VCCOOP_SERVER
#define VCCOOP_NAME						"VC CO-OP"
#else
#define VCCOOP_NAME						"VC CO-OP Server"
#endif

#ifndef VCCOOP_SERVER
#define VCCOOP_VER						"0.02"
#else
#define VCCOOP_VER						"0.2.0.a"
#endif

#define DEFAULT_TIME_FORMAT				"%Y%m%d_%H%M%S"
#define LOGGER_TIME_FORMAT				"%H:%M:%S"
#define VC_TITLE						"GTA: Vice City"
#define SERVER_SECRET					420420420

struct SPlayerData {
	float Health;
	float Armour;
	int iModelIndex;
	float Rotation;
	int iInteriorID;
	int iCurrentAnimID;
	unsigned int CurrWep;
	unsigned int WepModelIndex;
	unsigned int Ammo;
};