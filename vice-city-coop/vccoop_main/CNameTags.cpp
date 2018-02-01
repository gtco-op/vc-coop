#include "main.h"
CNameTags::CNameTags() {}
CNameTags::~CNameTags() {}

void CNameTags::Draw()
{
	for (int i = 0; i < CPools::ms_pPedPool->m_nSize; i++)
	{
		CPed *ped = CPools::ms_pPedPool->GetAt(i);
		if (ped && ped != FindPlayerPed())
		{
			CVector posn = ped->GetPosition();
			RwV3d screenCoors; float w, h;
			if (CSprite::CalcScreenCoors({ posn.x, posn.y, posn.z + 1.3f }, &screenCoors, &w, &h, true))
			{
				char text[600];

				sprintf(text, "Ped: %d", i);

				SIZE size = gRender->MeasureText(text);

				gRender->RenderText(text, { (LONG)screenCoors.x - (LONG)ceil(size.cx / 2.0), (LONG)screenCoors.y }, 0xFFFFFFFF);

				CSprite2d::DrawRect(CRect(screenCoors.x - 60, screenCoors.y - 5 + 30, screenCoors.x + 60, screenCoors.y + 5 + 30), CRGBA(0, 0, 0, 255));
				CSprite2d::DrawRect(CRect(screenCoors.x - 60 + 3, screenCoors.y - 5 + 30 + 1, screenCoors.x + 60 - 3, screenCoors.y + 5 + 30 - 1), CRGBA(145, 0, 0, 255));
				CSprite2d::DrawRect(CRect(screenCoors.x - 60 + 3, screenCoors.y - 5 + 30 + 1, (screenCoors.x - 60 + 3) + (((screenCoors.x + 60 - 3) - (screenCoors.x - 60 + 3)) / 100.0f)*ped->m_fHealth, screenCoors.y + 5 + 30 - 1), CRGBA(255, 0, 0, 255));
			}
		}
	}
}