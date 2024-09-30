#include "pch.h"
#include "CustomObject.h"
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "Core.h"
#include "BufferPool.h"

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
	core->GetTableHeap()->SetGraphicsRootDescriptorTable();
	_mesh->Render();

}


