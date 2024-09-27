#include "pch.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "BufferPool.h"
#include "Core.h"
#include "Transform.h"
void MeshRenderer::Update()
{
	_transform->Update();
}

void MeshRenderer::Render()
{
	_transform->PushData();
	_material->Pushdata();
	core->GetTableHeap()->SetGraphicsRootDescriptorTable();
	_mesh->Render();
}
