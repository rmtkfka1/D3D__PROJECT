#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "CameraManager.h"
#include "Core.h"
#include "TimeManager.h"
Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::Init()
{
	for (auto& ele : _gameObjects)
	{
		ele->Init();
	}
}

void Scene::Run()
{
	GameObjectRender();
	UiObjectRender();
}

void Scene::GameObjectRender()
{

	static CameraType type = CameraType::THIRDVIEW;

	if (KeyManager::GetInstance()->GetButton(KEY_TYPE::ONE))
	{
		type = CameraType::OBSERVE;
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

	static int i = 0;

	for (auto& ele : _gameObjects)
	{
		ele->Update();

		if (ele->GetFrustumCuling())
		{
			if (CameraManager::GetInstance()->GetActiveCamera()->IsInFrustum(ele->GetCollider()) == false)
			{
				continue;
			}
		}

		i++;
		ele->Render();
	}

	int temp = i;
	TimeManager::GetInstance()->_objectCount = temp;
	i = 0;
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

void Scene::LateUpdate()
{
	while (!_reserveAddQueue.empty())
	{
		shared_ptr<GameObject> element = _reserveAddQueue.front();
		AddGameObject(element);
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

void Scene::AddGameObject(shared_ptr<GameObject> object)
{
	_gameObjects.push_back(object);
}

void Scene::AddUiObject(shared_ptr<GameObject> object)
{
	_uiObjects.push_back(object);
}

void Scene::DeleteGameObject(shared_ptr<GameObject> object)
{
	if (object == nullptr)
		return;

	auto it = std::remove(_gameObjects.begin(), _gameObjects.end(), object);
	_gameObjects.erase(it, _gameObjects.end());
}


