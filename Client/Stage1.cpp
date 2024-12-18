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
#include "BloomEffect.h"
#include "Mirror.h"
#include "Tessllation.h"
#include "AnimationObject.h"

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

	core->GetRenderTarget()->ClearDepth();

	core->GetShadow()->RenderBegin();
	ShadowRender();
	core->GetShadow()->RenderEnd();

	CameraControl();

	core->GetGBuffer()->RenderBegin();
	DeferredRender();
	core->GetGBuffer()->RenderEnd();

	core->GetRenderTarget()->RenderBegin();
	FinalRender();
	ForwardRender();
	BoundingBoxRender();
	UiObjectRender();
	ComputePass();
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

	shared_ptr<ShadowCamera> shadowCamera = make_shared<ShadowCamera>();
	CameraManager::GetInstance()->AddCamera(CameraType::SHADOW, shadowCamera);

	shadowCamera->Update();
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
		auto& mat = static_pointer_cast<ShadowCamera>(CameraManager::GetInstance()->GetCamera(CameraType::SHADOW))->GetVPMatrix();

		auto&  v =player->GetMatrial();
		for (auto& ele : v)
		{
			ele->SetMatrix(0,mat);
		}

		AddGameObject(player, RenderingType::Deferred);
		AddGameObject(terrain,RenderingType::Forward);

		_player = player;

		ResourceManager::GetInstance()->Add<GameObject>(L"Player", _player);

	}


	for (int i = 0; i < 10; ++i)
	{
		shared_ptr<Box> object = make_shared<Box>();
		shared_ptr<Model> data = Model::ReadData(L"Box/Box", L"Box");
		object->SetModel(data);
		object->SetShader(ResourceManager::GetInstance()->Get<GraphicsShader>(L"deferred.hlsl"));
		object->AddBoxColliderWithModel("block", ColliderBehave::Passive);

		auto& camera = CameraManager::GetInstance()->GetCamera(CameraType::SHADOW);
		Matrix mat = static_pointer_cast<ShadowCamera>(camera)->GetVPMatrix();
		auto& vec = object->GetMaterial();

		for (auto& ele : vec)
		{
			ele->SetMatrix(0, mat);
		}

		AddGameObject(object, RenderingType::Deferred);

	}


	for (int i = 0; i < 3; ++i)
	{
		shared_ptr<Sphere> object = make_shared<Sphere>();
		shared_ptr<Model> data = Model::ReadData(L"Earth/Earth",L"Earth");
		object->SetModel(data);
		object->SetShader(ResourceManager::GetInstance()->Get<GraphicsShader>(L"deferred.hlsl"));
		object->AddSphereColliderWithModel("earth", ColliderBehave::Passive);

		auto& camera = CameraManager::GetInstance()->GetCamera(CameraType::SHADOW);
		Matrix mat = static_pointer_cast<ShadowCamera>(camera)->GetVPMatrix();
		auto& vec = object->GetMaterial();

		for (auto& ele : vec)
		{
			ele->SetMatrix(0,mat);
		}

		AddGameObject(object, RenderingType::Deferred);
	}


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

	/*for (int i = 0; i < 10; ++i)
	{
		shared_ptr<BilboardObject> gameobject = make_shared<BilboardObject>();

		vector<Vertex> v;
		v.push_back(Vertex(vec3(0, 0, 0.0f), vec2(0.0f, 0.0f)));
		gameobject->GetMesh()->Init(v);

		gameobject->SetShader(ResourceManager::GetInstance()->Get<GraphicsShader>(L"Bilboard.hlsl"));

		AddGameObject(gameobject, RenderingType::Deferred);
	}*/


	{
		shared_ptr<ModelObject> gameobject = make_shared<ModelObject>();
		gameobject->SetFrustumCuling(false);
		shared_ptr<Model> model = Model::ReadData(L"room/room", L"room");
		gameobject->SetModel(model);
		gameobject->GetTransform()->SetLocalScale(vec3(5000.0f, 5000.0f, 5000.0f));
		gameobject->GetTransform()->SetLocalPosition(vec3(50000, 49000.0f, 50000));

		shared_ptr<GraphicsShader> shader = ResourceManager::GetInstance()->Get<GraphicsShader>(L"deferred.hlsl");
		gameobject->SetShader(shader);

		ResourceManager::GetInstance()->Add<GameObject>(L"room", gameobject);
		AddGameObject(gameobject, RenderingType::Deferred);
	}

	{
		shared_ptr<CustomObject> gameobject = make_shared<CustomObject>();
		gameobject->SetFrustumCuling(false);
		gameobject->GetMesh() = GeoMetryHelper::LoadRectangleBox(10.0f);

		shared_ptr<Texture> texture = ResourceManager::GetInstance()->Load<Texture>(L"cubemap/result.dds", TextureType::CubeMap);

		shared_ptr<GraphicsShader> shader = ResourceManager::GetInstance()->Get<GraphicsShader>(L"sky.hlsl");

		gameobject->SetShader(shader);
		gameobject->GetMaterial()->SetDiffuseTexture(texture);

		AddGameObject(gameobject, RenderingType::Deferred);
	}



	{
		shared_ptr<Tessllation> gameobject = make_shared<Tessllation>();
		gameobject->SetFrustumCuling(false);

		std::vector<Vertex> controlPoints;

		Vertex p;
		p.position = vec3(-1.0f, 1.0f, 0);

		Vertex p2;
		p2.position = vec3(1.0f, 1.0f, 0);

		Vertex p3;
		p3.position = vec3(-1.0f, -1.0f, 0);

		Vertex p4;
		p4.position = vec3(1.0f, -1.0f, 0);

		controlPoints.push_back(p);
		controlPoints.push_back(p2);
		controlPoints.push_back(p3);
		controlPoints.push_back(p4);
	
		gameobject->GetMesh()->Init(controlPoints);
		//gameobject->GetMesh() = GeoMetryHelper::LoadRectangleMesh(1.0f);
		gameobject->GetTransform()->SetLocalPosition(vec3(50000.0f, 50000.f, 50000.0f));
		gameobject->GetTransform()->SetLocalScale(vec3(100.0f, 100.0f, 100.0f));

		shared_ptr<GraphicsShader> shader = make_shared<GraphicsShader>();
		ShaderInfo info;
		info.rasterizerType = RASTERIZER_TYPE::WIREFRAME;
		info.bActiveDSShader = true;
		info.bActiveHSShader = true;
		info.primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		shader->Init(L"tess.hlsl",info);
		gameobject->SetShader(shader);
		AddGameObject(gameobject, RenderingType::Forward);
	}


	{
		shared_ptr<AnimationObject> gameobject = make_shared<AnimationObject>();
		gameobject->SetFrustumCuling(false);
		shared_ptr<Model> model = Model::ReadData(L"Kachujin/Kachujin", L"Kachujin");
		model->ReadAnimation(L"Kachujin/Slash");
		gameobject->SetModel(model);
		gameobject->SetShader(ResourceManager::GetInstance()->Load<GraphicsShader>(L"deferredAnimation.hlsl"));

		ResourceManager::GetInstance()->Add<GameObject>(L"Kachujin", gameobject);
		gameobject->GetTransform()->SetLocalScale(vec3(2.0f, 2.0f, 2.0f));
		gameobject->GetTransform()->SetLocalRotation(vec3(0, 270.0f, 0));
		gameobject->GetTransform()->SetLocalPosition(vec3(50200.0f, 49700.0f, 49900.0f));
		AddGameObject(gameobject, RenderingType::Deferred);
	}


	{
		shared_ptr<Mirror> gameobject = make_shared<Mirror>();
		gameobject->PushObject(ResourceManager::GetInstance()->Get<GameObject>(L"Player"));
		gameobject->PushObject(ResourceManager::GetInstance()->Get<GameObject>(L"room"));
		gameobject->PushAniObject(static_pointer_cast<AnimationObject>(ResourceManager::GetInstance()->Get<GameObject>(L"Kachujin")));
		gameobject->SetFrustumCuling(false);
		shared_ptr<Model> model = Model::ReadData(L"mirror/mirror", L"mirror");
		gameobject->SetModel(model);
		gameobject->GetTransform()->SetLocalScale(vec3(4.0f, 4.0f, 4.0f));
		gameobject->GetTransform()->SetLocalPosition(vec3(51400.0f, 49600.0f, 47650.0f));
		AddGameObject(gameobject, RenderingType::Deferred);
	}





	////temp
	Model::ReadData(L"playerBullet/playerBullet", L"playerBullet");
}

void Stage1::BulidForward()
{

	for (int i = 0; i < 3; ++i)
	{
		shared_ptr<CustomObject> object = make_shared<CustomObject>();
		ResourceManager::GetInstance()->Add<GameObject>(L"gbufferUi"+i, object);
		object->GetMesh() = GeoMetryHelper::LoadRectangleMesh(30.0f);
	

		if (i < 2)
		{
			object->GetMaterial()->SetDiffuseTexture(core->GetGBuffer()->GetTexture(i));
			object->SetShader(ResourceManager::GetInstance()->Load<GraphicsShader>(L"uishader.hlsl"));
		}

		else
		{
			object->GetMaterial()->SetDiffuseTexture(core->GetShadow()->GetTexture());
			object->SetShader(ResourceManager::GetInstance()->Load<GraphicsShader>(L"depthrender.hlsl"));
		}
		
		object->GetTransform()->SetLocalScale(vec3(3.0f, 3.0f, 3.0f));
		object->GetTransform()->SetLocalPosition(vec3(-850.0f + 200.0f * i, 400.0f, 1.0f));
		AddGameObject(object, RenderingType::Ui);
	}

	{
		shared_ptr<CustomObject> gameobject = make_shared<CustomObject>();
		gameobject->SetFrustumCuling(false);
		gameobject->GetMesh() = GeoMetryHelper::LoadRectangleMesh(1000.0f);

		shared_ptr<Texture> texture = core->GetShadow()->GetTexture();

		shared_ptr<GraphicsShader> shader = ResourceManager::GetInstance()->Get<GraphicsShader>(L"depthrender.hlsl");

		gameobject->SetShader(shader);
		gameobject->GetMaterial()->SetDiffuseTexture(texture);

		gameobject->GetTransform()->SetLocalPosition(vec3(6000.0f, 2000.0f, 4000.0f));
		gameobject->GetTransform()->SetLocalRotation(vec3(0, 90.0f, 0));

		AddGameObject(gameobject, RenderingType::Forward);
	}


	{

		shared_ptr<Sea> gameobject = make_shared<Sea>();
		gameobject->SetFrustumCuling(false);
		gameobject->GetMesh() = GeoMetryHelper::LoadRectangleMesh(1000.0f);
		gameobject->SetShader(ResourceManager::GetInstance()->Get<GraphicsShader>(L"sea.hlsl"));
		

		gameobject->GetTransform()->SetLocalPosition(vec3(6000.0f, 2000.0f, 0));
		gameobject->GetTransform()->SetLocalRotation(vec3(0, 90.0f, 0));
		AddGameObject(gameobject, RenderingType::Forward);
	}


	{

		shared_ptr<Sea> gameobject = make_shared<Sea>();
		gameobject->SetFrustumCuling(false);
		gameobject->GetMesh() = GeoMetryHelper::LoadRectangleMesh(1000.0f);
		gameobject->SetShader(ResourceManager::GetInstance()->Get<GraphicsShader>(L"seatest.hlsl"));


		gameobject->GetTransform()->SetLocalPosition(vec3(6000.0f, 2000.0f, 2000.0f));
		gameobject->GetTransform()->SetLocalRotation(vec3(0, 90.0f, 0));
		AddGameObject(gameobject, RenderingType::Forward);
	}

	{

		shared_ptr<Sea> gameobject = make_shared<Sea>();

		gameobject->SetFrustumCuling(false);
		gameobject->GetMesh() = GeoMetryHelper::LoadRectangleMesh(1000.0f);
		gameobject->SetShader(ResourceManager::GetInstance()->Get<GraphicsShader>(L"seatest2.hlsl"));


		gameobject->GetTransform()->SetLocalPosition(vec3(6000.0f, 2000.0f, -2000.0f));
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
		auto& list = core->GetCmdList();
		ResourceManager::GetInstance()->Get<GraphicsShader>(L"final.hlsl")->SetPipelineState();
		shared_ptr<Mesh> mesh = ResourceManager::GetInstance()->Get<Mesh>(L"finalMesh");
		shared_ptr<Material> material = ResourceManager::GetInstance()->Get<Material>(L"finalMaterial");
		material->PushData();
		core->GetBufferManager()->GetTable()->SetGraphicsRootDescriptorTable();
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
void Stage1::ShadowRender()
{

	ResourceManager::GetInstance()->Get<GraphicsShader>(L"depthwrite.hlsl")->SetPipelineState();
	CameraManager::GetInstance()->SetActiveCamera(CameraType::SHADOW);
	CameraManager::GetInstance()->SetData();

	for (auto& ele : _deferredObjects)
	{
		ele->ShaderNoSetRender();
	}

	for (auto& ele : _forwardObjects)
	{
		ele->ShaderNoSetRender();
	}


}

void Stage1::ComputePass()
{
	
	int threadGroupSizeX = 16;
	int threadGroupSizeY = 16;

	// Dispatch 크기 계산
	int dispatchX = (WINDOW_WIDTH + threadGroupSizeX - 1) / threadGroupSizeX;
	int dispatchY = (WINDOW_HEIGHT + threadGroupSizeY - 1) / threadGroupSizeY;

	ResourceManager::GetInstance()->Get<BloomEffect>(L"Bloom")->Render(dispatchX, dispatchY, 1);
};
