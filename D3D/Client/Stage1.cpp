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
#include "CameraManager.h"
#include "LightManager.h"
#include "HireacyObject.h"
#include "TransformTree.h"
Stage1::Stage1()
{
}

Stage1::~Stage1()
{
}

void Stage1::Init()
{
	BulidCamera();
	BulidObject();
	BulidLight();
	Scene::Init();
}

void Stage1::Run()
{

	CameraManager::GetInstance()->PushData();
	LightManager::GetInstnace()->SetData();
	Scene::Run();
}

void Stage1::LateUpdate()
{
	Scene::LateUpdate();
}

void Stage1::BulidLight()
{
	{
	Light light;
	light.direction = CameraManager::GetInstance()->GetActiveCamera()->GetCameraLook();
	light.fallOffStart = 0.0f;
	light.position = _player->GetTransform()->GetWorldPosition();
	light.fallOffEnd = 3000.0f;
	light.spotPower = 100.0f;
	light.material.ambient = vec3(0.0f, 0, 0);
	light.material.diffuse = vec3(1.0f, 1.0f, 1.0f);
	light.material.specular = vec3(1.0f, 1.0f, 1.0f);
	light.material.shininess = 15.0f;
	light.material.lightType = static_cast<int32>(LIGHT_TYPE::SPOT_LIGHT);
	light.strength = vec3(8.0f, 8.0f, 8.0f);
	LightManager::GetInstnace()->PushLight(light);
	LightManager::GetInstnace()->SetPlayer(_player);
	}

	{
		Light light;

		light.direction = vec3(0, -1.0f, 0);
		light.material.ambient = vec3(0, 0, 0);
		light.material.diffuse = vec3(1.0f, 1.0f, 1.0f);
		light.material.specular = vec3(1.0f, 1.0f, 1.0f);
		light.material.shininess = 64.0f;
		light.material.lightType = static_cast<int32>(LIGHT_TYPE::DIRECTIONAL_LIGHT);
		light.strength = vec3(1.0f, 1.0f, 1.0f);
		LightManager::GetInstnace()->PushLight(light);
	}

};

void Stage1::BulidCamera()
{
	shared_ptr<ThirdPersonCamera> thirdCamera = make_shared<ThirdPersonCamera>();
	CameraManager::GetInstance()->AddCamera(CameraType::THIRDVIEW, thirdCamera);
	CameraManager::GetInstance()->SetActiveCamera(CameraType::THIRDVIEW);
}

void Stage1::BulidObject()
{
	




	{

		shared_ptr<Player> gameobject = make_shared<Player>();

		_player = gameobject;

		gameobject->SetThirdPersonCamera(static_pointer_cast<ThirdPersonCamera>(CameraManager::GetInstance()->GetCamera(CameraType::THIRDVIEW)));

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

	/*	shared_ptr<Terrain> terrain = make_shared<Terrain>();
		AddGameObject(terrain);
		gameobject->SetTerrain(terrain);*/
	
	}

	{
		shared_ptr<Model> data = Model::ReadData(L"Tank/Tank");

		shared_ptr<HireacyObject> object = make_shared<HireacyObject>();
		object->SetModel(data);
		object->GetTransform()->GetRoot()->SetLocalScale(vec3(5.0f, 5.0f, 5.0f));
		object->GetTransform()->GetRoot()->SetLocalPosition(vec3(100.0f, 0, 0));

		AddGameObject(object);

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
