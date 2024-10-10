#include "pch.h"
#include "BaseCollider.h"

 bool BaseCollider::brender=false;

BaseCollider::BaseCollider(ColliderType colliderType) :_colliderType(colliderType)
{

}

BaseCollider::~BaseCollider()
{

}

void BaseCollider::Clear()
{
	_collisionMap.clear();
}

void BaseCollider::Delete(BaseCollider* del)
{
	auto it = _collisionMap.find(del);
	if (it != _collisionMap.end())
	{
		_collisionMap.erase(it);
	}

}

