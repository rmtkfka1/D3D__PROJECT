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
#include "BoxCollider.h"
#include "CollisonManager.h"
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
	static float time = 0.0f;
	time+=0.1f * TimeManager::GetInstance()->GetDeltaTime();
	_material->SetFloat(0, time);



}

void CustomObject::Render()
{
	_shader->SetPipelineState();
	_transform->PushData();
	_material->PushData();
	core->GetBufferManager()->GetTable()->SetGraphicsRootDescriptorTable();
	_mesh->Render();

}

void CustomObject::ShaderNoSetRender()
{
	_transform->PushData();
	_material->PushData();
	core->GetBufferManager()->GetTable()->SetGraphicsRootDescriptorTable();
	_mesh->Render();
}


void CustomObject::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
	//[]
}

void CustomObject::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{

}


