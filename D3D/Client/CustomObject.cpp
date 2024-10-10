#include "pch.h"
#include "CustomObject.h"
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "Core.h"
#include "BufferPool.h"
#include "KeyManager.h"
#include "TimeManager.h"
#include "Shader.h"
CustomObject::CustomObject():GameObject(GameObjectType::Custom)
{
	_mesh = make_shared<Mesh>();
	_transform = make_shared<Transform>();
	_material = make_shared<Material>();
}
CustomObject::CustomObject(PlayerType type):GameObject(GameObjectType::Custom)
{
	_mesh = make_shared<Mesh>();
	_playerType = type;
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
	_shader->SetPipelineState();
	_transform->PushData();
	_material->Pushdata();
	core->GetTableHeap()->SetGraphicsRootDescriptorTable(2);
	_mesh->Render();

}



void CustomObject::AddCollider(string name, ColliderType type, vec3 offsetSize, vec3 offsetCeneter)
{
}

void CustomObject::AddBoxCollider(string name, vec3 size, vec3 center)
{

}

void CustomObject::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{

}

void CustomObject::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{

}


