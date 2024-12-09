#include "pch.h"
#include "Mirror.h"
#include "Shader.h"
#include "Material.h"
#include "Core.h"
#include "Model.h"
#include "Transform.h"
#include "BufferPool.h"
#include "Mesh.h"
#include "HireacyObject.h"
#include "RenderTargets.h"
#include "CustomObject.h"
#include "GeoMetryHelper.h"
void Mirror::Init()
{
	ModelObject::Init();
	_defaultShader = ResourceManager::GetInstance()->Get<GraphicsShader>(L"default.hlsl");

	{
		_mirrorWriteShader = make_shared<GraphicsShader>();
		ShaderInfo info;
		info.shaderType = ShaderType::FORWARD;
		info.depthStencilType = DEPTH_STENCIL_TYPE::STENCILL_WRITE;
		_mirrorWriteShader->Init(L"mirror.hlsl", info);
	}

	{
		_mirrorReadShader = make_shared<GraphicsShader>();
		ShaderInfo info;
		info.shaderType = ShaderType::FORWARD;
		info.depthStencilType = DEPTH_STENCIL_TYPE::STENCILL_READ;
		_mirrorReadShader->Init(L"mirror.hlsl", info);
	}




	_plane = SimpleMath::Plane(vec3(51339.36f, 500014.73f, 49858.14f),
		vec3(-1, 0, 0));

	Matrix _reflectMat = Matrix::CreateReflection(_plane);

	for (auto& ele : _mirrorObjects)
	{

		auto& v = static_pointer_cast<HireacyObject>(ele)->GetMatrial();

		for (auto& i : v)
		{
			i->SetMatrix(1,_reflectMat);
		}


	};
	


}

void Mirror::Update()
{
	ModelObject::Update();
}

void Mirror::Render()
{
	auto& list = core->GetCmdList();

	//// 1. 스텐실버퍼에  거울의 스텐실 값을 기록하는단계 ( 실제렌더링 X) => 1값을 기록함
	_mirrorWriteShader->SetPipelineState();
	list->OMSetStencilRef(1);
	ModelObject::ShaderNoSetRender();

	//// 2. 스텐실버퍼에 반사된 오브젝트 들은 렌더링  ( _mirrorReadShader 에는 반사행렬이 계산되있음)

	_mirrorReadShader->SetPipelineState();
	//씬에서 반사되는 오브젝트만을 렌더
	for (auto& ele : _mirrorObjects)
	{
		ele->ShaderNoSetRender();
	}



	{
		const float mixColor[4] = { 0.3f,  0.3f,  0.3f, 0.1f };
		vector<shared_ptr<ModelMesh>>& meshData = _model->GetMeshes();
		list->OMSetBlendFactor(mixColor);

		for (auto& data : meshData)
		{
			_transform->PushData();

			if (data->material)
			{
				data->material->PushData();
			}

			core->GetBufferManager()->GetTable()->SetGraphicsRootDescriptorTable();

			if (data->name == L"low_glass.001")
			{
				_shader->SetPipelineState();
			}

			else
			{
				_defaultShader->SetPipelineState();
			}

			data->meshes->Render();

		}
	}

	

}

void Mirror::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
}

void Mirror::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
}

