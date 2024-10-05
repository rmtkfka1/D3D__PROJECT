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
	static int i = 0;

	_transformTree->findByName(L"turret_geo")->SetLocalRotation(vec3(0, i++, 0));
	//_transformTree->findByName(L"r_engine_geo")->SetLocalRotation(vec3(0, i++, 0));
	//_transformTree->findByName(L"l_engine_geo")->SetLocalRotation(vec3(0, i++, 0));

	_transformTree->GetRoot()->Update();

}

void HireacyObject::Render()
{
	auto& list = core->GetCmdLIst();

	vector<shared_ptr<ModelMesh>>& meshData = _model->GetMeshes();

	for (auto& data : meshData)
	{
		_transformTree->findByName(data->name)->PushData();

		if (data->material)
		{
			data->material->Pushdata();
		}

		core->GetTableHeap()->SetGraphicsRootDescriptorTable(2);

		data->meshes->Render();
	}
}

void HireacyObject::SetModel(shared_ptr<Model> model)
{
	_model = model;
	_transformTree = make_shared<TransformTree>();
	_transformTree->MakeTransformTree(model);
}
