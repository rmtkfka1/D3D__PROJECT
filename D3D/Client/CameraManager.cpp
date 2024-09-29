#include "pch.h"
#include "CameraManager.h"
#include "KeyManager.h"
#include "TimeManager.h"
#include "CustomObject.h"
#include "Transform.h"
Matrix CameraManager::S_MatView;
Matrix CameraManager::S_MatProjection;
bool CameraManager::SetCursorMode = true;

void CameraManager::Init()
{
	RECT _rect;

	GetWindowRect(GetForegroundWindow(), &_rect);
	_centerScreen = { (_rect.right + _rect.left) / 2, (_rect.bottom + _rect.top) / 2 };
	_mousePos = _centerScreen;
}



void CameraManager::Update()
{

	if (KeyManager::GetInstance()->GetButtonDown(KEY_TYPE::TWO))
	{
		ChangeCamera(OBSERVER);
	}

	if (KeyManager::GetInstance()->GetButtonDown(KEY_TYPE::THREE))
	{
		ChangeCamera(THIRD_PERSON);
	}

	switch (_mode)
	{
	case OBSERVER:
		MouseUpdate();
		ObserverUpdate();
		RegenerateMatrix(_observer.cameraPos, _observer.cameraLook, _observer.cameraUp);
		break;
	case THIRD_PERSON:
		MouseUpdate();
		PlayerUpdate();
		ThirdCameraUpdate();
		RegenerateMatrix(_thrid.cameraPos, _thrid.cameraLook, _thrid.cameraUp);
		break;
	default:
		break;
	}

}


void CameraManager::RegenerateMatrix(vec3 cameraPos, vec3 cameraLook, vec3 CameraUp)
{
	_matView = XMMatrixLookToLH(cameraPos, cameraLook, CameraUp);
	_matProjection = ::XMMatrixPerspectiveFovLH(_fov, WINDOW_WIDTH / WINDOW_HEIGHT, _near, _far);

	S_MatView = _matView;
	S_MatProjection = _matProjection;
}

void CameraManager::MouseUpdate()
{

	if (SetCursorMode == false)
		return;

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

	SetCursorPos(static_cast<int>(_centerScreen.x), static_cast<int>(_centerScreen.y));

}

void CameraManager::PlayerUpdate()
{
	auto now = _player->GetTransform()->GetLocalRotation();

	_player->GetTransform()->SetLocalRotation(vec3(now.x + _cameraPitch, now.y + _cameraYaw +180.0f, 0));


}

void CameraManager::ThirdCameraUpdate()
{
	auto mat = _player->GetTransform()->GetWorldMatrix();

	//오프셋을 플레이어의 회전한 만큼 이동시킴.
	vec3 offset = vec3::TransformNormal(_offset, mat);
	vec3 targetPos = _player->GetTransform()->GetLocalPosition() + offset;

	//direction 을 구함.
	vec3 direction = vec3(targetPos - _thrid.cameraPos);

	float length = direction.Length();
	direction.Normalize();

	float dist = length * 0.3f;

	_thrid.cameraPos = _thrid.cameraPos + dist * direction;

	Matrix mat2 = XMMatrixLookAtLH(_thrid.cameraPos, _player->GetTransform()->GetLocalPosition(), _player->GetTransform()->GetUp());
	_thrid.cameraRight = vec3(mat2._11, mat2._21, mat2._31);
	_thrid.cameraUp = vec3(mat2._12, mat2._22, mat2._32);
	_thrid.cameraLook = vec3(mat2._13, mat2._23, mat2._33);

}

void CameraManager::ObserverUpdate()
{
	vec3 cameraLook = vec3(0, 0, 1.0f);
	vec3 caemraRight = vec3(1.0f, 0, 0.0f);

	_observer.cameraLook = cameraLook.TransformNormal(cameraLook, Matrix::CreateFromYawPitchRoll(XMConvertToRadians(_cameraYaw), XMConvertToRadians(_cameraPitch), 0));
	_observer.cameraRight = caemraRight.TransformNormal(caemraRight, Matrix::CreateFromYawPitchRoll(XMConvertToRadians(_cameraYaw), XMConvertToRadians(_cameraPitch), 0));

	_observer.cameraLook.Normalize();
	_observer.cameraRight.Normalize();

	const int speed = 10.0f;

	float dt = TimeManager::GetInstance()->GetDeltaTime();

	if (KeyManager::GetInstance()->GetButton(KEY_TYPE::W))
	{
		_observer.cameraPos += speed *dt * _observer.cameraLook;
	}

	if (KeyManager::GetInstance()->GetButton(KEY_TYPE::S))
	{

		_observer.cameraPos -= speed* dt * _observer.cameraLook;
	}

	if (KeyManager::GetInstance()->GetButton(KEY_TYPE::D))
	{
		_observer.cameraPos += speed *dt * _observer.cameraRight;
	}

	if (KeyManager::GetInstance()->GetButton(KEY_TYPE::A))
	{
		_observer.cameraPos -= speed *dt * _observer.cameraRight;
	}


}

void CameraManager::ChangeCamera(CameraMode mode)
{
	if (_mode == mode)
		return;

	_mode = mode;

	switch (mode)
	{
	case OBSERVER:
		break;
	case THIRD_PERSON:
		break;
	default:
		break;
	}




}





