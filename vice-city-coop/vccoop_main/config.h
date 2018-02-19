#pragma once

// Define this for verbose log output
//#define VCCOOP_VERBOSE_LOG

// Temporary spawn position
#define VCCOOP_DEFAULT_SPAWN_POSITION	531.629761f, 606.497253f, 10.901563f

// Default nickname
#define VCCOOP_DEFAULT_NICKNAME			"Nickname123"

// Default server address
#define VCCOOP_DEFAULT_SERVER_ADDRESS	"127.0.0.1"

// Default server port
#define VCCOOP_DEFAULT_SERVER_PORT		23546

// Default server secret
#define VCCOOP_DEFAULT_SERVER_SECRET	420420420

// Default server config filename
#define VCCOOP_DEFAULT_SERVER_CONFIG	"server.ini"

// Default client config filename
#define VCCOOP_DEFAULT_CLIENT_CONFIG	"client.ini"

// Default server scripts folder
#define VCCOOP_SERVER_SCRIPTS_DIR		"scripts"

// VCCOOP Window Title
#define VCCOOP_WINDOW_TITLE				"Vice City CO-OP"

//print game's internal debug messages
#define VCCOOP_DEBUG_ENGINE

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

#define MAX_PLAYERS						500
#define MAX_CONNECTIONS					2000

#pragma pack(1)
typedef struct _CAMERA_AIM
{
	float f1x, f1y, f1z;
	float pos1x, pos1y, pos1z;
	float pos2x, pos2y, pos2z;
	float f2x, f2y, f2z;

} CAMERA_AIM;

#pragma pack(1)
typedef struct _GTA_CONTROLSET
{
	DWORD dwFrontPad;
	WORD wKeys1[19];
	DWORD dwFrontPad2;
	WORD wKeys2[19];
	WORD wTurnLeftRightAnalog[10];
	BYTE bytePadding1[138];
	BYTE byteCrouchAnalog[5];
} GTA_CONTROLSET;


struct PedSyncData 
{
	float Health;
	float Armour;
	float Rotation;

	int iModelIndex;
	int iInteriorID;
	int iCurrentAnimID;

	unsigned int CurrWep;
	unsigned int WepModelIndex;
	unsigned int Ammo;

	int gameTimer;

	float m_fActionX;
	float m_fActionY;
	
	float m_fRotationDest;
	float m_fLookDirection;
	CVector2D m_vecAnimMoveDelta;
	float m_dwAnimGroupId;
	CVector m_vecMoveSpeed;

	int m_dwAction;
	int m_dwActionTimer;
	float m_dwActionX;
	float m_dwActionY;

	int m_dwObjective;
	int m_dwObjectiveTimer;

	CVector m_vecObjective;

	float m_fObjectiveAngle;
	int m_pObjectiveEntity;
	
	int m_dwFleeTimer;
	float m_fFleeFromPosX;
	float m_fFleeFromPosY;

	float m_fPathNextNodeDir;
	short wRouteCurDir;

	CVector m_vecPathNextNode;
	int m_dwPathNodeTimer;

	short m_wCurPathNode;
	unsigned short m_wPathNodes;
	CPathNode m_aPathNodeStates[8];
	CPathNode* m_apPathNodesStates[8];

	unsigned int m_dwPathNodeType;

	char m_nPathState;
	CPathNode* m_pLastPathNode;
	CPathNode* m_pNextPathNode;

	int m_dwMoveState;
	float m_fSeekExAngle;
	CVector m_vecSeekPosEx;
	CVector m_vecOffsetSeek;
	unsigned int m_dwEventType;

	float m_fAngleToEvent;
	float m_fEventOrThreatX;
	float m_fEventOrThreatY;
};

struct PlayerSyncData 
{
	CVector pos;

	float Health;
	float Armour;
	float Rotation;

	short iModelIndex;
	int iInteriorID;
	int iCurrentAnimID;

	unsigned int CurrWep;
	unsigned int WepModelIndex;
	unsigned int Ammo;

	GTA_CONTROLSET playerKeys;
	CAMERA_AIM playerLook;
};

struct deathData
{
	int killer;
	int weapon;
};

enum eBlipType {
	BLIP_NONE,          // 0
	BLIP_CAR,           // 1
	BLIP_CHAR,          // 2
	BLIP_OBJECT,        // 3
	BLIP_COORD,         // 4 - Checkpoint.
	BLIP_CONTACTPOINT,  // 5 - Sphere.
	BLIP_SPOTLIGHT,     // 6
	BLIP_PICKUP,        // 7
	BLIP_AIRSTRIP       // 8
};
enum eBlipDisplay {
	BLIP_DISPLAY_NEITHER,    // 0
	BLIP_DISPLAY_MARKERONLY, // 1
	BLIP_DISPLAY_BLIPONLY,   // 2
	BLIP_DISPLAY_BOTH        // 3
};

enum {
	VCOOP_CREATE_PED = 0,
	VCOOP_CREATE_VEHICLE,

	VCOOP_SEND_MESSAGE,
	VCOOP_RECEIVE_MESSAGE,

	VCOOP_GET_LUA_SCRIPT,

	VCOOP_PED_IS_DEAD,
	VCOOP_RESPAWN_AFTER_DEATH,

	VCOOP_DO_SOMETHING_ELSE,
};

enum {
	VCOOP_PLAYER,
	VCOOP_PED,
	VCOOP_VEHICLE,
};