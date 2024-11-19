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
#include <random>
#include "PlayerBullet.h"
#include "Model.h"
#include "GeoMetryHelper.h"
#include "Shader.h"
#include "ResourceManager.h"
std::random_device rd;
std::mt19937 g(rd());

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
		CollisonUpdate();
	}

	AnimateUpdate();
	Shot();
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

	if (_collisionDected == false)
	{
		if (key->GetButton(KEY_TYPE::W))
		{
			_transform->GetRoot()->AddMove((diection * _speed * dt));
			_camera->AddMove(diection * _speed * dt);
		}

		//if (key->GetButton(KEY_TYPE::S))
		//{
		//	_transform->GetRoot()->AddMove(-(diection * _speed * dt));
		//	_camera->AddMove(diection * _speed * dt);
		//}

		//if (key->GetButton(KEY_TYPE::D))
		//{
		//	_transform->GetRoot()->AddMove((right * _speed * dt));
		//	_camera->AddMove(diection * _speed * dt);
		//}

		//if (key->GetButton(KEY_TYPE::A))
		//{
		//	_transform->GetRoot()->AddMove(-(right * _speed * dt));
		//	_camera->AddMove(diection * _speed * dt);
		//}
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
	if (_collisionDected == false)
	{
		vec2 delataPos = KeyManager::GetInstance()->GetDeletaPos();
		_transform->GetRoot()->AddRotate(vec3(-delataPos.y, -delataPos.x, 0));
	}

	_camera->Rotate(static_pointer_cast<Player>(shared_from_this()));

};

void Player::CameraPushData()
{
	_camera->SetData();
}


void Player::Shot()
{
	if (KeyManager::GetInstance()->GetButtonDown(KEY_TYPE::SPACE))
	{
		shared_ptr<PlayerBullet> object = make_shared<PlayerBullet>();
		shared_ptr<Model> data = Model::ReadData(L"playerBullet/playerBullet", L"playerBullet");
		object->SetModel(data);
		object->_direction = GetTransform()->GetLook();
		auto& pos = this->GetTransform()->GetLocalPosition() + GetTransform()->GetLook()*100.0f;
		object->GetTransform()->SetLocalPosition(pos);
		object->GetTransform()->SetLocalScale(vec3(0.1f, 0.1f, 0.1f));
		object->SetShader(ResourceManager::GetInstance()->Get<GraphicsShader>(L"deferred.hlsl"));
		object->AddBoxColliderWithModel("playerBullet", ColliderBehave::Active);
		SceneManager::GetInstance()->GetCurrentScene()->ReserveAddGameObject(object,RenderingType::Deferred);
	}

}

void Player::CollisonUpdate()
{

	if (_collisionDected)
	{
		CollisonRotate(_look, _dir, _angle, _rotationAxis);
	}

}


void Player::StartCollisionRotation(vec3 direction,int i)
{

	_look = GetTransform()->GetLook();
	_dir = direction;
	_dir.Normalize();
	_rotationAxis = _look.Cross(_dir);
	//float dotProduct = _look.Dot(_dir);
	_angle = XMConvertToRadians(static_cast<float>(i));
	_collisionDected = true;

}

vec3 Player::CalculateNextDir(vec3 direction,float degree)
{

	vec3 look = GetTransform()->GetLook();

	direction.Normalize();
	look.Normalize();

	vec3 rotationAxis = look.Cross(direction);
	rotationAxis.Normalize();
	float angle = XMConvertToRadians(degree);

	Quaternion rotation = Quaternion::CreateFromAxisAngle(rotationAxis, angle);

	Matrix m = Matrix::CreateFromQuaternion(rotation);

	vec3 result = vec3::TransformNormal(look, m);

	return result;
	
}



void Player::CollisonRotate(vec3 look, vec3 dir, float angle, vec3 rotationAxis)
{

	float framePerDegree = _rotationSpeed * TimeManager::GetInstance()->GetDeltaTime();

	_addAngle += framePerDegree;

	if (_addAngle >=XMConvertToDegrees(angle))
	{
		_addAngle = 0;
		_collisionDected = false;
		return;
	}

	Quaternion rotationQuat = Quaternion::CreateFromAxisAngle(rotationAxis, XMConvertToRadians(framePerDegree));

	vec3 rotate = GetTransform()->GetLocalRotation();

	Quaternion nowQuat = Quaternion::CreateFromYawPitchRoll(vec3(XMConvertToRadians(rotate.x), XMConvertToRadians(rotate.y), XMConvertToRadians(rotate.z)));
	Quaternion result = nowQuat * rotationQuat;

	vec3 resultEuler = result.ToEuler();

	GetTransform()->SetLocalRotation(vec3(XMConvertToDegrees(resultEuler.x), XMConvertToDegrees(resultEuler.y), 0));

}

void Player::AvoidCollision(shared_ptr<BaseCollider>& collider, shared_ptr<BaseCollider>& other)
{
	if (_collisionDected)
		return;

	auto now = GetTransform()->GetLocalPosition();
	auto right = GetTransform()->GetRight();
	auto down = -GetTransform()->GetUp();
	auto up = GetTransform()->GetUp();
	auto left = -right;

	if (collider->GetName() == "raycheck" && other->GetName() == "block")
	{
		collider->Delete(other.get());

		vector<vec3> directions = { right, left ,up,down };

		shuffle(directions.begin(), directions.end(), g);

		for (int i = 30; i <= 180; i += 10)
		{
			for (const auto& dir : directions)
			{
				vec3 result = CalculateNextDir(dir, i);
				Ray rayResult = Ray(now, result);

				if (CollisonManager::GetInstance()->CheckRayCollusion(rayResult, other) == false)
				{
					StartCollisionRotation(dir, i);
					return;
				}
			};
		}
	}

	if (collider->GetName() == "raycheck" && other->GetName() == "earth")
	{
		vector<vec3> directions = { right, left };

		shuffle(directions.begin(), directions.end(), g);

		for (int i = 30; i <= 180; i += 10)
		{
			for (const auto& dir : directions)
			{
				vec3 result = CalculateNextDir(dir, i);
				Ray rayResult = Ray(now, result);

				if (CollisonManager::GetInstance()->CheckRayCollusion(rayResult, other) == false)
				{
					StartCollisionRotation(dir, i);
					return;
				}
			};
		}
	}

}



void Player::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
	if (collider->GetName() == "raycheck" && other->GetName() == "block" || other->GetName() == "earth")
	{
		AvoidCollision(collider, other);
	}

	if (collider->GetName() == "this" &&   other->GetName() == "enemyBullet")
	{
		auto& camera =CameraManager::GetInstance()->GetCamera(CameraType::THIRDVIEW);
		static_pointer_cast<ThirdPersonCamera>(camera)->ShakeOn();
	}
	

}

void Player::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
}