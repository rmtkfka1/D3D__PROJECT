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
}

ModelObject::~ModelObject()
{
}

void ModelObject::Init()
{
	_transform = make_shared<Transform>();

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
		list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		list->IASetVertexBuffers(0, 1, &data->meshes->GetVertexView());
		list->IASetIndexBuffer(&data->meshes->GetIndexView());

		_transform->PushData();

		if (data->material)
		{
			data->material->Pushdata();
		}

		core->GetTableHeap()->SetGraphicsRootDescriptorTable(1);

		list->DrawIndexedInstanced(data->meshes->GetIndexCount(), 1, 0, 0, 0);
	}


}


