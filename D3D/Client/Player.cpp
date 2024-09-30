#include "pch.h"
#include "Player.h"
#include "KeyManager.h"
#include "Transform.h"
#include "TimeManager.h"
#include "CameraManager.h"
void Player::Init()
{
	Super::Init();
}

void Player::Update()
{
	
	MoveUpdate();
	CameraManager::GetInstance()->Update();
	Super::Update();
}

void Player::Render()
{
	Super::Render();
}

void Player::MoveUpdate()
{
	if (CameraManager::GetInstance()->_mode == CameraMode::OBSERVER)
		return;

	float dt = TimeManager::GetInstance()->GetDeltaTime();
	auto key = KeyManager::GetInstance();

	vec3 diection = _transform->GetLook();
	vec3 right = _transform->GetRight();
	vec3 nowPos = _transform->GetLocalPosition();



	if (key->GetButton(KEY_TYPE::W))
	{
		_transform->SetLocalPosition(nowPos + diection * _speed * dt);
	}

	if (key->GetButton(KEY_TYPE::S))
	{
		_transform->SetLocalPosition(nowPos - diection * _speed * dt);
	}

	if (key->GetButton(KEY_TYPE::D))
	{
		_transform->SetLocalPosition(nowPos + right * _speed * dt);
	}

	if (key->GetButton(KEY_TYPE::A))
	{
		_transform->SetLocalPosition(nowPos - right * _speed * dt);
	}
}
