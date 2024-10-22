#include "pch.h"
#include "PlayerBullet.h"
#include "Transform.h"
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

	_transform->AddMove(_direction);
	super::Update();

}

void PlayerBullet::Render()
{
	super::Render();
}

void PlayerBullet::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
}

void PlayerBullet::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
}

