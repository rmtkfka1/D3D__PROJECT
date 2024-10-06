#include "pch.h"
#include "HireacyObject.h"
#include "TransformTree.h"
#include "Core.h"
#include "Model.h"
#include "BufferPool.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
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
