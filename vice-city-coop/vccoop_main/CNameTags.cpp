#include "main.h"
CNameTags::CNameTags() {}
CNameTags::~CNameTags() {}

void CNameTags::Draw()
{ 
	int size = gNetwork->networkEntities.size();
	if (size <= 0)
		return;

	for(auto it : gNetwork->networkEntities)
	{
		CClientEntity * networkEntity = it;
		if (networkEntity->type == VCOOP_PLAYER)
		{
			CClientPlayer * player = (CClientPlayer*)networkEntity;
			CPed * ped = player->ped;
			if (ped && ped != LocalPlayer() && player->streamed)
			{
				CVector camPos = TheCamera.m_vecGameCamPos;
				CVector posn = ped->GetPosition();

				if (!CWorld::GetIsLineOfSightClear(camPos, posn, true, false, false, false, false, false, false))continue;

				/*float distance = DistanceBetweenPoints(camPos, posn);
				float above = distance * 0.25f;
				if (distance <= 5)above = 1.25f;
				if(distance > 10)above = above / (1.25 * 1.45);*/

				float above = 1.25f;
				if (ped->m_fArmour > 0)above = 1.5f;

				RwV3d screenCoors; float w, h;
				if (CSprite::CalcScreenCoors({ posn.x, posn.y, posn.z + 1.25f }, &screenCoors, &w, &h, true))
				{
					char text[600];
					if (ped->IsPlayer())
					{
						sprintf(text, "%s(%d)", player->szName, player->networkID);

						SIZE size = gRender->MeasureText(text);

						gRender->RenderText(text, { (LONG)screenCoors.x - (LONG)ceil(size.cx / 2.0), (LONG)screenCoors.y }, 0xFFFFFFFF);

						if (ped->m_fArmour > 0)
						{
							gRender->DrawProgressBar(CRect(screenCoors.x - 50, screenCoors.y - 5 + 25, screenCoors.x + 50, screenCoors.y + 5 + 25), (int)ped->m_fArmour, CRGBA(185, 185, 185, 255));
							gRender->DrawProgressBar(CRect(screenCoors.x - 50, screenCoors.y - 5 + 40, screenCoors.x + 50, screenCoors.y + 5 + 40), (int)ped->m_fHealth, CRGBA(255, 150, 225, 255));
						}
						else
						{
							gRender->DrawProgressBar(CRect(screenCoors.x - 50, screenCoors.y - 5 + 25, screenCoors.x + 50, screenCoors.y + 5 + 25), (int)ped->m_fHealth, CRGBA(255, 150, 225, 255));
						}

						//CSprite2d::DrawRect(CRect(screenCoors.x - 60, screenCoors.y - 5 + 30, screenCoors.x + 60, screenCoors.y + 5 + 30), CRGBA(0, 0, 0, 255));
						//CSprite2d::DrawRect(CRect(screenCoors.x - 60 + 3, screenCoors.y - 5 + 30 + 1, screenCoors.x + 60 - 3, screenCoors.y + 5 + 30 - 1), CRGBA(255, 95, 225, 255));
						//CSprite2d::DrawRect(CRect(screenCoors.x - 60 + 3, screenCoors.y - 5 + 30 + 1, (screenCoors.x - 60 + 3) + (((screenCoors.x + 60 - 3) - (screenCoors.x - 60 + 3)) / 100.0f)*ped->m_fHealth, screenCoors.y + 5 + 30 - 1), CRGBA(255, 150, 225, 255));
					}
				}
			}
		}
	}
}