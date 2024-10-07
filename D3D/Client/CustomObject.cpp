#include "pch.h"
#include "CustomObject.h"
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "Core.h"
#include "BufferPool.h"
#include "KeyManager.h"
#include "TimeManager.h"
CustomObject::CustomObject():GameObject(GameObjectType::Custom)
{
	_mesh = make_shared<Mesh>();
	_transform = make_shared<Transform>();
	_material = make_shared<Material>();
}

CustomObject::~CustomObject()
{

}

void CustomObject::Init()
{

}

void CustomObject::Update()
{

	_transform->Update();

}

void CustomObject::Render()
{

	_transform->PushData();
	_material->Pushdata();
	core->GetTableHeap()->SetGraphicsRootDescriptorTable(2);
	_mesh->Render();

}

void CustomObject::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{

}

void CustomObject::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{

}


