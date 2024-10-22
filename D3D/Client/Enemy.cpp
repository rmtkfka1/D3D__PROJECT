#include "pch.h"
#include "Enemy.h"

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

