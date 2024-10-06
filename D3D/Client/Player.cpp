#include "pch.h"
#include "Player.h"
#include "KeyManager.h"
#include "Transform.h"
#include "TransformTree.h"
#include "TimeManager.h"
#include "Camera.h"
#include "Terrain.h"

void Player::Init()
{
	
}

void Player::Update()
{

	MoveUpdate();
	RotateUpdate();
	AnimateUpdate();
	CameraPushData();

	Super::Update();
}

void Player::Render()
{
	Super::Render();
}

void Player::AnimateUpdate()
{

	static int i = 0;
	// Top_Rotor의 Y축 로컬 회전을 업데이트
	_transform->findByName(L"Top_Rotor")->RotateShift(vec3(0, i++, 0));

}

void Player::MoveUpdate()
{


	float dt = TimeManager::GetInstance()->GetDeltaTime();
	auto key = KeyManager::GetInstance();

	vec3 diection = _transform->GetRoot()->GetLook();
	vec3 right = _transform->GetRoot()->GetRight();
	vec3 nowPos = _transform->GetRoot()->GetLocalPosition();

	if (key->GetButton(KEY_TYPE::W))
	{
		_transform->GetRoot()->MoveShift(-(diection * _speed * dt));
	}

	if (key->GetButton(KEY_TYPE::S))
	{
		_transform->GetRoot()->MoveShift((diection * _speed * dt));
	}

	if (key->GetButton(KEY_TYPE::D))
	{
		_transform->GetRoot()->MoveShift(-(right * _speed * dt));
	}

	if (key->GetButton(KEY_TYPE::A))
	{
		_transform->GetRoot()->MoveShift((right * _speed * dt));
	}

	if (_terrain)
	{
		auto& pos = _terrain->GetHeight(_transform->GetRoot()->GetLocalPosition());

		if (_transform->GetRoot()->GetLocalPosition().y < pos.y)
		{
			_transform->GetRoot()->SetLocalPosition(vec3(pos.x, pos.y, pos.z));
		}
	}

	_camera->MoveShift(diection * _speed * dt);

}


void Player::RotateUpdate()
{

	vec2 delataPos = KeyManager::GetInstance()->GetDeletaPos();
	_transform->GetRoot()->RotateShift(vec3(delataPos.y, delataPos.x, 0));
	_camera->Rotate(static_pointer_cast<Player>(shared_from_this()));

};

void Player::CameraPushData()
{
	_camera->PushData();
}
