#pragma once

// Define this for verbose log output
#define VCCOOP_VERBOSE_LOG

// Define this for verbose lua output (in server)
//#define VCCOOP_LUA_VERBOSE_LOG

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
#ifndef _DEBUG
#undef VCCOOP_DEBUG_ENGINE
#undef VCCOOP_VERBOSE_LOG
#endif 

#define DEFAULT_TIME_FORMAT				"%Y%m%d_%H%M%S"
#define LOGGER_TIME_FORMAT				"%H:%M:%S"
#define VC_TITLE						"GTA: Vice City"

#define MAX_PLAYERS						500
#define MAX_VEHICLES					1000
#define MAX_PEDS						1000

#if !defined(_MSC_VER)
#define MAX_PATH 260
#define LPVOID void*
#define HANDLE pthread_t
#define DWORD unsigned int
#define WORD unsigned short
#define BYTE unsigned char

typedef float RwReal;
typedef struct RwV3d RwV3d;
/**
* \ingroup rwv3d
* \struct RwV3d
*  This type represents 3D points and vectors specified by
* the (x, y, z) coordinates of a 3D point or the (x, y, z) components of a
* 3D vector.
*/
struct RwV3d
{
	RwReal x;   /**< X value */
	RwReal y;   /**< Y value */
	RwReal z;   /**< Z value */
};

//  CVECTOR2D
class CVector2D {
public:
	float x, y;

	inline CVector2D() {
		x = 0.0f; y = 0.0f;
	}

	inline CVector2D(float X, float Y) {
		x = X; y = Y;
	}

	inline CVector2D(CVector2D const& src) {
		x = src.x; y = src.y;
	}

	inline float Magnitude() {
		return sqrtf(this->x * this->x + this->y * this->y);
	}

	inline void Sum(CVector2D &a, CVector2D &b) {
		this->x = a.x + b.x;
		this->y = a.y + b.y;
	}

	inline void Difference(CVector2D &a, CVector2D &b) {
		this->x = a.x - b.x;
		this->y = a.y - b.y;
	}

	inline void operator=(const CVector2D& right) {
		this->x = right.x;
		this->y = right.y;
	}

	inline void operator+=(const CVector2D& right) {
		this->x += right.x;
		this->y += right.y;
	}

	inline void operator-=(const CVector2D& right) {
		this->x -= right.x;
		this->y -= right.y;
	}

	inline void operator *= (float multiplier) {
		this->x *= multiplier;
		this->y *= multiplier;
	}

	inline void operator /= (float divisor) {
		this->x /= divisor;
		this->y /= divisor;
	}
};

//  CVECTOR

class CVector {
public:
	float x, y, z;

	inline CVector() {}

	inline CVector(float X, float Y, float Z) {
		x = X; y = Y; z = Z;
	}

	inline CVector(CVector const& src) {
		x = src.x; y = src.y; z = src.z;
	}

	inline void Cross(CVector &a, CVector &b) {
		this->x = b.z * a.y - a.z * b.y;
		this->y = a.z * b.x - a.x * b.z;
		this->z = a.x * b.y - b.x * a.y;
	}

	inline float Magnitude() {
		return sqrtf(this->x * this->x + this->y * this->y + this->z * this->z);
	}

	inline void Sum(CVector &a, CVector &b) {
		this->x = a.x + b.x;
		this->y = a.y + b.y;
		this->z = a.z + b.z;
	}

	inline void Difference(CVector &a, CVector &b) {
		this->x = a.x - b.x;
		this->y = a.y - b.y;
		this->z = a.z - b.z;
	}

	inline void operator=(const CVector& right) {
		this->x = right.x;
		this->y = right.y;
		this->z = right.z;
	}

	inline void operator+=(const CVector& right) {
		this->x += right.x;
		this->y += right.y;
		this->z += right.z;
	}

	inline void operator-=(const CVector& right) {
		this->x -= right.x;
		this->y -= right.y;
		this->z -= right.z;
	}

	inline void operator *= (float multiplier) {
		this->x *= multiplier;
		this->y *= multiplier;
		this->z *= multiplier;
	}

	inline void operator /= (float divisor) {
		this->x /= divisor;
		this->y /= divisor;
		this->z /= divisor;
	}

	float Normalise();

	inline RwV3d ToRwV3d() {
		return{ x, y, z };
	}

	inline void FromRwV3d(RwV3d &rwvec) {
		x = rwvec.x; y = rwvec.y; z = rwvec.z;
	}

	inline void Set(float X, float Y, float Z) {
		x = X; y = Y; z = Z;
	}
};

inline CVector operator-(const CVector& vecOne, const CVector& vecTwo) {
	return CVector(vecOne.x - vecTwo.x, vecOne.y - vecTwo.y, vecOne.z - vecTwo.z);
}

inline CVector operator+(const CVector& vecOne, const CVector& vecTwo) {
	return CVector(vecOne.x + vecTwo.x, vecOne.y + vecTwo.y, vecOne.z + vecTwo.z);
}

inline CVector operator*(const CVector& vec, float multiplier) {
	return CVector(vec.x * multiplier, vec.y * multiplier, vec.z * multiplier);
}

inline CVector operator*(float multiplier, const CVector& vec) {
	return CVector(vec.x * multiplier, vec.y * multiplier, vec.z * multiplier);
}

inline float DistanceBetweenPoints(const CVector &pointOne, const CVector &pointTwo) {
	CVector diff = pointTwo - pointOne;
	return diff.Magnitude();
}

class CPathNode {
public:
	short m_wPathData1;
	short m_wPathData2;
	short m_wPosX;
	short m_wPosY;
	short m_wPosZ;
	char field_A[10];
};

class CColPoint {
public:
	CVector  m_vPoint;
	float field_C;
	CVector m_vNormal;
	float field_1C;
	unsigned char m_nSurfaceTypeA;
	unsigned char m_nPieceTypeA;
	unsigned char m_nSurfaceTypeB;
	unsigned char m_nPieceTypeB;
	int field_24;
};
#pragma pack(push, 1)
struct CMouseState {
	char lmb;
	char rmb;
	char mmb;
	char wheelUp;
	char wheelDown;
	char bmx1;
	char bmx2;
	char __padding;
	long X;
	long Y;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct CKeyState {
	short standardKey;
	short esc;
	short insert;
	short del;
	short home;
	short end;
	short pgup;
	short pgdn;
	short up;
	short down;
	short left;
	short right;
	short scroll;
	short pause;
	short numlock;
	short div;
	short mul;
	short sub;
	short add;
	short enter;
	short decimal;
	short num1;
	short num2;
	short num3;
	short num4;
	short num5;
	short num6;
	short num7;
	short num8;
	short num9;
	short num0;
	short back;
	short tab;
	short capslock;
	short extenter;
	short lshift;
	short rshift;
	short shift;
	short lctrl;
	short rctrl;
	short lmenu;
	short rmenu;
	short lwin;
	short rwin;
	short apps;
};
#pragma pack(pop)

#pragma pack(push, 1)
// Set values to 128 unless otherwise specified
class CControllerState {
public:
	signed short LeftStickX;
	signed short LeftStickY;
	signed short RightStickX;
	signed short RightStickY;

	signed short LeftShoulder1;
	signed short LeftShoulder2;
	signed short RightShoulder1;
	signed short RightShoulder2;

	signed short DPadUp;
	signed short DPadDown;
	signed short DPadLeft;
	signed short DPadRight;

	signed short Start;
	signed short Select;

	signed short ButtonSquare;
	signed short ButtonTriangle;
	signed short ButtonCross;
	signed short ButtonCircle;

	signed short ShockButtonL;
	signed short ShockButtonR;

	signed short m_bRadioTrackSkip;
};
#pragma pack(pop)
#endif

#pragma pack(1)
typedef struct _CAMERA_AIM
{
	CVector LookFront;
	CVector Source;
	CVector SourceBeforeLookBehind;
	CVector LookUp;

} CAMERA_AIM;

#pragma pack(1)
typedef struct _GTA_CONTROLSET
{
	DWORD dwFrontPad;
	WORD wKeys1[19];
	DWORD dwFrontPad2;
	WORD wKeys2[19];
	BYTE padding[0xC0];
} GTA_CONTROLSET;

typedef struct _PED_FLAGS
{
	unsigned char bIsStanding : 1;
	unsigned char bWasStanding : 1;
	unsigned char b03 : 1;
	unsigned char bIsPointingGunAt : 1;
	unsigned char bIsLooking : 1;
	unsigned char b06 : 1;
	unsigned char bIsRestoringLook : 1;
	unsigned char bIsAimingGun : 1;

	unsigned char bIsRestoringGun : 1;
	unsigned char bCanPointGunAtTarget : 1;
	unsigned char bIsTalking : 1;
	unsigned char bIsInTheAir : 1;
	unsigned char bIsLanding : 1;
	unsigned char bIsRunning : 1;
	unsigned char b15 : 1;
	unsigned char b16 : 1;

	unsigned char bCanPedEnterSeekedCar : 1;
	unsigned char bRespondsToThreats : 1;
	unsigned char bRenderPedInCar : 1;
	unsigned char b20 : 1;
	unsigned char bUpdateAnimHeading : 1;
	unsigned char bRemoveHead : 1;
	unsigned char bFiringWeapon : 1;
	unsigned char b24 : 1;

	unsigned char b25a : 1;
	unsigned char b25b : 1;
	unsigned char bPedIsBleeding : 1;
	unsigned char bStopAndShoot : 1;
	unsigned char bIsPedDieAnimPlaying : 1;
	unsigned char b30 : 1;
	unsigned char b31 : 1;
	unsigned char b32 : 1;

	unsigned char bKindaStayInSamePlace : 1;
	unsigned char bBeingChasedByPolice : 1;
	unsigned char bNotAllowedToDuck : 1;
	unsigned char bCrouchWhenShooting : 1;
	unsigned char bIsDucking : 1;
	unsigned char bGetUpAnimStarted : 1;
	unsigned char bDoBloodyFootprints : 1;
	unsigned char b40 : 1;

	unsigned char b41 : 1;
	unsigned char b42 : 1;
	unsigned char b43 : 1;
	unsigned char b44 : 1;
	unsigned char b45 : 1;
	unsigned char b46 : 1;
	unsigned char b47 : 1;
	unsigned char b48 : 1;

	unsigned char b49 : 1;
	unsigned char b50 : 1;
	unsigned char b51 : 1;
	unsigned char b52 : 1;
	unsigned char b53 : 1;
	unsigned char b54 : 1;
	unsigned char bIsBeingJacked : 1;
	unsigned char bFadeOut : 1;

	unsigned char b57 : 1;
	unsigned char b58 : 1;
	unsigned char b59 : 1;
	unsigned char bClearObjective : 1;
	unsigned char b61 : 1;
	unsigned char b62 : 1;
	unsigned char b63 : 1;
	unsigned char b64 : 1;

	unsigned char bShakeFist : 1;
	unsigned char bNoCriticalHits : 1;
	unsigned char b67 : 1;
	unsigned char bHasAlreadyBeenRecorded : 1;
	unsigned char bFallenDown : 1;
	unsigned char bUpdateMatricesRequired : 1;
	unsigned char b71 : 1;
	unsigned char b72 : 1;

	unsigned char b73 : 1;
	unsigned char b74 : 1;
	unsigned char b75 : 1;
	unsigned char b76 : 1;
	unsigned char b77 : 1;
	unsigned char bMiamiViceCop : 1;
	unsigned char bMoneyHasBeenGivenByScript : 1;
	unsigned char bHasBeenPhotographed : 1;

	unsigned char bIsDrowning : 1;
	unsigned char bDrownsInWater : 1;
	unsigned char bStayBehindIfLeaderTooFar : 1;
	unsigned char bUsingExitCarTimer : 1;
	unsigned char bScriptPedIsPlayerAlly : 1;
	unsigned char bKnockedUpIntoAir : 1;
	unsigned char bDeadPedInFrontOfCar : 1;
	unsigned char bStayInCarOnJack : 1;

	unsigned char b89 : 1;
	unsigned char bDoomAim : 1;
	unsigned char bCanBeShotInVehicle : 1;
	unsigned char b92 : 1;
	unsigned char b93 : 1;
	unsigned char b94 : 1;
	unsigned char b95 : 1;
	unsigned char b96 : 1;

	unsigned char b97 : 1;
	unsigned char bCrouchWhenScared : 1;
	unsigned char b99 : 1;
	unsigned char b100 : 1;
	unsigned char b101 : 1;
	unsigned char b102 : 1;
	unsigned char bPedWasSetOutOfCollision : 1;
	unsigned char bGangMemberReturnsFire : 1;
} PED_FLAGS;

struct PedSyncData 
{
	float Health;
	float Armour;
	float Rotation;

	bool Wander;

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
	float Health;
	float Armour;
	float Rotation;

	int iModelIndex;
	int iInteriorID;
	int iCurrentAnimID;

	bool isInVehicle;

	unsigned int CurrWep;
	unsigned int WepModelIndex;
	unsigned int Ammo;

	PED_FLAGS m_nPedFlags;

	CControllerState newPlayerKeys;
	CControllerState oldPlayerKeys;

	short WORD_EA;
	short Mode;
	short ShakeDur;
	short DisablePlayerControls;

	CAMERA_AIM playerLook;
};

struct VehicleSyncData
{
	int modelID;
	int driver;
	float Health;
	int vehicleID;
	int vehicleSeat;
	CVector vehiclePos;
	CVector vehicleAt;
	CVector vehicleRight;
	CVector vehicleUp;
	CVector moveSpeed;
	CVector turnSpeed;
};

//CWeapon*This, DWORD _EDX, CEntity* source, CEntity* target, CVector* start, CVector* end, CColPoint* colpoint, CVector2D ahead

struct bulletSyncData
{
	int player;
	int targetEntityID;
	CVector start;
	CVector end;
	CColPoint colPoint;
	CVector2D ahead;
};

struct deathData
{
	int killer;
	int weapon;
};

struct connectData
{
	char name[25];
	int playerId;
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
	VCOOP_CREATE_PED = LIBRG_EVENT_LAST,
	VCOOP_CREATE_VEHICLE,
	VCOOP_CREATE_OBJECT,

	VCOOP_SEND_MESSAGE,
	VCOOP_RECEIVE_MESSAGE,

	VCOOP_GET_LUA_SCRIPT,

	VCOOP_PED_IS_DEAD,
	VCOOP_BULLET_SYNC,
	VCOOP_RESPAWN_AFTER_DEATH,

	VCOOP_DISCONNECT,
	VCOOP_CONNECT,

	VCOOP_SPAWN_ALLOWED,

	VCOOP_DO_SOMETHING_ELSE,
};

enum {
	VCOOP_PLAYER,
	VCOOP_OBJECT,
	VCOOP_PED,
	VCOOP_VEHICLE,
};

extern std::vector<librg_entity_t*>	playerEntities;
extern char							playerNames[MAX_PLAYERS][25];
extern std::vector<librg_entity_t*>	otherEntities;

#if defined( _MSC_VER )
#pragma warning (disable : 4244)
#pragma warning (disable : 4348)
#pragma warning (disable : 4060)
#pragma warning (disable : 4018)
#endif