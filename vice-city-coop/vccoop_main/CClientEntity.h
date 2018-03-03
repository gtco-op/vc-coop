#pragma once

class CClientEntity
{
public:
	int type;
	int networkID;
	
	virtual CEntity* GetEntity() { return NULL; };
};