#include "pch.h"
#include "EnemyBullet.h"
#include "Transform.h"
#include "TimeManager.h"
EnemyBullet::EnemyBullet() :ModelObject(PlayerType::EnemyBullet)
{

}

EnemyBullet::~EnemyBullet()
{
}

void EnemyBullet::Init()
{
	super::Init();
}

void EnemyBullet::Update()
{

	_transform->AddMove(_direction * 1500.0f * TimeManager::GetInstance()->GetDeltaTime());
	super::Update();
	Delete();

}

void EnemyBullet::Render()
{
	super::Render();
}

void EnemyBullet::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
	if (other->GetName() == "this")
	{
		
	}
}

void EnemyBullet::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
}

void EnemyBullet::Delete()
{
	
	_sumTime += TimeManager::GetInstance()->GetDeltaTime();

	if (_sumTime >= 5.0f)
	{
		CollisonManager::GetInstance()->ReserveDeleteCollider(this->GetCollider());
		SceneManager::GetInstance()->GetCurrentScene()->ReserveDeleteGameObject(shared_from_this(), RenderingType::Deferred);
	}

}

