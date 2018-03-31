#pragma once

namespace CHooks
{
	void InitHooks();
	void DoBulletImpact(CWeapon*This, CEntity* source, CEntity* target, CVector* start, CVector* end, CColPoint* colpoint, CVector2D ahead);
	int FindFreeIDForPed();
	template<class A, class B = A>
	void InitPool(CPool<A, B> *pool, int nSize);
}