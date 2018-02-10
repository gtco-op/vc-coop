#pragma once

// Define this for verbose log output
//#define VCCOOP_VERBOSE_LOG

// Temporary spawn position
#define VCCOOP_DEFAULT_SPAWN_POSITION	10.0f, 5.0f, 25.0f

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

struct SPlayerData {
	char szName[25];
	
	float Health;
	float Armour;
	float Rotation;

	int iModelIndex;
	int iInteriorID;
	int iCurrentAnimID;

	unsigned int CurrWep;
	unsigned int WepModelIndex;
	unsigned int Ammo;

	GTA_CONTROLSET playerKeys;
	CAMERA_AIM playerLook;

	PedSyncData pedData;
};

enum {
	VCOOP_CREATE_PED = LIBRG_EVENT_LAST,
	VCOOP_CREATE_VEHICLE,

	VCOOP_SEND_MESSAGE,
	VCOOP_RECEIVE_MESSAGE,

	VCOOP_DO_SOMETHING_ELSE,
};

enum {
	VCOOP_PLAYER,
	VCOOP_PED,
	VCOOP_VEHICLE,
};