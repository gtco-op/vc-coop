#include "main.h"
CNameTags::CNameTags() {}
CNameTags::~CNameTags() {}

void CNameTags::Draw()
{
	vector <pair<CPed*, int>> ::iterator it;
	for (it = gNetwork->players.begin(); it != gNetwork->players.end(); it++)
	{
		CPed * ped = it->first;
		int id = it->second;
		if (ped && ped != FindPlayerPed())
		{
			CVector camPos = TheCamera.m_vecGameCamPos;
			CVector posn = ped->GetPosition();

			if(!CWorld::GetIsLineOfSightClear(camPos, posn, true, false, false, false, false, false, false))continue;

			RwV3d screenCoors; float w, h;
			if (CSprite::CalcScreenCoors({ posn.x, posn.y, posn.z + 1.3f }, &screenCoors, &w, &h, true))
			{
				char text[600];
				if (ped->IsPlayer())
				{
					sprintf(text, "Player(%d)", id);

					SIZE size = gRender->MeasureText(text);

					gRender->RenderText(text, { (LONG)screenCoors.x - (LONG)ceil(size.cx / 2.0), (LONG)screenCoors.y }, 0xFFFFFFFF);

					CSprite2d::DrawRect(CRect(screenCoors.x - 60, screenCoors.y - 5 + 30, screenCoors.x + 60, screenCoors.y + 5 + 30), CRGBA(0, 0, 0, 255));
					CSprite2d::DrawRect(CRect(screenCoors.x - 60 + 3, screenCoors.y - 5 + 30 + 1, screenCoors.x + 60 - 3, screenCoors.y + 5 + 30 - 1), CRGBA(145, 0, 0, 255));
					CSprite2d::DrawRect(CRect(screenCoors.x - 60 + 3, screenCoors.y - 5 + 30 + 1, (screenCoors.x - 60 + 3) + (((screenCoors.x + 60 - 3) - (screenCoors.x - 60 + 3)) / 100.0f)*ped->m_fHealth, screenCoors.y + 5 + 30 - 1), CRGBA(255, 0, 0, 255));
				}
				else
				{

					sprintf(text, "Civil(%d)\nHP: %d", id, (int)ped->m_fHealth);

					SIZE size = gRender->MeasureText(text);

					gRender->RenderText(text, { (LONG)screenCoors.x - (LONG)ceil(size.cx / 2.0), (LONG)screenCoors.y }, 0xFFFFFFFF);
				}
			}
		}
	}
}