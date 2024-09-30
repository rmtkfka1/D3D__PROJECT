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
#include "CameraManager.h"
#include "Player.h"
#include "Terrain.h"

Stage1::Stage1()
{
}

Stage1::~Stage1()
{
}

void Stage1::Init()
{
	BulidObject();
	Scene::Init();
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

		
		shared_ptr<Terrain> terrain = make_shared<Terrain>();
		AddGameObject(terrain);
		CameraManager::GetInstance()->SetTerrain(terrain);
		
		shared_ptr<Player> gameobject = make_shared<Player>();

		gameobject->GetTransform()->SetLocalPosition(vec3(0, 0, 0));

		auto& meshptr = gameobject->GetMesh();
		meshptr = GeoMetryHelper::LoadRectangleBox(0.5f);

		auto materialptr = gameobject->GetMaterial();

		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->Init(L"1.jpg");
		materialptr->SetDiffuseTexture(texture);

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->Init(L"default.hlsl");
		materialptr->SetShader(shader);

		CameraManager::GetInstance()->SetPlayer(gameobject);
		gameobject->SetTerrain(terrain);
		AddGameObject(gameobject);


	}



	{

		shared_ptr<CustomObject> gameobject = make_shared<CustomObject>();
		gameobject->GetMesh() = GeoMetryHelper::LoadRectangleBox(10.0f);

		shared_ptr<Texture> texture = make_shared<Texture>();
		texture->InitCubeMap(L"cubemap/DGarden_specularIBL.dds");

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
