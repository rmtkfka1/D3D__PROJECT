#include "pch.h"
#include "HireacyObject.h"
#include "TransformTree.h"
#include "Core.h"
#include "Model.h"
#include "BufferPool.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "BoxCollider.h"
#include "CollisonManager.h"
HireacyObject::HireacyObject():GameObject(GameObjectType::Hierarchy)
{

}

HireacyObject::~HireacyObject()
{
}

void HireacyObject::Init()
{
}

void HireacyObject::Update()
{

	_transform->GetRoot()->Update();

	for (auto& ele : _colliders)
	{
		ele->Update();
	}
}

void HireacyObject::Render()
{
	auto& list = core->GetCmdLIst();

	vector<shared_ptr<ModelMesh>>& meshData = _model->GetMeshes();

	for (auto& data : meshData)
	{
		_transform->findByName(data->name)->PushData();

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

void HireacyObject::AddCollider(ColliderType type, vec3 offsetSize , vec3 offsetCeneter)
{
	if (type == ColliderType::Box)
	{
		shared_ptr<BoxCollider> box = make_shared<BoxCollider>();
		box->SetOwner(shared_from_this());
		box->SetTotalCenter(_model->GetCenter() + offsetCeneter);
		box->SetSize(_model->GetSize() + offsetSize);
		GetTransform()->Update();
		box->MakeBoundingBox();
		CollisonManager::GetInstance()->AddCollider(box);
		_colliders.push_back(box);
	}
}

void HireacyObject::AddBoxCollider(vec3 size, vec3 center)
{
	shared_ptr<BoxCollider> box = make_shared<BoxCollider>();
	box->SetOwner(shared_from_this());
	box->SetTotalCenter(center);
	box->SetSize(size);
	GetTransform()->Update();
	box->MakeBoundingBox();
	CollisonManager::GetInstance()->AddCollider(box);
	_colliders.push_back(box);
}

shared_ptr<Transform> HireacyObject::GetTransform()
{
	return _transform->GetRoot();
}

void HireacyObject::SetModel(shared_ptr<Model> model)
{
	_model = model;
	_transform = make_shared<TransformTree>();
	_transform->MakeTransformTree(model);
}

void HireacyObject::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
	
}

void HireacyObject::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
}
