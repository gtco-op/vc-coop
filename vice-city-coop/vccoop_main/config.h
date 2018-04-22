#pragma once

// Define this for verbose log output
//#define VCCOOP_VERBOSE_LOG

// Define this for verbose lua output (in server)
//#define VCCOOP_LUA_VERBOSE_LOG

// Temporary spawn position
#define VCCOOP_DEFAULT_SPAWN_POSITION	531.629761f, 606.497253f, 10.901563f

// Default camera coordinates
#define VCCOOP_DEFAULT_CAMERA_POSITION		627.544556f, 599.654846f, 12.071713f
#define VCCOOP_DEFAULT_CAMERA_LOOKPOSITION	617.544556f, 589.654846f, 14.071713f

// Default nickname
#define VCCOOP_DEFAULT_NICKNAME			"Nickname123"

// Default server address
#define VCCOOP_DEFAULT_SERVER_ADDRESS	"127.0.0.1"

// Default server port
#define VCCOOP_DEFAULT_SERVER_PORT		23546

// Default web server port
#define VCCOOP_DEFAULT_WEBSERVER_PORT	"8080"

// Default web server root dir
#define VCCOOP_DEFAULT_WEBSERVER_DIR	".\\Management"

// Default server secret
#define VCCOOP_DEFAULT_SERVER_SECRET	1007455905

// Default server config filename
#define VCCOOP_DEFAULT_SERVER_CONFIG	"server.ini"

// Default client config filename
#define VCCOOP_DEFAULT_CLIENT_CONFIG	GetExecutablePath().append("\\scripts\\vccoop\\client.ini").c_str()

// Default client path to main.scm
#define VCCOOP_DEFAULT_MAIN_SCM_PATH	"\\data\\main.scm"

// Default masterlist URL
#define VCCOOP_DEFAULT_MASTER_LIST_URL			"http://www.grandtheftcoop.ml/master/check/servers.xml"
#define VCCOOP_DEFAULT_MASTER_LIST_BACKUP_URL	"http://159.65.62.71/check/servers.xml"

// Print game's internal debug messages
#define VCCOOP_DEBUG_ENGINE

// Default client tick delay
#define VCCOOP_CLIENT_TICK_DELAY		1

// Default server tick delay
#define VCCOOP_SERVER_TICK_DELAY		16

// Default max players
#define MAX_PLAYERS						500

// Default max objects
#define MAX_OBJECTS						1000

// Default max vehicles
#define MAX_VEHICLES					1000

// Default max peds
#define MAX_PEDS						1000

// Internal-use only
#define MAX_ENTITIES					(MAX_PEDS+MAX_OBJECTS+MAX_VEHICLES)

//------------------------------------------------------------------------------------------
#ifndef VCCOOP_SERVER 
	#define VCCOOP_NAME									"VC CO-OP"
	#define VCCOOP_VER									"0.02"

	#ifdef VCCOOP_DEBUG
		#define VCCOOP_WINDOW_TITLE						"Vice City CO-OP [Debug] " VCCOOP_VER
	#else
		#define VCCOOP_WINDOW_TITLE						"Vice City CO-OP " VCCOOP_VER
	#endif
#else
	#define VCCOOP_VER									"0.02a"
	#ifdef _MSC_VER
			#ifdef VCCOOP_DEBUG
				#define VCCOOP_NAME						"VC CO-OP Debug Server"
			#else
				#define VCCOOP_NAME						"VC CO-OP Server"
			#endif
			#define VCCOOP_SERVER_WINDOW_TITLE			VCCOOP_NAME " " VCCOOP_VER
	#else
			#ifdef VCCOOP_DEBUG
				#define VCCOOP_NAME						"VC CO-OP Debug Server"
			#else
				#define VCCOOP_NAME						"VC CO-OP Server"
			#endif	
	#endif
#endif

#define DEFAULT_TIME_FORMAT				"%Y%m%d_%H%M%S"
#define LOGGER_TIME_FORMAT				"%H:%M:%S"

#define GTA_SET_TIMER(timer) (CTimer::m_snTimeInMilliseconds + timer)
#define GTA_GET_TIMER(timer) (timer - CTimer::m_snTimeInMilliseconds)

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

#if defined(VCCOOP_SERVER) || !defined(_MSC_VER)
enum ePedAction : unsigned int
{
	None = 0,
	Normal = 1,
	TurnBody1 = 2,
	TurnBody2 = 3,
	WanderPath = 5,
	FleeFromPos = 8,
	FleeFromEnt = 9,
	FollowCheckpoint = 11,
	Aiming = 12,
	Shooting = 16,
	Fighting = 17,
	Chatting = 20,
	PointGun = 22,
	SeekCar = 24,
	SeekBoat = 25,
	Investigate = 30,
	RunOnFire = 32,
	Jumping = 41,
	LyingOnGround = 42,
	GettingUp = 43,
	JumpingFromVehicle = 44,
	Driving = 50,
	Dying = 54,
	Wasted = 55,
	JackingVehicle = 56,
	EnteringVehicle = 58,
	ExitingVehicle = 60
};


enum eObjective {
	OBJECTIVE_NO_OBJ = 0,
	OBJECTIVE_WAIT_ON_FOOT = 1,
	OBJECTIVE_WAIT_ON_FOOT_FOR_COP = 2,
	OBJECTIVE_FLEE_ON_FOOT_TILL_SAFE = 3,
	OBJECTIVE_GUARD_SPOT = 4,
	OBJECTIVE_GUARD_AREA = 5,
	OBJECTIVE_WAIT_IN_CAR = 6,
	OBJECTIVE_WAIT_IN_CAR_THEN_GETOUT = 7,
	OBJECTIVE_KILL_CHAR_ON_FOOT = 8,
	OBJECTIVE_KILL_CHAR_ANY_MEANS = 9,
	OBJECTIVE_FLEE_CHAR_ON_FOOT_TILL_SAFE = 10,
	OBJECTIVE_FLEE_CHAR_ON_FOOT_ALWAYS = 11,
	OBJECTIVE_GOTO_CHAR_ON_FOOT = 12,
	OBJECTIVE_GOTO_CHAR_ON_FOOT_WALKING = 13,
	OBJECTIVE_HASSLE_CHAR = 14,
	OBJECTIVE_FOLLOW_CHAR_IN_FORMATION = 15,
	OBJECTIVE_LEAVE_CAR = 16,
	OBJECTIVE_ENTER_CAR_AS_PASSENGER = 17,
	OBJECTIVE_ENTER_CAR_AS_DRIVER = 18,
	OBJECTIVE_FOLLOW_CAR_IN_CAR = 19,
	OBJECTIVE_FIRE_AT_OBJ_FROM_VEHICLE = 20,
	OBJECTIVE_DESTROY_OBJ = 21,
	OBJECTIVE_DESTROY_CAR = 22,
	OBJECTIVE_GOTO_AREA_ANY_MEANS = 23,
	OBJECTIVE_GOTO_AREA_ON_FOOT = 24,
	OBJECTIVE_RUN_TO_AREA = 25,
	OBJECTIVE_GOTO_AREA_IN_CAR = 26,
	OBJECTIVE_FOLLOW_CAR_ON_FOOT_WOFFSET = 27,
	OBJECTIVE_GUARD_ATTACK = 28,
	OBJECTIVE_SET_LEADER = 29,
	OBJECTIVE_FOLLOW_ROUTE = 30,
	OBJECTIVE_SOLICIT_VEHICLE = 31,
	OBJECTIVE_TAKE_TAXI = 32,
	OBJECTIVE_CATCH_TRAIN = 33,
	OBJECTIVE_BUY_ICECREAM = 34,
	OBJECTIVE_STEAL_ANY_CAR = 35,
	OBJECTIVE_STEAL_ANY_MISSION_CAR = 36,
	OBJECTIVE_MUG_CHAR = 37,
	OBJECTIVE_LV_CAR_DIE = 38,
	OBJECTIVE_GOTO_SEAT = 39,
	OBJECTIVE_GOTO_ATM = 40,
	OBJECTIVE_FLEE_CAR = 41,
	OBJECTIVE_SUNBATHE = 42,
	OBJECTIVE_GOTO_BUS_STOP = 43,
	OBJECTIVE_GOTO_PIZZA = 44,
	OBJECTIVE_GOTO_SHELTER = 45,
	OBJECTIVE_AIM_GUN_AT = 46,
	OBJECTIVE_WANDER = 47,
	OBJECTIVE_WAIT_ON_FOOT_AT_SHLTR = 48,
	OBJECTIVE_SPRINT_TO_AREA = 49,
	OBJECTIVE_KILL_CHAR_ON_BOAT = 50,
	OBJECTIVE_SOLICIT_PED = 51,
	OBJECTIVE_WAIT_AT_BUS_STOP = 52,
	OBJECTIVE_GOTO_ICE_CREAM_VAN_FOOT = 53,
	OBJECTIVE_WAIT_FOOT_ICECREAM_VAN = 54
};
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
	float Health = 100.f;
	float Armour;
	float Rotation;

	float OrientX;
	float OrientY;
	float OrientZ;

	bool Wander = true;
	bool DontDestroy = false;

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

	ePedAction m_dwAction;
	int m_dwActionTimer;
	float m_dwActionX;
	float m_dwActionY;

	eObjective m_dwObjective;
	int m_dwObjectiveTimer;

	CVector m_vecObjective;

	float m_fObjectiveAngle;
	int m_pObjectiveEntity;
	int m_pObjectiveVehicle;
	
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
	float Health = 100.f;
	float Armour;
	float Rotation;

	float OrientX;
	float OrientY;
	float OrientZ;

	int iModelIndex;
	int iInteriorID;
	int iCurrentAnimID;

	bool isInVehicle;
	int vehicleID;
	int vehicleSeat;

	eObjective objective;

	unsigned int CurrWep = -1;
	unsigned int WepModelIndex = -1;
	unsigned int Ammo = -1;

	PED_FLAGS m_nPedFlags;

	CControllerState newPlayerKeys;
	CControllerState oldPlayerKeys;

	CAMERA_AIM playerLook;
};

struct VehicleSyncData
{
	int modelID;
	int driver;
	
	int passengers[8];
	unsigned char NumPassengers;
	unsigned char NumGettingIn;
	unsigned char GettingInFlags;
	unsigned char GettingOutFlags;
	unsigned char MaxPassengers;

	float Health = 100.f;
	int vehicleID;
	int vehicleSeat;

	unsigned char nPrimaryColor;
	unsigned char nSecondaryColor;

	float OrientX;
	float OrientY;
	float OrientZ;

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

struct ObjectSyncData {
	int modelID;
	int objectID;
	CVector objectPos;
};
 
struct serverInfo {
	char serverID[16];
	char serverName[128];
	char serverHost[64];
	char serverPort[16];  
}; 
 
struct sStartParams
{
	char name[25] = { 0 };
	char serveraddress[64] = { 0 };
	int serverport = 0;
	bool bEmpty = true;
	bool bConnect = false;
	bool bWebRequest = false; 
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

	VCOOP_START_MISSION_SCRIPT,

	VCOOP_GET_LUA_SCRIPT,

	VCOOP_PED_IS_DEAD,
	VCOOP_BULLET_SYNC,
	VCOOP_RESPAWN_AFTER_DEATH,

	VCOOP_DISCONNECT,
	VCOOP_CONNECT,

	VCOOP_SPAWN_ALLOWED,

	VCOOP_RECEIVE_SPD_UPDATE,

	VCOOP_DO_SOMETHING_ELSE,
};

enum {
	VCOOP_PLAYER,
	VCOOP_OBJECT,
	VCOOP_PED,
	VCOOP_VEHICLE,
};

extern char				playerNames[MAX_PLAYERS][25];
extern librg_entity_t*	playerEntities[MAX_PLAYERS];
extern librg_entity_t*	otherEntities[MAX_ENTITIES];

extern bool bIsLoadingModel;
extern void WaitForModelLoad(int modelIndex);

extern void LoadMissionScript();

#if defined( _MSC_VER )
#pragma warning (disable : 4244)
#pragma warning (disable : 4348)
#pragma warning (disable : 4060)
#pragma warning (disable : 4018)
#pragma warning (disable : 4267)
#endif