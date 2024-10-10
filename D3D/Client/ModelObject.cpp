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
#include "Shader.h"
#include "SphereCollider.h"
ModelObject::ModelObject(PlayerType type):GameObject(GameObjectType::Model)
{
	_playerType = type;
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

	_shader->SetPipelineState();

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

	else
	{
		shared_ptr<SphereCollider> sphere = make_shared<SphereCollider>();
		sphere->SetName(name);
		sphere->SetOwner(shared_from_this());
		sphere->SetTotalCenter(_model->GetCenter());
		sphere->SetRadius(_model->GetRadius());
		GetTransform()->Update();
		sphere->MakeBoundingSphere();
		CollisonManager::GetInstance()->AddCollider(sphere);
		_colliders.push_back(sphere);
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


