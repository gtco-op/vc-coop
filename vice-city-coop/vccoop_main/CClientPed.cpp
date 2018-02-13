#include "main.h"

CClientPed::CClientPed(int nID)
{
	this->ped = NULL;

	CVector pos = FindPlayerPed()->GetPosition();

	bool res = false;

	int xrand = Random(0, 200) - 100;
	int yrand = Random(0, 200) - 100;

	int path = ThePaths.FindNodeClosestToCoors({ pos.x + xrand, pos.y + yrand, pos.z }, 1, 10000.0f, 1, 0, 0, 0);
	CPathNode node = ThePaths.nodes[path];

	CVector spawnPos = { (float)node.m_wPosX*0.125f, (float)node.m_wPosY*0.125f, CWorld::FindGroundZFor3DCoord((float)node.m_wPosX*0.125f, (float)node.m_wPosY*0.125f, (float)node.m_wPosZ*0.125f, &res) + 1.0f };

	this->ped = new CCivilianPed(ePedType::PEDTYPE_CIVMALE, 7);
	CWorld::Add(this->ped);
	this->ped->Teleport(spawnPos);
	this->ped->SetWanderPath(Random(0, 9650));

	this->networkID = nID;

	gLog->Log("[CClientPed]Network ID: %d\nPed pointer: 0x%X\n\n", nID, this->ped);
}



void CClientPed::SyncPed(PedSyncData spd)
{
	this->syncData = spd;

	ped->m_nModelIndex = spd.iModelIndex;
	ped->m_fHealth = spd.Health;
	ped->m_fRotationCur = spd.Rotation;
	ped->m_fArmour = spd.Armour;

	ped->m_fRotationDest = spd.m_fRotationDest;
	ped->m_fLookDirection = spd.m_fLookDirection;
	ped->m_vecAnimMoveDelta = spd.m_vecAnimMoveDelta;
	ped->m_dwAnimGroupId = spd.m_dwAnimGroupId;
	ped->m_vecMoveSpeed = spd.m_vecMoveSpeed;

	//Action sync
	ped->m_dwAction = spd.m_dwAction;
	ped->m_dwActionTimer = spd.m_dwActionTimer;
	ped->m_fActionX = spd.m_fActionX;
	ped->m_fActionY = spd.m_fActionY;

	//Objective sync
	ped->m_dwObjective = spd.m_dwObjective;
	ped->m_dwObjectiveTimer = spd.m_dwObjectiveTimer;
	ped->m_vecObjective = spd.m_vecObjective;
	ped->m_fObjectiveAngle = spd.m_fObjectiveAngle;
	ped->m_pObjectiveEntity = gNetwork->GetEntityFromNetworkID(spd.m_pObjectiveEntity);
	//ped->m_pObjectiveVehicle		= spd.m_pObjectiveVehicle;

	//Flee sync
	ped->m_dwFleeTimer = spd.m_dwFleeTimer;
	ped->m_fFleeFromPosX = spd.m_fFleeFromPosX;
	ped->m_fFleeFromPosY = spd.m_fFleeFromPosY;

	//Path sync
	ped->m_fPathNextNodeDir = spd.m_fPathNextNodeDir;
	ped->wRouteCurDir = spd.wRouteCurDir;
	ped->m_vecPathNextNode = spd.m_vecPathNextNode;
	ped->m_dwPathNodeTimer = spd.m_dwPathNodeTimer;
	ped->m_wCurPathNode = spd.m_wCurPathNode;
	ped->m_wPathNodes = spd.m_wPathNodes;

	for (int i = 0; i < 8; i++)
	{
		ped->m_aPathNodeStates[i] = spd.m_aPathNodeStates[i];
		//ped->m_apPathNodesStates[i] = spd.m_apPathNodesStates[i];
	}

	ped->m_dwPathNodeType = spd.m_dwPathNodeType;
	ped->m_nPathState = spd.m_nPathState;
	ped->m_pLastPathNode = spd.m_pLastPathNode;
	ped->m_pNextPathNode = spd.m_pNextPathNode;

	ped->SetMoveState((eMoveState)spd.m_dwMoveState);

	//Seek sync
	ped->m_fSeekExAngle = spd.m_fSeekExAngle;
	ped->m_vecSeekPosEx = spd.m_vecSeekPosEx;
	ped->m_vecOffsetSeek = spd.m_vecOffsetSeek;

	//Event sync
	ped->m_dwEventType = spd.m_dwEventType;
	ped->m_fAngleToEvent = spd.m_fAngleToEvent;
	ped->m_fEventOrThreatX = spd.m_fEventOrThreatX;
	ped->m_fEventOrThreatY = spd.m_fEventOrThreatY;
}

PedSyncData CClientPed::BuildSyncData()
{
	PedSyncData spd;

	spd.Health = ped->m_fHealth;
	spd.iCurrentAnimID = ped->m_dwAnimGroupId;
	spd.Armour = ped->m_fArmour;
	spd.iModelIndex = ped->m_nModelIndex;
	spd.Rotation = ped->m_fRotationCur;

	//Move sync
	spd.m_fRotationDest = ped->m_fRotationDest;
	spd.m_fLookDirection = ped->m_fLookDirection;
	spd.m_vecAnimMoveDelta = ped->m_vecAnimMoveDelta;
	spd.m_dwAnimGroupId = ped->m_dwAnimGroupId;
	spd.m_vecMoveSpeed = ped->m_vecMoveSpeed;

	//Action sync
	spd.m_dwAction = ped->m_dwAction;
	spd.m_dwActionTimer = ped->m_dwActionTimer;
	spd.m_fActionX = ped->m_fActionX;
	spd.m_fActionY = ped->m_fActionY;

	//Objective sync
	spd.m_dwObjective = ped->m_dwObjective;
	spd.m_dwObjectiveTimer = ped->m_dwObjectiveTimer;
	spd.m_vecObjective = ped->m_vecObjective;
	spd.m_fObjectiveAngle = ped->m_fObjectiveAngle;
	ped->m_pObjectiveEntity = gNetwork->GetEntityFromNetworkID(spd.m_pObjectiveEntity);
	//spd.m_pObjectiveVehicle		= ped->m_pObjectiveVehicle;

	//Flee sync
	spd.m_dwFleeTimer = ped->m_dwFleeTimer;
	spd.m_fFleeFromPosX = ped->m_fFleeFromPosX;
	spd.m_fFleeFromPosY = ped->m_fFleeFromPosY;

	//Path sync
	spd.m_fPathNextNodeDir = ped->m_fPathNextNodeDir;
	spd.wRouteCurDir = ped->wRouteCurDir;
	spd.m_vecPathNextNode = ped->m_vecPathNextNode;
	spd.m_dwPathNodeTimer = ped->m_dwPathNodeTimer;
	spd.m_wCurPathNode = ped->m_wCurPathNode;
	spd.m_wPathNodes = ped->m_wPathNodes;

	//Path node sync
	for (int i = 0; i < 8; i++)
	{
		spd.m_aPathNodeStates[i] = ped->m_aPathNodeStates[i];
		//spd.m_apPathNodesStates[i] = ped->m_apPathNodesStates[i];
	}

	spd.m_dwPathNodeType = ped->m_dwPathNodeType;
	spd.m_nPathState = ped->m_nPathState;
	spd.m_pLastPathNode = ped->m_pLastPathNode;
	spd.m_pNextPathNode = ped->m_pNextPathNode;

	//Move state sync
	spd.m_dwMoveState = ped->m_dwMoveState;

	//Seek sync
	spd.m_fSeekExAngle = ped->m_fSeekExAngle;
	spd.m_vecSeekPosEx = ped->m_vecSeekPosEx;
	spd.m_vecOffsetSeek = ped->m_vecOffsetSeek;

	//Event sync
	spd.m_dwEventType = ped->m_dwEventType;
	spd.m_fAngleToEvent = ped->m_fAngleToEvent;
	spd.m_fEventOrThreatX = ped->m_fEventOrThreatX;
	spd.m_fEventOrThreatY = ped->m_fEventOrThreatY;

	this->syncData = spd;

	return spd;
}