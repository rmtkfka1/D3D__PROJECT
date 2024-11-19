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

}


void Scene::LateUpdate()
{

	if (_reserveAddQueue.empty() == false || _reserveDeleteQueue.empty() == false)
	{
		core->GetGraphics()->WaitForAllFence();
	}

	while (!_reserveAddQueue.empty())
	{
		auto& [object,type] = _reserveAddQueue.front();
		AddGameObject(object,type);
		_reserveAddQueue.pop();
	}

	while (!_reserveDeleteQueue.empty())
	{
		auto& [object, type] = _reserveDeleteQueue.front();
		DeleteGameObject(object, type);
		_reserveDeleteQueue.pop();
	}
}

void Scene::ReserveAddGameObject(const shared_ptr<GameObject>& object,RenderingType type)
{
	_reserveAddQueue.push(make_pair(object,type));

}

void Scene::ReserveDeleteGameObject(const shared_ptr<GameObject>& object,RenderingType type)
{
	_reserveDeleteQueue.push(make_pair(object, type));
}




void Scene::AddGameObject(const shared_ptr<GameObject>& object, RenderingType type)
{
	switch (type)
	{
	case RenderingType::Deferred:
		_deferredObjects.push_back(object);
		break;
	case RenderingType::Forward:
		_forwardObjects.push_back(object);	
		break;
	case RenderingType::Ui:
		_uiObjects.push_back(object);	
		break;
	default:
		break;
	}

}



void Scene::DeleteGameObject(const shared_ptr<GameObject>& object, RenderingType type)
{
	if (object == nullptr)
		return;

	switch (type)
	{
	case RenderingType::Deferred:
	{
		auto it = std::remove(_deferredObjects.begin(), _deferredObjects.end(), object);
		_deferredObjects.erase(it, _deferredObjects.end());
	}
		break;
	case RenderingType::Forward:
	{
		auto it = std::remove(_forwardObjects.begin(), _forwardObjects.end(), object);
		_forwardObjects.erase(it, _forwardObjects.end());
	}
		break;
	case RenderingType::Ui:
	{
		auto it = std::remove(_uiObjects.begin(), _uiObjects.end(), object);
		_uiObjects.erase(it, _uiObjects.end());
	}
		break;
	default:
		break;
	}



}


