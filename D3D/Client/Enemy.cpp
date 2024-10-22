#include "pch.h"
#include "Enemy.h"
#include "TimeManager.h"
#include "Transform.h"
#include "TransformTree.h"

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
	float dt = TimeManager::GetInstance()->GetDeltaTime();
	float rotation = 720.0f;

	float result = rotation * dt; //1초에 2바퀴 회전하도록

	_transform->findByName(L"Top_Rotor")->AddRotate(vec3(0, result, 0));


	Super::Update();
}

void Enemy::Render()
{
	Super::Render();
}

void Enemy::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{

}

void Enemy::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{

}

