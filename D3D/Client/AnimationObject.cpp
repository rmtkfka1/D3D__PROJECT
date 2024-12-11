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
#include "Animator.h"
AnimationObject::AnimationObject():GameObject(GameObjectType::ANI)
{
	_transform = make_shared<Transform>();
}
AnimationObject::~AnimationObject()
{
}
void AnimationObject::Init()
{
	_animator = make_shared<Animator>();
	_animator->SetModel(_model);
	_animator->Init();

	
}

void AnimationObject::Update()
{
	_transform->Update();
}

void AnimationObject::Render()
{

	auto& list = core->GetCmdList();

	vector<shared_ptr<ModelMesh>>& meshData = _model->GetMeshes();

	_shader->SetPipelineState();
	_animator->PushData();

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
	return _transform;
}


void AnimationObject::SetModel(shared_ptr<Model> model)
{
	_model = model;
}

void AnimationObject::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
}

void AnimationObject::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
}
