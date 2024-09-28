#include "pch.h"
#include "CameraManager.h"
#include "KeyManager.h"
#include "TimeManager.h"
Matrix CameraManager::S_MatView;
Matrix CameraManager::S_MatProjection;

void CameraManager::Init()
{
	RECT _rect;

	GetWindowRect(GetForegroundWindow(), &_rect);
	_centerScreen = { (_rect.right + _rect.left) / 2, (_rect.bottom + _rect.top) / 2 };
	_mousePos = _centerScreen;
}



void CameraManager::Update()
{

	MouseUpdate();
	KeyUpdate();


	_matView = XMMatrixLookToLH(_cameraPos, _cameraLook, _cameraUp);
	_matProjection = ::XMMatrixPerspectiveFovLH(_fov, WINDOW_WIDTH / WINDOW_HEIGHT, _near, _far);

	S_MatView = _matView;
	S_MatProjection = _matProjection;

}


void CameraManager::MouseUpdate()
{
	_mousePos = KeyManager::GetInstance()->GetMousePos();

	// 화면 중심과의 차이 계산
	float deltaPosX = static_cast<float>(_mousePos.x - _centerScreen.x);
	float deltaPosY = static_cast<float>(_mousePos.y - _centerScreen.y);

	// 시간 간격 얻기
	float dt = TimeManager::GetInstance()->GetDeltaTime();

	// Yaw와 Pitch 업데이트
	_cameraYaw += deltaPosX * _cameraSpeed * dt;
	_cameraPitch += deltaPosY * _cameraSpeed * dt;

	// Pitch 제한
	if (_cameraPitch > 89.0f) _cameraPitch = 89.0f;
	if (_cameraPitch < -89.0f) _cameraPitch = -89.0f;

	if (_cameraYaw < 0.0f) _cameraYaw += 360.0f;
	if (_cameraYaw > 360.0f) _cameraYaw -= 360.0f;

	vec3 cameraLook = vec3(0, 0, 1.0f);
	vec3 caemraRight = vec3(1.0f, 0, 0.0f);

	_cameraLook = _cameraLook.TransformNormal(cameraLook, Matrix::CreateFromYawPitchRoll(XMConvertToRadians(_cameraYaw), XMConvertToRadians(_cameraPitch), 0));
	_cameraRight = _cameraRight.TransformNormal(caemraRight, Matrix::CreateFromYawPitchRoll(XMConvertToRadians(_cameraYaw), XMConvertToRadians(_cameraPitch), 0));

	_cameraLook.Normalize();
	_cameraRight.Normalize();

	SetCursorPos(static_cast<int>(_centerScreen.x), static_cast<int>(_centerScreen.y));

}


void CameraManager::KeyUpdate()
{
	float dt = TimeManager::GetInstance()->GetDeltaTime();

	if (KeyManager::GetInstance()->GetButton(KEY_TYPE::W))
	{
		_cameraPos +=  dt * _cameraLook;
	}

	if (KeyManager::GetInstance()->GetButton(KEY_TYPE::S))
	{

		_cameraPos -= dt * _cameraLook;
	}

	if (KeyManager::GetInstance()->GetButton(KEY_TYPE::D))
	{
		_cameraPos += dt * _cameraRight;
	}

	if (KeyManager::GetInstance()->GetButton(KEY_TYPE::A))
	{
		_cameraPos -= dt * _cameraRight;
	}

}
