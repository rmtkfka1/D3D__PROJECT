#include "pch.h"
#include "Stage1.h"
#include "Shader.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "CustomObject.h"
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
		shared_ptr<CustomObject> gameobject = make_shared<CustomObject>();
	

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

		auto& meshptr = gameobject->GetMesh();
		meshptr->Init(vec, index);

		auto materialptr =gameobject->GetMaterial();

		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Init(L"..\\Resources\\Texture\\box.jpg");
		materialptr->SetDiffuseTexture(texture);

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->Init(L"..\\Resources\\Shader\\default.hlsl");
		materialptr->SetShader(shader);
	

		AddGameObject(gameobject);
	}
}
