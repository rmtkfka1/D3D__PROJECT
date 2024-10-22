#include "pch.h"
#include "PlayerBullet.h"
#include "Transform.h"
#include "TimeManager.h"
PlayerBullet::PlayerBullet():ModelObject(PlayerType::PlayerBullet)
{

}

PlayerBullet::~PlayerBullet()
{
}

void PlayerBullet::Init()
{
	super::Init();
}

void PlayerBullet::Update()
{

	_transform->AddMove(_direction * 1500.0f * TimeManager::GetInstance()->GetDeltaTime());
	super::Update();

}

void PlayerBullet::Render()
{
	super::Render();
}

void PlayerBullet::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
	if (other->GetName() == "block")
	{
		
	}
}

void PlayerBullet::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
}

