#pragma once
namespace CCameraStuff
{
	void SetPlayerCameraPosition(float fX, float fY, float fZ, float fRotationX, float fRotationY, float fRotationZ);
	void CameraLookAtPoint(float fX, float fY, float fZ, int iType);
	void SetCameraBehindPlayer();
	void RestoreCamera();
	CVector GetCameraPos();
}