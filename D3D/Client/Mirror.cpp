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

void Mirror::Init()
{
	ModelObject::Init();

	_mirrorShader = make_shared<GraphicsShader>();

	ShaderInfo info;
	info.shaderType = ShaderType::DEFREED;

	_mirrorShader->Init(L"mirror.hlsl",info);

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

}

void Mirror::Update()
{
	ModelObject::Update();
}

void Mirror::Render()
{
	//물체의 형태를 그대로 그리는것.
	ModelObject::Render();

	MirrorRender();

}

void Mirror::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
}

void Mirror::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{
}

void Mirror::MirrorRender()
{

	_mirrorShader->SetPipelineState();

	for (auto& ele : _mirrorObjects)
	{
		ele->ShaderNoSetRender();
	}
	
}
