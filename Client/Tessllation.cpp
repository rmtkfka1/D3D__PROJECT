#include "pch.h"
#include "Tessllation.h"
#include "Shader.h"
#include "Transform.h"
#include "Material.h"
#include "Core.h"
#include "BufferManager.h"
#include "BufferPool.h"
#include "Mesh.h"


Tessllation::Tessllation()
{
}

Tessllation::~Tessllation()
{
}

void Tessllation::Init()
{
}

void Tessllation::Update()
{
	CustomObject::Update();
}

void Tessllation::Render()
{
	_shader->SetPipelineState();
	_transform->PushData();
	_material->PushData();
	core->GetBufferManager()->GetTable()->SetGraphicsRootDescriptorTable();
	_mesh->RenderWithoutIndex(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
}
