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
	BoundaryCheck();
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
	
	float result = rotation * dt; //1�ʿ� 2���� ȸ���ϵ���

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

void Player::BoundaryCheck()
{

	if (KeyManager::GetInstance()->GetButtonDown(KEY_TYPE::ENTER))
	{
		CollisionDected = true;
		rotationProgress = 0.0f;  // ���� ���� �� �ʱ�ȭ

		// ���� ȸ�� ���¸� ���� ȸ������ ����
		startRotation = Quaternion::CreateFromYawPitchRoll(
			XMConvertToRadians(GetTransform()->GetLocalRotation().y),
			XMConvertToRadians(GetTransform()->GetLocalRotation().x),
			XMConvertToRadians(GetTransform()->GetLocalRotation().z)
		);

		// ��ǥ ȸ�� ���� (look���� right�� ȸ��)
		auto look = GetTransform()->GetLook();
		vec3 goingVec = GetTransform()->GetRight();
		targetRotation = Quaternion::FromToRotation(look, goingVec);
	}

	if (CollisionDected)
	{
		// �� �����Ӹ��� ȸ�� ���� ����
		float rotationSpeed = 1.0f;  // ���� �ӵ�, ���� �������� ������ ȸ��
		rotationProgress += rotationSpeed * TimeManager::GetInstance()->GetDeltaTime();

		// `Slerp`�� �ε巴�� ȸ��
		Quaternion smoothRotation = Quaternion::Slerp(startRotation, targetRotation, rotationProgress);
		vec3 result = smoothRotation.ToEuler();

		GetTransform()->SetLocalRotation(vec3(
			XMConvertToDegrees(result.x),
			XMConvertToDegrees(result.y),
			XMConvertToDegrees(result.z)
		));
		GetTransform()->Update();

		if (rotationProgress >= 1.0f)
		{
			CollisionDected = false;
		}


	}
}
void Player::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
	//SceneManager::GetInstance()->GetCurrentScene()->ReserveDeleteGameObject(other->GetOwner());
	//CollisonManager::GetInstance()->ReserveDeleteCollider(other);
}

void Player::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
}