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
#include "KeyManager.h"
#include "Shader.h"
#include "SphereCollider.h"
HireacyObject::HireacyObject(PlayerType type):GameObject(GameObjectType::Hierarchy)
{
	_playerType = type;
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
	auto& list = core->GetGraphics()->GetCmdLIst();

	vector<shared_ptr<ModelMesh>>& meshData = _model->GetMeshes();

	_shader->SetPipelineState();

	for (auto& data : meshData)
	{
		_transform->findByName(data->name)->PushData();

		if (data->material)
		{
			data->material->PushData();
		}

		core->GetBufferManager()->GetGraphicsTableHeap()->SetGraphicsRootDescriptorTable();

		data->meshes->Render();
	}


}



shared_ptr<Transform> HireacyObject::GetTransform()
{
	return _transform->GetRoot();
}

void HireacyObject::AddBoxColliderWithModel(string name, ColliderBehave behave, vec3 offsetSize, vec3 offsetCeneter)
{
	shared_ptr<BoxCollider> box = make_shared<BoxCollider>();
	box->SetName(name);
	box->SetOwner(shared_from_this());
	box->SetTotalCenter(_model->GetCenter() + offsetCeneter);
	box->SetSize(_model->GetSize() + offsetSize);
	GetTransform()->Update();
	box->MakeBoundingBox();
	CollisonManager::GetInstance()->AddCollider(box, behave);
	_colliders.push_back(box);
}

void HireacyObject::AddSphereColliderWithModel(string name, ColliderBehave behave, float offsetSize, vec3 offsetCeneter)
{
	shared_ptr<SphereCollider> sphere = make_shared<SphereCollider>();
	sphere->SetName(name);
	sphere->SetOwner(shared_from_this());
	sphere->SetTotalCenter(_model->GetCenter() + offsetCeneter);
	sphere->SetRadius(_model->GetRadius() + offsetSize);
	GetTransform()->Update();
	sphere->MakeBoundingSphere();
	CollisonManager::GetInstance()->AddCollider(sphere, behave);
	_colliders.push_back(sphere);
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
