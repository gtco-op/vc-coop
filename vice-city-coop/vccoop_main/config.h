#pragma once

// Define this to have a console window spawn on boot
#define VCCOOP_DEBUG

// Define this for verbose log output
//#define VCCOOP_VERBOSE_LOG

// Temporary spawn position
#define VCCOOP_DEFAULT_SPAWN_POSITION	10.0f, 5.0f, 25.0f

// Default nickname
#define VCCOOP_DEFAULT_NICKNAME			"Nickname123"

// Default server address
#define VCCOOP_DEFAULT_SERVER_ADDRESS	"89.132.130.116"

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


struct PedSyncData {
	int m_pFleeFrom;

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

	CVehicle* m_pObjectiveVehicle;

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
	CPathNode m_apPathNodesStates[8];

	unsigned int m_dwPathNodeType;

	char m_nPathState;
	CPathNode* m_pLastPathNode;
	CPathNode* m_pNextPathNode;
	int m_pPathRelEntity;

	int m_dwMoveState;
	float m_fSeekExAngle;
	int m_pSeekTarget;
	CVector m_vecSeekPosEx;
	CVector m_vecOffsetSeek;
	unsigned int m_dwEventType;

	float m_fAngleToEvent;
	float m_fEventOrThreatX;
	float m_fEventOrThreatY;
	int m_pEventEntity;
};

struct SPlayerData {
	PedSyncData pedData;

	float Health;
	float Armour;
	float Rotation;

	int iModelIndex;
	int iInteriorID;
	int iCurrentAnimID;

	unsigned int CurrWep;
	unsigned int WepModelIndex;
	unsigned int Ammo;
};

enum {
	VCOOP_CREATE_PED = LIBRG_EVENT_LAST,
	VCOOP_CREATE_VEHICLE,
	VCOOP_DO_SOMETHING_ELSE,
};

enum {
	VCOOP_PLAYER,
	VCOOP_PED,
	VCOOP_VEHICLE,
};