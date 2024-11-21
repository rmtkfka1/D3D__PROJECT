#include "pch.h"
#include "GameObject.h"
#include "Transform.h"
#include "BoxCollider.h"
#include "SphereCollider.h"

GameObject::GameObject(GameObjectType type):_type(type)
{

}
GameObject::~GameObject()
{

}



void GameObject::AddBoxCollider(string name, ColliderBehave behave, vec3 size, vec3 center)
{
	shared_ptr<BoxCollider> box = make_shared<BoxCollider>();
	box->SetName(name);
	box->SetOwner(shared_from_this());
	box->SetTotalCenter(center);
	box->SetSize(size);
	GetTransform()->Update();
	box->MakeBoundingBox();
	CollisonManager::GetInstance()->AddCollider(box,behave);
	_colliders.push_back(box);
}

void GameObject::AddSphereCollider(string name, ColliderBehave behave, float radius, vec3 center)
{
	shared_ptr<SphereCollider> sphere = make_shared<SphereCollider>();
	sphere->SetName(name);
	sphere->SetOwner(shared_from_this());
	sphere->SetTotalCenter(center);
	sphere->SetRadius(radius);
	GetTransform()->Update();
	sphere->MakeBoundingSphere();
	CollisonManager::GetInstance()->AddCollider(sphere, behave);
	_colliders.push_back(sphere);
}

void GameObject::BoundingRender()
{
	for (auto& ele : _colliders)
	{
		ele->Render();
	}
}

shared_ptr<BaseCollider>& GameObject::GetCollider()
{
	return _colliders[0];
}
