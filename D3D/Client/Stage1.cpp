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
#include "Sea.h"
#include "BufferPool.h"
#include "TimeManager.h"
#include "Enemy.h"
Stage1::Stage1()
{
}

Stage1::~Stage1()
{
}

void Stage1::Init()
{
	BulidCamera();
	BulidDeferred();
	BulidForward();
	BulidLight();
	Scene::Init(); //ObjectInit
}



void Stage1::Run()
{
	LightManager::GetInstnace()->SetData();
	Scene::Run();

	core->GetRenderTarget()->ClearDepth();
	CameraControl();

	core->GetGBuffer()->RenderBegin();
	DeferredRender();
	core->GetGBuffer()->RenderEnd();

	core->GetRenderTarget()->RenderBegin();
	FinalRender();
	BoundingBoxRender();
	ForwardRender();
	UiObjectRender();
	core->GetRenderTarget()->RenderEnd();



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
		light.material.ambient = vec3(0.6f, 0.6f, 0.6f);
		light.material.diffuse = vec3(0.6f, 0.6f, 0.6f);
		light.material.specular = vec3(0.6f, 0.6f, 0.6f);
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
};

void Stage1::BulidDeferred()
{


	{
		shared_ptr<Model> data = Model::ReadData(L"helicopter/helicopter",L"helicopter");
		shared_ptr<Player> player = make_shared<Player>();

		_player = player;
		player->SetModel(data);
		shared_ptr<Shader> shader = ResourceManager::GetInstance()->Get<Shader>(L"deferred.hlsl");
		player->SetShader(shader);

		player->GetTransform()->SetLocalScale(vec3(5.0f, 5.0f, 5.0f));
		player->GetTransform()->SetLocalPosition(vec3(100.0f, 0, 0));

		player->AddCollider("this", ColliderType::Box, vec3(-2.5f, -1.0f, -0.5f));
		player->AddBoxCollider("raycheck", vec3(1.5f, 1.5f, 40.0f), vec3(0, 2.0f, -30.0f));

		player->SetThirdPersonCamera(static_pointer_cast<ThirdPersonCamera>(CameraManager::GetInstance()->GetCamera(CameraType::THIRDVIEW)));

		shared_ptr<Terrain> terrain = make_shared<Terrain>();
		terrain->SetFrustumCuling(false);
		player->SetTerrain(terrain);

		AddGameObject(player, RenderingType::Deferred);
		AddGameObject(terrain,RenderingType::Forward);

	}

	for (int i = 0; i < 10; ++i)
	{
		shared_ptr<Box> object = make_shared<Box>();
		shared_ptr<Model> data = Model::ReadData(L"Box/Box",L"Box");
		object->SetModel(data);
		object->SetShader(ResourceManager::GetInstance()->Get<Shader>(L"deferred.hlsl"));
		object->AddCollider("boxbox", ColliderType::Box);
		AddGameObject(object, RenderingType::Deferred);
	}



	for (int i = 0; i < 10; ++i)
	{
		shared_ptr<Sphere> object = make_shared<Sphere>();
		shared_ptr<Model> data = Model::ReadData(L"Earth/Earth",L"Earth");
		object->SetModel(data);
		object->SetShader(ResourceManager::GetInstance()->Get<Shader>(L"deferred.hlsl"));
		object->AddCollider("earth", ColliderType::Sphere);
		AddGameObject(object, RenderingType::Deferred);
	}

	{

		shared_ptr<Enemy> enemy = make_shared<Enemy>();
		shared_ptr<Model> data = Model::ReadData(L"helicopter/helicopter",L"EnemyHelicopter");
		data->SetIntValue(0, 1);
		enemy->SetModel(data);
		enemy->GetTransform()->SetLocalScale(vec3(10.0f, 10.0f, 10.0f));
		enemy->GetTransform()->SetLocalPosition(vec3(0, 2000.0f, 0));
		enemy->SetShader(ResourceManager::GetInstance()->Load<Shader>(L"deferred.hlsl"));
		enemy->AddCollider("enemy", ColliderType::Box);
		AddGameObject(enemy, RenderingType::Deferred);
	}
}

void Stage1::BulidForward()
{


	for (int i = 0; i < 3; ++i)
	{
		shared_ptr<CustomObject> object = make_shared<CustomObject>();
		ResourceManager::GetInstance()->Add<GameObject>(L"gbufferUi"+i, object);
		object->GetMesh() = GeoMetryHelper::LoadRectangleMesh(30.0f);
		object->SetShader(ResourceManager::GetInstance()->Load<Shader>(L"uishader.hlsl"));
		object->GetMaterial()->SetDiffuseTexture(core->GetGBuffer()->GetTexture(i));
		object->GetTransform()->SetLocalScale(vec3(3.0f, 3.0f, 3.0f));
		object->GetTransform()->SetLocalPosition(vec3(-850.0f + 200.0f * i, 400.0f, 1.0f));
		AddGameObject(object, RenderingType::Ui);
	}

	{

		shared_ptr<CustomObject> gameobject = make_shared<CustomObject>();
		gameobject->SetFrustumCuling(false);
		gameobject->GetMesh() = GeoMetryHelper::LoadRectangleBox(10.0f);

		shared_ptr<Texture> texture = ResourceManager::GetInstance()->Load<Texture>(L"cubemap/output.dds", TextureType::CubeMap);

		shared_ptr<Shader> shader = ResourceManager::GetInstance()->Get<Shader>(L"sky.hlsl");

		gameobject->SetShader(shader);
		gameobject->GetMaterial()->SetDiffuseTexture(texture);

		AddGameObject(gameobject,RenderingType::Forward);
	}


	{
		shared_ptr<Sea> gameobject = make_shared<Sea>();
		gameobject->SetFrustumCuling(false);
		gameobject->GetMesh() = GeoMetryHelper::LoadRectangleBox(4000.0f);

		shared_ptr<Texture> texture = ResourceManager::GetInstance()->Load<Texture>(L"sea.jpg");

		ShaderInfo info;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.blendType = BLEND_TYPE::ONE_TO_ONE_BLEND;
		//info.depthStencilType = DEPTH_STENCIL_TYPE::DPTH_TEST_NO_WRITE;
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->Init(L"blendingsea.hlsl", info);

		gameobject->GetTransform()->SetLocalPosition(vec3(0, -2700.0f, 0));

		gameobject->SetShader(shader);
		gameobject->GetMaterial()->SetDiffuseTexture(texture);

		AddGameObject(gameobject, RenderingType::Forward);
	}


}


void Stage1::DeferredRender()
{

	static int count = 0;
	for (auto& ele : _deferredObjects)
	{
		ele->Update();

		if (ele->GetFrustumCuling())
		{
			if (CameraManager::GetInstance()->GetActiveCamera()->IsInFrustum(ele->GetCollider()) == false)
			{
				continue;
			}
		}

		count++;
		ele->Render();
	}

	TimeManager::GetInstance()->_objectCount = count;
	count = 0;
}

void Stage1::ForwardRender()
{
	for (auto& ele : _forwardObjects)
	{
		ele->Update();

		if (ele->GetFrustumCuling())
		{
			if (CameraManager::GetInstance()->GetActiveCamera()->IsInFrustum(ele->GetCollider()) == false)
			{
				continue;
			}
		}

		ele->Render();
	}
}

void Stage1::UiObjectRender()
{
	CameraManager::GetInstance()->SetActiveCamera(CameraType::UI);
	CameraManager::GetInstance()->SetData();

	for (auto& ele : _uiObjects)
	{
		ele->Update();
		ele->Render();
	}
}

void Stage1::FinalRender()
{

	auto& list = core->GetCmdLIst();


	ResourceManager::GetInstance()->Get<Shader>(L"final.hlsl")->SetPipelineState();
	shared_ptr<Mesh> mesh = ResourceManager::GetInstance()->Get<Mesh>(L"finalMesh");
	shared_ptr<Material> material = ResourceManager::GetInstance()->Get<Material>(L"finalMaterial");


	material->Pushdata();
	core->GetTableHeap()->SetGraphicsRootDescriptorTable();
	mesh->Render();

}

void Stage1::BoundingBoxRender()
{
	if (BaseCollider::brender == false)
		return;

	for (auto& ele : _deferredObjects)
	{
		ele->BoundingRender();
	}
}

void Stage1::CameraControl()
{
	static CameraType type = CameraType::THIRDVIEW;

	if (KeyManager::GetInstance()->GetButton(KEY_TYPE::ONE))
	{
		type = CameraType::OBSERVE;
		CameraManager::GetInstance()->ChangeSetting(type);
	}

	if (KeyManager::GetInstance()->GetButton(KEY_TYPE::THREE))
	{
		type = CameraType::THIRDVIEW;
	}

	if (type == CameraType::OBSERVE)
	{
		CameraManager::GetInstance()->SetActiveCamera(CameraType::OBSERVE);
	}

	else if (type == CameraType::THIRDVIEW)
	{
		CameraManager::GetInstance()->SetActiveCamera(CameraType::THIRDVIEW);
	}


	CameraManager::GetInstance()->SetData();
};
