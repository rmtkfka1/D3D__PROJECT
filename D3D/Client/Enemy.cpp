#include "pch.h"
#include "Enemy.h"
#include "TimeManager.h"
#include "Transform.h"
#include "TransformTree.h"
#include "Player.h"

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
	Animation();
	LookatPlayer();

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

void Enemy::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{

}

void Enemy::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{

}

