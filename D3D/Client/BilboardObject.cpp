#include "pch.h"
#include "BilboardObject.h"
#include "Transform.h"
#include "Shader.h"
#include "Mesh.h"
#include "Core.h"
#include "BufferPool.h"
#include "Material.h"
#include <random>

static default_random_engine dre;
static uniform_real_distribution<double> random_xz(-5000.0f, 5000.0f);
static uniform_real_distribution<double> random_y(3000.0f, 5000.0f);
static uniform_real_distribution<double> random_sclae(10.0f, 180.0f);

BilboardObject::BilboardObject()
{
}

BilboardObject::~BilboardObject()
{
}

void BilboardObject::Init()
{
	if (_useWithHeightMap==false)
	{
		float x = random_xz(dre);
		float z = random_xz(dre);
		float y = random_y(dre);
		float scale = random_sclae(dre);

		_transform->SetLocalScale(vec3(scale, scale, scale));
		_transform->SetLocalPosition(vec3(x, y + scale * 15.0f, z));
	}
}

void BilboardObject::Update()
{

	auto CameraPos = CameraManager::GetInstance()->GetActiveCamera()->GetCameraPos();

	// 카메라 방향 벡터 계산 (빌보드 -> 카메라)
	vec3 toCameraDir = CameraPos - _transform->GetLocalPosition();
	toCameraDir.Normalize();

	// y축을 기준으로 빌보드를 회전시켜 항상 카메라를 바라보게 함
	float angle = atan2(toCameraDir.x, toCameraDir.z); 
	_transform->SetLocalRotation(vec3(0, XMConvertToDegrees(angle), 0));
	_transform->Update();
}

void BilboardObject::Render()
{
	_shader->SetPipelineState();
	_transform->PushData();
	_material->Pushdata();
	core->GetTableHeap()->SetGraphicsRootDescriptorTable();
	_mesh->RenderWithoutIndex(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
}
