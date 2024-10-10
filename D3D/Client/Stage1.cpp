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
#include "RenderTargets.h"
#include "Sphere.h"
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
		light.material.ambient = vec3(0.5f, 0.5f, 0.5f);
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

	shared_ptr<ObserveCamera> observeCamera = make_shared<ObserveCamera>();
	CameraManager::GetInstance()->AddCamera(CameraType::OBSERVE, observeCamera);

	shared_ptr<UiCamera> uicamera = make_shared<UiCamera>();
	CameraManager::GetInstance()->AddCamera(CameraType::UI, uicamera);
	CameraManager::GetInstance()->SetActiveCamera(CameraType::THIRDVIEW);

}

void Stage1::BulidObject()
{
	
	BulidDeferred();
	BulidForward();

	
}

void Stage1::BulidDeferred()
{

	{
		shared_ptr<Model> data = Model::ReadData(L"helicopter/helicopter");
		shared_ptr<Player> player = make_shared<Player>();

		_player = player;
		player->SetModel(data);
		shared_ptr<Shader> shader = ResourceManager::GetInstance()->Get<Shader>(L"deferred.hlsl");
		player->SetShader(shader);

		player->GetTransform()->SetLocalScale(vec3(5.0f, 5.0f, 5.0f));
		player->GetTransform()->SetLocalPosition(vec3(100.0f, 0, 0));

		player->AddCollider("this", ColliderType::Box, vec3(-2.5f, -1.0f, -0.5f));
		player->AddBoxCollider("raycheck", vec3(1.5f, 1.5f, 5.0f), vec3(0, 2.0f, -10.0f));

		player->SetThirdPersonCamera(static_pointer_cast<ThirdPersonCamera>(CameraManager::GetInstance()->GetCamera(CameraType::THIRDVIEW)));

		shared_ptr<Terrain> terrain = make_shared<Terrain>();
		terrain->SetFrustumCuling(false);
		player->SetTerrain(terrain);

		AddDeferredObject(player);
		AddForwardObject(terrain);

	}

	for (int i = 0; i < 50; ++i)
	{
		shared_ptr<Box> object = make_shared<Box>();
		shared_ptr<Model> data = Model::ReadData(L"Box/Box");
		object->SetModel(data);
		object->SetShader(ResourceManager::GetInstance()->Get<Shader>(L"deferred.hlsl"));
		object->AddCollider("boxbox", ColliderType::Box);
		AddDeferredObject(object);
	}

	for (int i = 0; i < 10; ++i)
	{
		shared_ptr<Sphere> object = make_shared<Sphere>();
		shared_ptr<Model> data = Model::ReadData(L"Earth/Earth");
		object->SetModel(data);
		object->SetShader(ResourceManager::GetInstance()->Get<Shader>(L"deferred.hlsl"));
		object->AddCollider("sphere", ColliderType::Sphere);
		AddDeferredObject(object);
	}

}

void Stage1::BulidForward()
{

	for (int i = 0; i < 3; ++i)
	{
		shared_ptr<CustomObject> object = make_shared<CustomObject>();

		object->GetMesh() = GeoMetryHelper::LoadRectangleMesh(30.0f);
		object->SetShader(ResourceManager::GetInstance()->Get<Shader>(L"default.hlsl"));
		object->GetMaterial()->SetDiffuseTexture(core->GetGBuffer()->GetTexture(i));
		object->GetTransform()->SetLocalScale(vec3(1.0f, 1.0f, 1.0f));
		object->GetTransform()->SetLocalPosition(vec3(-260.0 + 70.0f * i, 250.0f, 1.0f));
		AddUiObject(object);
	}

	{

		shared_ptr<CustomObject> gameobject = make_shared<CustomObject>();
		gameobject->SetFrustumCuling(false);
		gameobject->GetMesh() = GeoMetryHelper::LoadRectangleBox(10.0f);

		shared_ptr<Texture> texture = ResourceManager::GetInstance()->Load<Texture>(L"cubemap/skybox.dds", TextureType::CubeMap);

		shared_ptr<Shader> shader = ResourceManager::GetInstance()->Get<Shader>(L"sky.hlsl");

		gameobject->SetShader(shader);
		gameobject->GetMaterial()->SetDiffuseTexture(texture);

		AddForwardObject(gameobject);
	}
}
