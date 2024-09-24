#include "pch.h"
#include "Game.h"
#include "Core.h"
#include "Mesh.h"
#include "Shader.h"

shared_ptr<Mesh> mesh = make_shared<Mesh>();
shared_ptr<Shader> shader = make_shared<Shader>();

void Game::Init(HWND hnwd)
{
	core->Init(hnwd, true, true);

	vector<Vertex> vec(3);
	vec[0].pos = vec3(0.f, 0.5f, 0.5f);
	vec[0].color = vec4(1.f, 0.f, 0.f, 1.f);
	vec[1].pos = vec3(0.5f, -0.5f, 0.5f);
	vec[1].color = vec4(0.f, 1.0f, 0.f, 1.f);
	vec[2].pos = vec3(-0.5f, -0.5f, 0.5f);
	vec[2].color = vec4(0.f, 0.f, 1.f, 1.f);
	mesh->Init(vec);

	shader->Init(L"..\\Resources\\Shader\\default.hlsl");

	core->WaitSync();
}

void Game::Run()
{
	core->RenderBegin();

	shader->Update();
	mesh->Render();

	core->RenderEnd();
}
