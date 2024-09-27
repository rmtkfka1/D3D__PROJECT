#include "pch.h"
#include "Stage1.h"
#include "Shader.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"

Stage1::Stage1()
{
}

Stage1::~Stage1()
{
}

void Stage1::Init()
{
	Scene::Init();
	BulidObject();
}

void Stage1::Run()
{
	Scene::Run();
}

void Stage1::LateUpdate()
{
	Scene::LateUpdate();
}

void Stage1::BulidObject()
{
	{
		shared_ptr<GameObject> gameobject = make_shared<GameObject>();
		shared_ptr<Mesh> mesh = make_shared<Mesh>();
		shared_ptr<Shader> shader = make_shared<Shader>();
		shared_ptr<Texture> texture = make_shared<Texture>();

		vector<Vertex> vec(3);
		vec[0].pos = vec3(0.f, 0.5f, 0.5f); //À­²À
		vec[0].color = vec4(1.f, 0.f, 0.f, 1.f);
		vec[0].uv = vec2(0, 0);

		vec[1].pos = vec3(0.5f, -0.5f, 0.5f); //¿À¸¥ÂÊ²À
		vec[1].color = vec4(0.f, 1.0f, 0.f, 1.f);
		vec[1].uv = vec2(1, 1);

		vec[2].pos = vec3(-0.5f, -0.5f, 0.5f); //¿Þ²À
		vec[2].color = vec4(0.f, 0.f, 1.f, 1.f);
		vec[2].uv = vec2(0.5, 0.5);

		vector<uint32> index(3);

		index[0] = 0;
		index[1] = 1;
		index[2] = 2;

		mesh->Init(vec, index);
		shader->Init(L"..\\Resources\\Shader\\default.hlsl");
		texture->Init(L"..\\Resources\\Texture\\box.jpg");

		/////////////////////////
		shared_ptr<Material> material = make_shared<Material>();
		material->SetDiffuseTexture(texture);
		material->SetShader(shader);

		shared_ptr<Transform> transform = make_shared<Transform>();
		shared_ptr<MeshRenderer> renderer = make_shared<MeshRenderer>();
		renderer->SetMaterial(material);
		renderer->SetTransform(transform);
		renderer->SetMesh(mesh);

		gameobject->Push(renderer);

		AddGameObject(gameobject);
	}
}
