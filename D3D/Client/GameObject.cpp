#include "pch.h"
#include "GameObject.h"
#include "Transform.h"
GameObject::GameObject(GameObjectType type):_type(type)
{

}
GameObject::~GameObject()
{

}

shared_ptr<BaseCollider>& GameObject::GetCollider()
{
	return _colliders[0];
}
