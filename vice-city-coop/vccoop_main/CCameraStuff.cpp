#include "main.h"

void CCameraStuff::SetPlayerCameraPosition(float fX, float fY, float fZ, float fRotationX, float fRotationY, float fRotationZ)
{
	TheCamera.SetCamPositionForFixedMode({ fX, fY, fZ }, { fRotationX, fRotationY, fRotationZ });
	//Command<0x015F>(fX, fY, fZ, fRotationX, fRotationY, fRotationZ);
}
void CCameraStuff::CameraLookAtPoint(float fX, float fY, float fZ, int iType)
{
	TheCamera.TakeControlNoEntity({ fX, fY, fZ }, iType, 1);
	//Command<0x0160>(fX, fY, fZ, iType);
}
void CCameraStuff::SetCameraBehindPlayer()
{
	TheCamera.SetCameraDirectlyBehindForFollowPed_CamOnAString();
	//Command<0x0373>();
}
void CCameraStuff::RestoreCamera()
{
	TheCamera.Restore();
	//Command<0x015A>();
}
CVector CCameraStuff::GetCameraPos()
{
	return TheCamera.pos;
	//return CVector(MemRead<float>(0x7E46B8), MemRead<float>(0x7E46BC), MemRead<float>(0x7E46C0));
}