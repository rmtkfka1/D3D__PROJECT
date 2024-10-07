#include "pch.h"
#include "Player.h"
#include "KeyManager.h"
#include "Transform.h"
#include "TransformTree.h"
#include "TimeManager.h"
#include "Camera.h"
#include "Terrain.h"
#include "SceneManager.h"
#include "Scene.h"
#include "CollisonManager.h"
#include "CameraManager.h"
void Player::Init()
{
	
}

void Player::Update()
{
	if (CameraManager::GetInstance()->GetCameraType() == CameraType::THIRDVIEW)
	{

		MoveUpdate();
		RotateUpdate();
		CameraPushData();
	}

	AnimateUpdate();
	Super::Update();
}

void Player::Render()
{
	Super::Render();
}



void Player::AnimateUpdate()
{
	float dt = TimeManager::GetInstance()->GetDeltaTime();
	float rotation = 720.0f;
	
	float result = rotation * dt; //1초에 2바퀴 회전하도록

	_transform->findByName(L"Top_Rotor")->AddRotate(vec3(0, result, 0));

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
		_transform->GetRoot()->AddMove((diection * _speed * dt));
	}

	if (key->GetButton(KEY_TYPE::S))
	{
		_transform->GetRoot()->AddMove(-(diection * _speed * dt));
	}

	if (key->GetButton(KEY_TYPE::D))
	{
		_transform->GetRoot()->AddMove(-(right * _speed * dt));
	}

	if (key->GetButton(KEY_TYPE::A))
	{
		_transform->GetRoot()->AddMove((right * _speed * dt));
	}

	if (_terrain)
	{
		auto& pos = _terrain->GetHeight(_transform->GetRoot()->GetLocalPosition());

		if (_transform->GetRoot()->GetLocalPosition().y < pos.y)
		{
			_transform->GetRoot()->SetLocalPosition(vec3(pos.x, pos.y, pos.z));
		}
	}

	_camera->AddMove(diection * _speed * dt);

}


void Player::RotateUpdate()
{

	vec2 delataPos = KeyManager::GetInstance()->GetDeletaPos();
	_transform->GetRoot()->SetLocalRotation(vec3(delataPos.y, delataPos.x, 0));
	_camera->Rotate(static_pointer_cast<Player>(shared_from_this()));

};

void Player::CameraPushData()
{
	_camera->PushData();
}

void Player::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
	//SceneManager::GetInstance()->GetCurrentScene()->ReserveDeleteGameObject(other->GetOwner());
	//CollisonManager::GetInstance()->ReserveDeleteCollider(other);
}

void Player::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
}