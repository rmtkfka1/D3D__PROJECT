#include "pch.h"
#include "Enemy.h"
#include "TimeManager.h"
#include "Transform.h"
#include "TransformTree.h"
#include "Player.h"
#include "EnemyBullet.h"
#include "Model.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include <random>
std::random_device rrd;
std::mt19937 gg(rrd());

Enemy::Enemy():HireacyObject(PlayerType::Enemy)
{


}

Enemy::~Enemy()
{


}

void Enemy::Init()
{
	Super::Init();
}

void Enemy::Update()
{

	GetShoted();
	Animation();

	if (_blocked == false)
	{
		LookatPlayer();
		Move();
		Shot();
	}

	Super::Update();
}

void Enemy::Render()
{
	Super::Render();
}

void Enemy::Animation()
{
	float dt = TimeManager::GetInstance()->GetDeltaTime();
	float rotation = 720.0f;

	float result = rotation * dt; //1초에 2바퀴 회전하도록

	_transform->findByName(L"Top_Rotor")->AddRotate(vec3(0, result, 0));

	static float rotateSpeed = 180.0f;

	if (_blocked == true)
	{
		GetTransform()->AddRotate(vec3(0, rotateSpeed * dt, 0));
	}


}

void Enemy::LookatPlayer()
{
    if (_player == nullptr)
        return;

    vec3 enemyPos = GetTransform()->GetLocalPosition();  // Enemy's position
    vec3 playerPos = _player->GetTransform()->GetLocalPosition(); // Player's position

    vec3 direction = playerPos - enemyPos;
    direction.Normalize();

    float yaw = atan2(direction.x, direction.z);

    float distance = sqrt(direction.x * direction.x + direction.z * direction.z); 
    float pitch = atan2(direction.y, distance); 

    vec3 now = GetTransform()->GetLocalRotation();
    GetTransform()->SetLocalRotation(vec3(XMConvertToDegrees(pitch), XMConvertToDegrees(yaw) + 180.0f, now.z));


}

void Enemy::Shot()
{
    static float sum = 0;

    sum += TimeManager::GetInstance()->GetDeltaTime();

    if (sum >= 3.0f)
    {
        sum = 0;
		shared_ptr<EnemyBullet> object = make_shared<EnemyBullet>();
		shared_ptr<Model> data = Model::ReadData(L"playerBullet/playerBullet", L"playerBullet");
		object->SetModel(data);
		object->_direction = GetTransform()->GetLook();
		auto& pos = this->GetTransform()->GetLocalPosition() + GetTransform()->GetLook() * 100.0f;
		object->GetTransform()->SetLocalPosition(pos);
		object->GetTransform()->SetLocalScale(vec3(0.1f, 0.1f, 0.1f));
		object->SetShader(ResourceManager::GetInstance()->Get<GraphicsShader>(L"deferred.hlsl"));
		object->AddBoxColliderWithModel("enemyBullet",ColliderBehave::Active);
		SceneManager::GetInstance()->GetCurrentScene()->ReserveAddGameObject(object, RenderingType::Deferred);
    }

}

void Enemy::Move()
{
	float dt = TimeManager::GetInstance()->GetDeltaTime();
	auto& look = GetTransform()->GetLook();
	GetTransform()->AddMove(_speed*look * dt);
}

void Enemy::GetShoted()
{
	if (_blocked == false)
		return;

	static float sumTime = 0;

	sumTime += TimeManager::GetInstance()->GetDeltaTime();

	if (sumTime > 10.0f)
	{
		sumTime = 0;
		_blocked = false;
	}


}


void Enemy::CollisonUpdate()
{

	if (_collisionDected)
	{
		CollisonRotate(_look, _dir, _angle, _rotationAxis);
	}

}


void Enemy::StartCollisionRotation(vec3 direction, int i)
{

	_look = GetTransform()->GetLook();
	_dir = direction;
	_dir.Normalize();
	_rotationAxis = _look.Cross(_dir);
	//float dotProduct = _look.Dot(_dir);
	_angle = XMConvertToRadians(static_cast<float>(i));
	_collisionDected = true;

}

vec3 Enemy::CalculateNextDir(vec3 direction, float degree)
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



void Enemy::CollisonRotate(vec3 look, vec3 dir, float angle, vec3 rotationAxis)
{

	float framePerDegree = _rotationSpeed * TimeManager::GetInstance()->GetDeltaTime();

	_addAngle += framePerDegree;

	if (_addAngle >= XMConvertToDegrees(angle))
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

void Enemy::AvoidCollision(shared_ptr<BaseCollider>& collider, shared_ptr<BaseCollider>& other)
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

		shuffle(directions.begin(), directions.end(), gg);

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

		shuffle(directions.begin(), directions.end(), gg);

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



void Enemy::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
	if (other->GetName() == "playerBullet")
	{
		
		_blocked = true;
		/*for (auto& ele : _colliders)
		{
			CollisonManager::GetInstance()->ReserveDeleteCollider(ele);
		}
		SceneManager::GetInstance()->GetCurrentScene()->ReserveDeleteGameObject(shared_from_this(), RenderingType::Deferred);*/
	}

	/*if (collider->GetName() == "raycheck" && other->GetName() == "block" || other->GetName() == "earth")
	{
		AvoidCollision(collider, other);
	}*/


}

void Enemy::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{

}

