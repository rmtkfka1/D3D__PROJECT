#include "pch.h"
#include "AnimationObject.h"
#include "Transform.h"
#include "Core.h"
#include "Model.h"
#include "Shader.h"
#include "Material.h"
#include "BufferManager.h"
#include "BufferPool.h"
#include "Mesh.h"
void AnimationObject::Init()
{
	_transform = make_shared<Transform>();
}

void AnimationObject::Update()
{
	_transform->Update();
}

void AnimationObject::Render()
{

	auto& list = core->GetCmdList();

	vector<shared_ptr<ModelMesh>>& meshData = _model->GetMeshes();
	vector<shared_ptr<ModelAnimation>>& animation = _model->GetAnimations();

	_shader->SetPipelineState();






	for (auto& data : meshData)
	{
		_transform->PushData();

		if (data->material)
		{
			data->material->PushData();
		}

		core->GetBufferManager()->GetTable()->SetGraphicsRootDescriptorTable();

		data->meshes->Render();

	}

}

void AnimationObject::ShaderNoSetRender()
{
}

shared_ptr<Transform> AnimationObject::GetTransform()
{
	return shared_ptr<Transform>();
}

void AnimationObject::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
}

void AnimationObject::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
}
