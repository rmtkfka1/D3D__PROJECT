#include "pch.h"
#include "CameraManager.h"
#include "KeyManager.h"
Matrix CameraManager::S_MatView;
Matrix CameraManager::S_MatProjection;

void CameraManager::Init()
{

}

void CameraManager::Update()
{
	_matView = XMMatrixLookToLH(_cameraPos, _cameraLook, _cameraUp);
	_matProjection = ::XMMatrixPerspectiveFovLH(_fov, WINDOW_WIDTH / WINDOW_HEIGHT, _near, _far);

	S_MatView = _matView;
	S_MatProjection = _matProjection;
}


void CameraManager::MouseUpdate()
{



}


void CameraManager::KeyUpdate()
{


}
