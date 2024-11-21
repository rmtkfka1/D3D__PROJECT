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
#include "BilboardObject.h"
#include <random>

static default_random_engine dre;
static uniform_int_distribution<int> random_xz(-3800, 3800);

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

	core->GetGraphics()->GetRenderTarget()->ClearDepth();
	CameraControl();

	core->GetGraphics()->GetGBuffer()->RenderBegin();
	DeferredRender();
	core->GetGraphics()->GetGBuffer()->RenderEnd();

	ComputePass();
	
	core->GetGraphics()->GetRenderTarget()->RenderBegin();
	FinalRender();
	ForwardRender();
	BoundingBoxRender();
	UiObjectRender();
	core->GetGraphics()->GetRenderTarget()->RenderEnd();



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

		player->SetModel(data);
		shared_ptr<GraphicsShader> shader = ResourceManager::GetInstance()->Get<GraphicsShader>(L"deferred.hlsl");
		player->SetShader(shader);

		player->GetTransform()->SetLocalScale(vec3(5.0f, 5.0f, 5.0f));
		player->GetTransform()->SetLocalPosition(vec3(100.0f, 0, 0));

		player->AddBoxColliderWithModel("this", ColliderBehave::Active,vec3(-2.5f, -1.0f, -0.5f));
		player->AddBoxCollider("raycheck", ColliderBehave::Active, vec3(1.5f, 1.5f, 40.0f), vec3(0, 2.0f, -30.0f));

		player->SetThirdPersonCamera(static_pointer_cast<ThirdPersonCamera>(CameraManager::GetInstance()->GetCamera(CameraType::THIRDVIEW)));

		shared_ptr<Terrain> terrain = make_shared<Terrain>();
		_terrain = terrain;
		_terrain->Init();
		terrain->SetFrustumCuling(false);
		player->SetTerrain(terrain);

		AddGameObject(player, RenderingType::Deferred);
		AddGameObject(terrain,RenderingType::Forward);

		_player = player;

	}


	for (int i = 0; i < 10; ++i)
	{
		shared_ptr<Box> object = make_shared<Box>();
		shared_ptr<Model> data = Model::ReadData(L"Box/Box",L"Box");
		object->SetModel(data);
		object->SetShader(ResourceManager::GetInstance()->Get<GraphicsShader>(L"deferred.hlsl"));
		object->AddBoxColliderWithModel("block",ColliderBehave::Passive);
		AddGameObject(object, RenderingType::Deferred);

	}
	



	for (int i = 0; i < 10; ++i)
	{
		shared_ptr<Sphere> object = make_shared<Sphere>();
		shared_ptr<Model> data = Model::ReadData(L"Earth/Earth",L"Earth");
		object->SetModel(data);
		object->SetShader(ResourceManager::GetInstance()->Get<GraphicsShader>(L"deferred.hlsl"));
		object->AddSphereColliderWithModel("earth", ColliderBehave::Passive);
		AddGameObject(object, RenderingType::Deferred);
	}

	//{
	//	shared_ptr<Enemy> enemy = make_shared<Enemy>();
	//	shared_ptr<Model> data = Model::ReadData(L"helicopter/helicopter",L"EnemyHelicopter");
	//	data->SetIntValue(0, 1);
	//	enemy->SetModel(data);
	//	enemy->SetPlayer(_player);
	//	enemy->GetTransform()->SetLocalScale(vec3(70.0f, 70.0f, 70.0f));
	//	enemy->GetTransform()->SetLocalPosition(vec3(2000.0f, 5000.0f, 0));
	//	//enemy->GetTransform()->SetLocalRotation(vec3(50.0f, 0, 40.0f));
	//	enemy->SetShader(ResourceManager::GetInstance()->Load<Shader>(L"deferred.hlsl"));
	//	//enemy->AddBoxCollider("raycheck", vec3(1.5f, 1.5f, 40.0f), vec3(0, 2.0f, -30.0f));
	//	enemy->AddBoxColliderWithModel("enemy", ColliderBehave::Active,vec3(-2.0f,-0.5,0));
	//	AddGameObject(enemy, RenderingType::Deferred);
	//}


	for (int i = 0; i < 50; ++i)
	{
		shared_ptr<BilboardObject> gameobject = make_shared<BilboardObject>();
		gameobject->_useWithHeightMap = true;

		vector<Vertex> v;

		v.push_back(Vertex(vec3(0, 0, 0.0f), vec2(0.0f, 0.0f)));
		gameobject->GetMesh()->Init(v);


		int randomValueX = random_xz(dre);
		int randomValueZ = random_xz(dre);

		vec3 Pos = _terrain->GetHeight(vec3(randomValueX, 0, randomValueZ));
		gameobject->GetTransform()->SetLocalPosition(vec3(Pos.x, Pos.y + 250.0f, Pos.z));
		gameobject->GetTransform()->SetLocalScale(vec3(60.0f, 60.0f, 60.0f));
		gameobject->AddBoxCollider("bilboard", ColliderBehave::Passive, vec3(2.0f, 5.0f, 2.0f), vec3(0, 0, 0));
		gameobject->SetShader(ResourceManager::GetInstance()->Get<GraphicsShader>(L"Bilboard.hlsl"));

		AddGameObject(gameobject, RenderingType::Deferred);
	}

	for (int i = 0; i < 10; ++i)
	{
		shared_ptr<BilboardObject> gameobject = make_shared<BilboardObject>();


		vector<Vertex> v;
		v.push_back(Vertex(vec3(0, 0, 0.0f), vec2(0.0f, 0.0f)));
		gameobject->GetMesh()->Init(v);

		gameobject->SetShader(ResourceManager::GetInstance()->Get<GraphicsShader>(L"Bilboard.hlsl"));

		AddGameObject(gameobject, RenderingType::Deferred);
	}


	{
		shared_ptr<CustomObject> gameobject = make_shared<CustomObject>();
		gameobject->SetFrustumCuling(false);
		gameobject->GetMesh() = GeoMetryHelper::LoadRectangleBox(10.0f);

		shared_ptr<Texture> texture = ResourceManager::GetInstance()->Load<Texture>(L"cubemap/output.dds", TextureType::CubeMap);

		shared_ptr<GraphicsShader> shader = ResourceManager::GetInstance()->Get<GraphicsShader>(L"sky.hlsl");

		gameobject->SetShader(shader);
		gameobject->GetMaterial()->SetDiffuseTexture(texture);

		AddGameObject(gameobject, RenderingType::Deferred);
	}




	//temp
	Model::ReadData(L"playerBullet/playerBullet", L"playerBullet");
}

void Stage1::BulidForward()
{


	for (int i = 0; i < 3; ++i)
	{
		shared_ptr<CustomObject> object = make_shared<CustomObject>();
		ResourceManager::GetInstance()->Add<GameObject>(L"gbufferUi"+i, object);
		object->GetMesh() = GeoMetryHelper::LoadRectangleMesh(30.0f);
		object->SetShader(ResourceManager::GetInstance()->Load<GraphicsShader>(L"uishader.hlsl"));

	
		object->GetMaterial()->SetDiffuseTexture(GRAPHICS->GetGBuffer()->GetTexture(i));
		
		object->GetTransform()->SetLocalScale(vec3(3.0f, 3.0f, 3.0f));
		object->GetTransform()->SetLocalPosition(vec3(-850.0f + 200.0f * i, 400.0f, 1.0f));
		AddGameObject(object, RenderingType::Ui);
	}

	


	//{
	//	shared_ptr<Sea> gameobject = make_shared<Sea>();
	//	gameobject->SetFrustumCuling(false);
	//	gameobject->GetMesh() = GeoMetryHelper::LoadRectangleBox(4000.0f);

	//	shared_ptr<Texture> texture = ResourceManager::GetInstance()->Load<Texture>(L"sea.jpg");

	//	ShaderInfo info;
	//	info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
	//	info.blendType = BLEND_TYPE::ONE_TO_ONE_BLEND;
	//	//info.depthStencilType = DEPTH_STENCIL_TYPE::DPTH_TEST_NO_WRITE;
	//	shared_ptr<Shader> shader = make_shared<Shader>();
	//	shader->Init(L"blendingsea.hlsl", info);

	//	gameobject->GetTransform()->SetLocalPosition(vec3(0, -2700.0f, 0));

	//	gameobject->SetShader(shader);
	//	gameobject->GetMaterial()->SetDiffuseTexture(texture);

	//	AddGameObject(gameobject, RenderingType::Forward);
	//}

	{

		shared_ptr<Sea> gameobject = make_shared<Sea>();
		gameobject->SetFrustumCuling(false);
		gameobject->GetMesh() = GeoMetryHelper::LoadRectangleMesh(2000.0f);
		gameobject->SetShader(ResourceManager::GetInstance()->Get<GraphicsShader>(L"sea.hlsl"));
		

		gameobject->GetTransform()->SetLocalPosition(vec3(9000.0f, 2000.0f, 0));
		gameobject->GetTransform()->SetLocalRotation(vec3(0, 90.0f, 0));
		AddGameObject(gameobject, RenderingType::Forward);
	}


	{

		shared_ptr<Sea> gameobject = make_shared<Sea>();
		gameobject->SetFrustumCuling(false);
		gameobject->GetMesh() = GeoMetryHelper::LoadRectangleMesh(2000.0f);
		gameobject->SetShader(ResourceManager::GetInstance()->Get<GraphicsShader>(L"seatest.hlsl"));


		gameobject->GetTransform()->SetLocalPosition(vec3(9000.0f, 2000.0f, 5000.0f));
		gameobject->GetTransform()->SetLocalRotation(vec3(0, 90.0f, 0));
		AddGameObject(gameobject, RenderingType::Forward);
	}

	{

		shared_ptr<Sea> gameobject = make_shared<Sea>();

		gameobject->SetFrustumCuling(false);
		gameobject->GetMesh() = GeoMetryHelper::LoadRectangleMesh(2000.0f);
		gameobject->SetShader(ResourceManager::GetInstance()->Get<GraphicsShader>(L"seatest2.hlsl"));


		gameobject->GetTransform()->SetLocalPosition(vec3(9000.0f, 2000.0f, -5000.0f));
		gameobject->GetTransform()->SetLocalRotation(vec3(0, 90.0f, 180.0f));
		AddGameObject(gameobject, RenderingType::Forward);
	}

	{

		shared_ptr<Sea> gameobject = make_shared<Sea>();

		gameobject->SetFrustumCuling(false);
		gameobject->GetMesh() = GeoMetryHelper::LoadRectangleBox(4000.0f);
		gameobject->SetShader(ResourceManager::GetInstance()->Get<GraphicsShader>(L"seatest2Blend.hlsl"));


		gameobject->GetTransform()->SetLocalPosition(vec3(0, -3000.0f, -500.0));
		gameobject->GetTransform()->SetLocalRotation(vec3(0, 90.0f, 0));
		AddGameObject(gameobject, RenderingType::Forward);
	}


	////시연용
	//{

	//	shared_ptr<Sea> gameobject = make_shared<Sea>();

	//	gameobject->SetFrustumCuling(false);
	//	gameobject->GetMesh() = GeoMetryHelper::LoadRectangleBox(4000.0f);
	//	gameobject->SetShader(ResourceManager::GetInstance()->Get<GraphicsShader>(L"seatest2.hlsl"));

	//	gameobject->GetTransform()->SetLocalPosition(vec3(0, -10000.0f, -500.0));
	//	gameobject->GetTransform()->SetLocalRotation(vec3(0, 90.0f, 0));
	//	AddGameObject(gameobject, RenderingType::Forward);
	//}




	

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
	
	{
		auto& list = GRAPHICS->GetCmdLIst();
		ResourceManager::GetInstance()->Get<GraphicsShader>(L"final.hlsl")->SetPipelineState();
		shared_ptr<Mesh> mesh = ResourceManager::GetInstance()->Get<Mesh>(L"finalMesh");
		shared_ptr<Material> material = ResourceManager::GetInstance()->Get<Material>(L"finalMaterial");
		material->PushData();
		core->GetBufferManager()->GetGraphicsTableHeap()->SetGraphicsRootDescriptorTable();
		mesh->Render();
	}

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
}
void Stage1::ComputePass()
{

	static int PostProcess = 1;

	if (KeyManager::GetInstance()->GetButtonDown(KEY_TYPE::Q))
	{
		PostProcess *= -1;
	}

	{
		shared_ptr<Texture>& texture = ResourceManager::GetInstance()->Get<Texture>(L"TestCS");
		GRAPHICS->GetCmdLIst()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(texture->GetResource().Get(),
			D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
	}

	{
		ResourceManager::GetInstance()->Get<ComputeShader>(L"compute.hlsl")->SetPipelineState();
		core->GetBufferManager()->GetComputeTableHeap()->CopyUAV(ResourceManager::GetInstance()->Get<Texture>(L"TestCS")->GetUAVCpuHandle(), UAV_REGISTER::u0);
		core->GetBufferManager()->GetComputeTableHeap()->CopySRV(GRAPHICS->GetGBuffer()->GetTexture(2)->GetSRVCpuHandle(), SRV_REGISTER::t0);

		auto& material = ResourceManager::GetInstance()->Get<Material>(L"TestMaterial");

		material->SetInt(0, WINDOW_WIDTH);
		material->SetInt(1, WINDOW_HEIGHT);
		material->SetInt(2, PostProcess);

		material->PushComputeData();
		core->GetBufferManager()->GetComputeTableHeap()->SetComputeRootDescriptorTable();

		int threadGroupSizeX = 16;
		int threadGroupSizeY = 16;

		// Dispatch 크기 계산
		int dispatchX = (WINDOW_WIDTH + threadGroupSizeX - 1) / threadGroupSizeX;
		int dispatchY = (WINDOW_HEIGHT + threadGroupSizeY - 1) / threadGroupSizeY;

		COMPUTE->GetCmdList()->Dispatch(dispatchX, dispatchY, 1);
		COMPUTE->Excute();
	}

	{
		shared_ptr<Texture>& texture = ResourceManager::GetInstance()->Get<Texture>(L"TestCS");
		GRAPHICS->GetCmdLIst()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(texture->GetResource().Get(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE));
	}

}
;
