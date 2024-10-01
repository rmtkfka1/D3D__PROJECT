#include "pch.h"
#include "Player.h"
#include "KeyManager.h"
#include "Transform.h"
#include "TimeManager.h"
#include "Camera.h"
#include "Terrain.h"
void Player::Init()
{
	Super::Init();
	_camera = make_shared<ThirdPersonCamera>();

}

void Player::Update()
{
	
	MoveUpdate();
	RotateUpdate();
	_camera->UpdateShaderVariables();
	Super::Update();
}

void Player::Render()
{
	Super::Render();
}

void Player::MoveUpdate()
{
	

	float dt = TimeManager::GetInstance()->GetDeltaTime();
	auto key = KeyManager::GetInstance();

	vec3 diection = _transform->GetLook();
	vec3 right = _transform->GetRight();
	vec3 nowPos = _transform->GetLocalPosition();

	if (key->GetButton(KEY_TYPE::W))
	{
		_transform->Move(diection * _speed * dt);
	}

	if (key->GetButton(KEY_TYPE::S))
	{
		_transform->Move(-(diection * _speed * dt));
	}

	if (key->GetButton(KEY_TYPE::D))
	{
		_transform->Move(right * _speed * dt);
	}

	if (key->GetButton(KEY_TYPE::A))
	{
	    _transform->Move(-(right * _speed * dt));
	}


	auto& pos = _terrain->GetHeight(_transform->GetLocalPosition());
	if (_transform->GetLocalPosition().y < pos.y)
	{
		_transform->SetLocalPosition(vec3(pos.x, pos.y + 0.5f, pos.z));
	}

	_camera->Move(diection * _speed * dt);

}


void Player::RotateUpdate()
{
	float dx = KeyManager::GetInstance()->_dx;
	float dy = KeyManager::GetInstance()->_dy;

	auto now = GetTransform()->GetLocalRotation();
	GetTransform()->SetLocalRotation(vec3(now.x+dx, now.y + 180.0f +dy, 0));

	_camera->Rotate(static_pointer_cast<Player>(shared_from_this()));

}
