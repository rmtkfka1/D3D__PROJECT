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
#include "Utils.h"
Player::Player():HireacyObject(PlayerType::Player)
{

}
Player::~Player()
{
}
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
	CollisonUpdate();
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
		_camera->AddMove(diection * _speed * dt);
	}

	if (key->GetButton(KEY_TYPE::S))
	{
		_transform->GetRoot()->AddMove(-(diection * _speed * dt));
		_camera->AddMove(diection * _speed * dt);
	}

	if (key->GetButton(KEY_TYPE::D))
	{
		_transform->GetRoot()->AddMove((right * _speed * dt));
		_camera->AddMove(diection * _speed * dt);
	}

	if (key->GetButton(KEY_TYPE::A))
	{
		_transform->GetRoot()->AddMove(-(right * _speed * dt));
		_camera->AddMove(diection * _speed * dt);
	}

	if (_terrain)
	{
		auto& pos = _terrain->GetHeight(_transform->GetRoot()->GetLocalPosition());

		if (_transform->GetRoot()->GetLocalPosition().y < pos.y)
		{
			_transform->GetRoot()->SetLocalPosition(vec3(pos.x, pos.y, pos.z));
		}
	}



}


void Player::RotateUpdate()
{

	vec2 delataPos = KeyManager::GetInstance()->GetDeletaPos();
	_transform->GetRoot()->AddRotate(vec3(-delataPos.y, -delataPos.x, 0));
	_camera->Rotate(static_pointer_cast<Player>(shared_from_this()));

};

void Player::CameraPushData()
{
	_camera->PushData();
}

void Player::CollisonUpdate()
{

	if(_collisionDected)
	{
		CollisonRotate(_look, _dir, _angle, _rotationAxis);
	}

}

void Player::StartCollisionRotation(vec3 direction)
{

	CollisonManager::GetInstance()->Reset();
	_collisionDected = true;
	_look = GetTransform()->GetLook();
	_dir = direction;
	_dir.Normalize();
	_rotationAxis = _look.Cross(_dir);
	float dotProduct = _look.Dot(_dir);
	_angle = acosf(dotProduct);

}



void Player::CollisonRotate(vec3 look,vec3 dir ,float angle , vec3 rotationAxis)
{
	
	_addAngle +=  XMConvertToRadians(_rotationSpeed);

	if (_addAngle >= angle)
	{
		_addAngle = 0;
		_collisionDected = false;
		return;
	}

	// Create a quaternion representing the rotation
	Quaternion rotationQuat = Quaternion::CreateFromAxisAngle(rotationAxis, XMConvertToRadians(_rotationSpeed));

	vec3 rotate = GetTransform()->GetLocalRotation();
	Quaternion nowQuat = Quaternion::CreateFromYawPitchRoll(vec3(XMConvertToRadians(rotate.x), XMConvertToRadians(rotate.y), XMConvertToRadians(rotate.z)));

	Quaternion result = nowQuat * rotationQuat;

	vec3 resultEuler = result.ToEuler();

	GetTransform()->SetLocalRotation(vec3(XMConvertToDegrees(resultEuler.x), XMConvertToDegrees(resultEuler.y),0));



}
void Player::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{

	if (collider->GetName() == "raycheck" && other->GetName() == "boxbox")
	{
	
		//ray 를 여러방향으로 발사해봄.

		auto now = GetTransform()->GetLocalPosition();
		auto right = GetTransform()->GetRight();
		auto down = -GetTransform()->GetUp();
		auto left = -right;

		Ray rayleft = Ray(now, left);

		if (CollisonManager::GetInstance()->CheckRayCollusion(rayleft) == false)
		{
			StartCollisionRotation(left);
			return;
		}

		Ray rayright = Ray(now, right);
		
		if (CollisonManager::GetInstance()->CheckRayCollusion(rayright) == false)
		{
			StartCollisionRotation(right);
			return;
		}

		Ray RayDown = Ray(now, down);

		if (CollisonManager::GetInstance()->CheckRayCollusion(RayDown) == false)
		{
			StartCollisionRotation(down);
			return;
		}

	}


}

void Player::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
}