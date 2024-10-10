#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "CameraManager.h"
#include "Core.h"
#include "TimeManager.h"
#include "RenderTargets.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "Mesh.h"
#include "Material.h"
#include "BufferPool.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::Init()
{
	for (auto& ele : _forwardObjects)
	{
		ele->Init();
	}

	for (auto& ele : _deferredObjects)
	{
		ele->Init();
	}

	for (auto& ele : _uiObjects)
	{
		ele->Init();
	}
}

void Scene::Run()
{

	core->GetRenderTarget()->ClearDepth();
	CameraControl();

	core->GetGBuffer()->RenderBegin();
	DeferredRender();
	core->GetGBuffer()->RenderEnd();

	core->GetRenderTarget()->RenderBegin();
	FinalRender(); 
	ForwardRender();
	UiObjectRender();
	core->GetRenderTarget()->RenderEnd();
}

void Scene::DeferredRender()
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

void Scene::ForwardRender()
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

void Scene::UiObjectRender()
{
	CameraManager::GetInstance()->SetActiveCamera(CameraType::UI);
	CameraManager::GetInstance()->PushData();

	for (auto& ele : _uiObjects)
	{
		ele->Update();
		ele->Render();
	}
}

void Scene::FinalRender()
{




	auto& list = core->GetCmdLIst();

	list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ResourceManager::GetInstance()->Get<Shader>(L"final.hlsl")->SetPipelineState();
	shared_ptr<Mesh> mesh = ResourceManager::GetInstance()->Get<Mesh>(L"finalMesh");
	shared_ptr<Material> material = ResourceManager::GetInstance()->Get<Material>(L"finalMaterial");


	material->Pushdata();
	core->GetTableHeap()->SetGraphicsRootDescriptorTable(2);
	mesh->Render();

}
void Scene::CameraControl()
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


	CameraManager::GetInstance()->PushData();
}
;

void Scene::LateUpdate()
{
	while (!_reserveAddQueue.empty())
	{
		shared_ptr<GameObject> element = _reserveAddQueue.front();
		AddForwardObject(element);
		_reserveAddQueue.pop();
	}

	while (!_reserveDeleteQueue.empty())
	{
		shared_ptr<GameObject> element = _reserveDeleteQueue.front();
		DeleteGameObject(element);
		_reserveDeleteQueue.pop();
	}
}

void Scene::ReserveAddGameObject(shared_ptr<GameObject> object)
{
	_reserveAddQueue.push(object);
}

void Scene::ReserveDeleteGameObject(shared_ptr<GameObject> object)
{
	_reserveDeleteQueue.push(object);
}

void Scene::AddForwardObject(shared_ptr<GameObject> object)
{
	_forwardObjects.push_back(object);
}

void Scene::AddUiObject(shared_ptr<GameObject> object)
{
	_uiObjects.push_back(object);
}

void Scene::AddDeferredObject(shared_ptr<GameObject> object)
{
	_deferredObjects.push_back(object);
}

void Scene::DeleteGameObject(shared_ptr<GameObject> object)
{
	if (object == nullptr)
		return;

	auto it = std::remove(_forwardObjects.begin(), _forwardObjects.end(), object);
	_forwardObjects.erase(it, _forwardObjects.end());
}


