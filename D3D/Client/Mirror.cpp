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
			i->SetMatrix(_reflectMat);
		}
	}

	_glass = make_shared<CustomObject>();
	_glass->SetFrustumCuling(false);
	_glass->GetMesh() = GeoMetryHelper::LoadRectangleMesh(1.0f);
	_glass->GetTransform()->SetLocalPosition(vec3(51330.0f, 50120.0f, 49850.0f));
	_glass->GetTransform()->SetLocalScale(vec3(248.0f, 320.0f, 250.0f));
	_glass->GetTransform()->SetLocalRotation(vec3(0, 90.0f, 0));

	_glass->GetTransform()->Update();
}

void Mirror::Update()
{
	ModelObject::Update();
}

void Mirror::Render()
{
	auto& list = core->GetCmdList();

	//// 1. ���ٽǹ��ۿ�  �ſ��� ���ٽ� ���� ����ϴ´ܰ� ( ���������� X) => 1���� �����
	_mirrorWriteShader->SetPipelineState();
	list->OMSetStencilRef(1);
	_glass->ShaderNoSetRender();

	//// 2. ���ٽǹ��ۿ� �ݻ�� ������Ʈ ���� ������  ( _mirrorReadShader ���� �ݻ������ ��������)

	_mirrorReadShader->SetPipelineState();
	//������ �ݻ�Ǵ� ������Ʈ���� ����
	for (auto& ele : _mirrorObjects)
	{
		ele->ShaderNoSetRender();
	}


	{
		const float color[4] = { 0.4f,  0.4f,  0.4f,  0.4f };
		list->OMSetBlendFactor(color);

		ModelObject::Render();  //���ڿ�����. // DEPTH TEST - O / DEPTH WRITE - X
	}
	



}

void Mirror::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
}

void Mirror::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
}

