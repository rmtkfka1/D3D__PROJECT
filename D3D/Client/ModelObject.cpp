#include "pch.h"
#include "ModelObject.h"
#include "Model.h"
#include "Core.h"
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "BufferPool.h"
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


}

void ModelObject::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
}

void ModelObject::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
}


