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
#include "Terrain.h"
#include "ResourceManager.h"
#include "ModelObject.h"
#include "Model.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "HireacyObject.h"
#include "TransformTree.h"
#include "Player.h"
#include "Core.h"
#include "BoxCollider.h"

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
	

	//WCHAR wchTxt[64];
	//swprintf_s(wchTxt, 64, L"look.x: %.2f, look.y: %.2f, look.z: %.2f",
	//	_player->GetTransform()->GetRoot()->GetLook().x,
	//	_player->GetTransform()->GetRoot()->GetLook().y,
	//	_player->GetTransform()->GetRoot()->GetLook().z);

	//SetWindowText(core->GetWindowHandle(), wchTxt);
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
	light.fallOffEnd = 5000.0f;
	light.spotPower = 200.0f;
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
		shared_ptr<Model> data = Model::ReadData(L"helicopter/helicopter");

		shared_ptr<Player> player = make_shared<Player>();
		_player = player;
		player->SetModel(data);
		player->GetTransform()->SetLocalScale(vec3(5.0f, 5.0f, 5.0f));
		player->GetTransform()->SetLocalPosition(vec3(100.0f, 0, 0));

		player->AddCollider(ColliderType::Box,vec3(-2.5f,-1.0f,0));
	
		player->SetThirdPersonCamera(static_pointer_cast<ThirdPersonCamera>(CameraManager::GetInstance()->GetCamera(CameraType::THIRDVIEW)));

		//shared_ptr<Terrain> terrain = make_shared<Terrain>();
		//AddGameObject(terrain);
		//player->SetTerrain(terrain);

		AddGameObject(player);

	}

	{

		shared_ptr<HireacyObject> object = make_shared<HireacyObject>();
		shared_ptr<Model> data = Model::ReadData(L"Tank/Tank");
		object->SetModel(data);
		object->AddCollider(ColliderType::Box);

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
