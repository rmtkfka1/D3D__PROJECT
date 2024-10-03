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
#include "Player.h"
#include "Terrain.h"
#include "ResourceManager.h"
#include "ModelObject.h"
#include "Model.h"
#include "Camera.h"

Stage1::Stage1()
{
}

Stage1::~Stage1()
{
}

void Stage1::Init()
{
	_thirdCamera = make_shared<ThirdPersonCamera>();
	
	BulidObject();
	Scene::Init();
}

void Stage1::Run()
{
	_thirdCamera->PushData();

	Scene::Run();
}

void Stage1::LateUpdate()
{
	Scene::LateUpdate();
}

void Stage1::BulidObject()
{
	

	{
		shared_ptr<Model> data = Model::ReadData(L"Tank/Tank");

		shared_ptr<ModelObject> object = make_shared<ModelObject>();
		object->SetModel(data);

		AddGameObject(object);

	}


	{

		shared_ptr<Player> gameobject = make_shared<Player>();

		gameobject->SetThirdPersonCamera(_thirdCamera);

		gameobject->GetTransform()->SetLocalPosition(vec3(0, 0, 0));

		auto& meshptr = gameobject->GetMesh();
		meshptr = GeoMetryHelper::LoadRectangleBox(0.5f);

		auto materialptr = gameobject->GetMaterial();

		shared_ptr<Texture> texture = ResourceManager::GetInstance()->Load<Texture>(L"1.jpg");
		materialptr->SetDiffuseTexture(texture);

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->Init(L"default.hlsl");
		materialptr->SetShader(shader);
		AddGameObject(gameobject);

		//shared_ptr<Terrain> terrain = make_shared<Terrain>();
		//AddGameObject(terrain);
		//gameobject->SetTerrain(terrain);
	
	}



	{

		shared_ptr<CustomObject> gameobject = make_shared<CustomObject>();
		gameobject->GetMesh() = GeoMetryHelper::LoadRectangleBox(10.0f);
		
		shared_ptr<Texture> texture = ResourceManager::GetInstance()->Load<Texture>(L"cubemap/skybox.dds", TextureType::CubeMap);


		ShaderInfo info;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS_EQUAL;
		shared_ptr<Shader> shader = ResourceManager::GetInstance()->Load<Shader>(L"sky.hlsl", info);
	
		gameobject->GetMaterial()->SetShader(shader);
		gameobject->GetMaterial()->SetDiffuseTexture(texture);


		AddGameObject(gameobject);
	}

	
}
