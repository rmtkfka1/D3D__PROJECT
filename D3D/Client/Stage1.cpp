#include "pch.h"
#include "Stage1.h"
#include "Shader.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "CustomObject.h"
#include "Transform.h"
#include "GeoMetryHelper.h"

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


		auto& meshptr = gameobject->GetMesh();
		meshptr = GeoMetryHelper::LoadRectangleBox();

		auto materialptr =gameobject->GetMaterial();

		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Init(L"..\\Resources\\Texture\\box.jpg");
		materialptr->SetDiffuseTexture(texture);

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->Init(L"default.hlsl");
		materialptr->SetShader(shader);
	

		AddGameObject(gameobject);
	}

	{

		shared_ptr<CustomObject> gameobject = make_shared<CustomObject>();
		gameobject->GetMesh() = GeoMetryHelper::LoadRectangleBox(10.0f);

		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->InitCubeMap(L"../Resources/Texture/cubemap/DGarden_specularIBL.dds");

		shared_ptr<Shader> shader = make_shared<Shader>();
		ShaderInfo info;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS_EQUAL;
		shader->Init(L"sky.hlsl", info);

		gameobject->GetMaterial()->SetShader(shader);
		gameobject->GetMaterial()->SetDiffuseTexture(texture);


		AddGameObject(gameobject);
	}
}
