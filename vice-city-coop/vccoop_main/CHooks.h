#pragma once

namespace CHooks
{
	void InitHooks();
	void DoBulletImpact(CWeapon*This, CEntity* source, CEntity* target, CVector* start, CVector* end, CColPoint* colpoint, CVector2D ahead);
	CPad * GetPad(int pad);
	int FindFreeIDForPed();
}