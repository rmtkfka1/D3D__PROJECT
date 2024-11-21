#include "pch.h"
#include "BilboardObject.h"
#include "Transform.h"
#include "Shader.h"
#include "Mesh.h"
#include "Core.h"
#include "BufferPool.h"
#include "Material.h"
#include <random>
#include "Texture.h"
#include "SteamOutputBuffer.h"


static default_random_engine dre;
static default_random_engine dre2;
static uniform_real_distribution<double> random_xz(-5000.0f, 5000.0f);
static uniform_real_distribution<double> random_y(3000.0f, 5000.0f);
static uniform_real_distribution<double> random_sclae(10.0f, 180.0f);
static uniform_int_distribution<int> random_texture(1, 6);
StreamOutputData BilboardObject::_soData;

BilboardObject::BilboardObject()
{
}

BilboardObject::~BilboardObject()
{
}

void BilboardObject::Init()
{
	/*if (_soData.binit == false)
	{
		_soData.SOBuffer = make_shared<StreamOutputBuffer>();
		_soData.SOBuffer->Init(44*6);
		_soData.SOShader = ResourceManager::GetInstance()->Get<Shader>(L"BilboardStreamOutput.hlsl");
		_soData.binit = true;
	}*/

	int randomTexture = random_texture(dre2);

	switch (randomTexture)
	{

	case 1:
	{
		shared_ptr<Texture> texture = ResourceManager::GetInstance()->Load<Texture>(L"BilboardTexture/1.png");
		GetMaterial()->SetDiffuseTexture(texture);
	}
	break;
	case 2:
	{
		shared_ptr<Texture> texture = ResourceManager::GetInstance()->Load<Texture>(L"BilboardTexture/2.png");
		GetMaterial()->SetDiffuseTexture(texture);
	}
	break;

	case 3:
	{
		shared_ptr<Texture> texture = ResourceManager::GetInstance()->Load<Texture>(L"BilboardTexture/3.png");
		GetMaterial()->SetDiffuseTexture(texture);
	}
	break;

	case 4:
	{
		shared_ptr<Texture> texture = ResourceManager::GetInstance()->Load<Texture>(L"BilboardTexture/4.png");
		GetMaterial()->SetDiffuseTexture(texture);
	}
	break;

	case 5:
	{
		shared_ptr<Texture> texture = ResourceManager::GetInstance()->Load<Texture>(L"BilboardTexture/5.png");
		GetMaterial()->SetDiffuseTexture(texture);
	}
	break;

	case 6:
	{
		shared_ptr<Texture> texture = ResourceManager::GetInstance()->Load<Texture>(L"BilboardTexture/6.png");
		GetMaterial()->SetDiffuseTexture(texture);
	}
	break;
	default:
		assert(false);
		break;
	}


	if (_useWithHeightMap==false)
	{
		float x = random_xz(dre);
		float z = random_xz(dre);
		float y = random_y(dre);
		float scale = random_sclae(dre);

		_transform->SetLocalScale(vec3(scale, scale, scale));
		_transform->SetLocalPosition(vec3(x, y + scale * 14.0f, z));
		AddBoxCollider("bilboard", ColliderBehave::Passive, vec3(2.0f, 4.0f, 2.0f), vec3(0, 0, 0));
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

	/*if (_soData._bStreamRender == false)
	{
		_soData.SOShader->SetPipelineState();
		_soData.SOBuffer->Bind();
		_mesh->RenderWithoutIndex(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		_soData.SOBuffer->UnBind();
		_soData._bStreamRender = true;

	}*/

	//else
	//{
		_shader->SetPipelineState();
		_transform->PushData();
		_material->PushData();
		core->GetBufferManager()->GetGraphicsTableHeap()->SetGraphicsRootDescriptorTable();
		//_soData.SOBuffer->Render();
		_mesh->RenderWithoutIndex(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	//}

	

}

