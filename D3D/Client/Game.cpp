#include "pch.h"
#include "Game.h"
#include "Core.h"
#include "Mesh.h"
#include "Shader.h"
#include "BufferPool.h"
#include "Texture.h"
shared_ptr<Mesh> mesh = make_shared<Mesh>();
shared_ptr<Shader> shader = make_shared<Shader>();
Temp t;
shared_ptr<Texture> texture = make_shared<Texture>();

void Game::Init(HWND hnwd)
{
	//다이렉트 코어 초기화
	core->Init(hnwd, true, true);

	vector<Vertex> vec(3);
	vec[0].pos = vec3(0.f, 0.5f, 0.5f); //윗꼭
	vec[0].color = vec4(1.f, 0.f, 0.f, 1.f);
	vec[0].uv = vec2(0, 0);

	vec[1].pos = vec3(0.5f, -0.5f, 0.5f); //오른쪽꼭
	vec[1].color = vec4(0.f, 1.0f, 0.f, 1.f);
	vec[1].uv = vec2(1, 1);

	vec[2].pos = vec3(-0.5f, -0.5f, 0.5f); //왼꼭
	vec[2].color = vec4(0.f, 0.f, 1.f, 1.f);
	vec[2].uv = vec2(0.5, 0.5);

	vector<uint32> index(3);

	index[0] = 0;
	index[1] = 1;
	index[2] = 2;

	mesh->Init(vec, index);

	shader->Init(L"..\\Resources\\Shader\\default.hlsl");
	t.offset = vec4(0.3f, 0, 0,0);

	texture->Init(L"..\\Resources\\Texture\\box.jpg");



}

void Game::Run()
{
	core->RenderBegin();

	shader->Update();

	core->GetConstantBufferPool()->PushData(&t, sizeof(t));
	core->GetTableHeap()->CopySRV(texture->GetCpuHandle(), SRV_REGISTER::t0);
	core->GetTableHeap()->SetGraphicsRootDescriptorTable();

	mesh->Render();

	core->RenderEnd();
}
