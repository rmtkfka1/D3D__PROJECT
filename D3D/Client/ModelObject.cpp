#include "pch.h"
#include "ModelObject.h"
#include "Model.h"
#include "Core.h"
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "BufferPool.h"
#include "BoxCollider.h"
#include "CollisonManager.h"

ModelObject::ModelObject():GameObject(GameObjectType::Model)
{
	_transform = make_shared<Transform>();
}

ModelObject::~ModelObject()
{
}

void ModelObject::Init()
{
	

}

void ModelObject::Update()
{
	_transform->Update();


	for (auto& ele : _colliders)
	{
		ele->Update();
	}
}

void ModelObject::Render()
{
	auto& list = core->GetCmdLIst();

	vector<shared_ptr<ModelMesh>>& meshData = _model->GetMeshes();

	for (auto& data : meshData)
	{
		_transform->PushData();

		if (data->material)
		{
			data->material->Pushdata();
		}

		core->GetTableHeap()->SetGraphicsRootDescriptorTable(2);

		data->meshes->Render();

	}


	for (auto& ele : _colliders)
	{
		ele->Render();
	}
}

void ModelObject::AddCollider(string name, ColliderType type, vec3 offsetSize, vec3 offsetCeneter)
{
	if (type == ColliderType::Box)
	{
		shared_ptr<BoxCollider> box = make_shared<BoxCollider>();
		box->SetName(name);
		box->SetOwner(shared_from_this());
		box->SetTotalCenter(_model->GetCenter() + offsetCeneter);
		box->SetSize(_model->GetSize() + offsetSize);
		GetTransform()->Update();
		box->MakeBoundingBox();
		CollisonManager::GetInstance()->AddCollider(box);
		_colliders.push_back(box);
	}
}

void ModelObject::AddBoxCollider(string name, vec3 size, vec3 center)
{
	shared_ptr<BoxCollider> box = make_shared<BoxCollider>();
	box->SetName(name);
	box->SetOwner(shared_from_this());
	box->SetTotalCenter(center);
	box->SetSize(size);
	GetTransform()->Update();
	box->MakeBoundingBox();
	CollisonManager::GetInstance()->AddCollider(box);
	_colliders.push_back(box);
}

void ModelObject::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
}

void ModelObject::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
}


