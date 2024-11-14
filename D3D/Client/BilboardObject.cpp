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

	float x = random_xz(dre);
	float z = random_xz(dre);
	float y = random_y(dre);
	float scale = random_sclae(dre);

	_transform->SetLocalScale(vec3(scale, scale, scale));
	_transform->SetLocalPosition(vec3(x, y+scale*15.0f, z));
}

void BilboardObject::Update()
{
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
