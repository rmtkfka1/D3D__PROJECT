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
#include "KeyManager.h"
#include "Box.h"
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

	if (KeyManager::GetInstance()->GetButtonDown(KEY_TYPE::ONE))
	{
		CameraManager::GetInstance()->SetActiveCamera(CameraType::OBSERVE);
	}

	if (KeyManager::GetInstance()->GetButtonDown(KEY_TYPE::THREE))
	{
		CameraManager::GetInstance()->SetActiveCamera(CameraType::THIRDVIEW);
	}

	CameraManager::GetInstance()->PushData();
	LightManager::GetInstnace()->SetData();
	Scene::Run();
	

	WCHAR wchTxt[100];
	swprintf_s(wchTxt, 100, L"look.x: %.2f, look.y: %.2f, look.z: %.2f, pos.x: % .2f, pos.y : % .2f, pos.z : % .2f",
		_player->GetTransform()->GetLook().x,
		_player->GetTransform()->GetLook().y,
		_player->GetTransform()->GetLook().z,

		_player->GetTransform()->GetLocalPosition().x,
		_player->GetTransform()->GetLocalPosition().y,
		_player->GetTransform()->GetLocalPosition().z
	);

	SetWindowText(core->GetWindowHandle(), wchTxt);
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

	shared_ptr<ObserveCamera> observeCamera = make_shared<ObserveCamera>();
	CameraManager::GetInstance()->AddCamera(CameraType::OBSERVE, observeCamera);
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

		player->AddCollider("this",ColliderType::Box, vec3(-2.5f, -1.0f, -0.5f));
		player->AddBoxCollider("raycheck",vec3(1.5f, 1.5f,50.0f),vec3(0,2.0f,-50.0f));
	
		player->SetThirdPersonCamera(static_pointer_cast<ThirdPersonCamera>(CameraManager::GetInstance()->GetCamera(CameraType::THIRDVIEW)));

		shared_ptr<Terrain> terrain = make_shared<Terrain>();
		AddGameObject(terrain);
		player->SetTerrain(terrain);

		AddGameObject(player);

	}

	for (int i = 0; i < 50; ++i)
	{
		shared_ptr<Box> object = make_shared<Box>();
		shared_ptr<Model> data = Model::ReadData(L"Box/Box");
		object->SetModel(data);
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
