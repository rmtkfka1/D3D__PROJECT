#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Stage1.h"
#include "StartScene.h"

void SceneManager::Init()
{
	if (_scene)
		_scene->Init();

}


void SceneManager::LateUpdate()
{
	if (_scene)
		_scene->LateUpdate();
}


void SceneManager::Run()
{
	if (_scene)
		_scene->Run();
}


void SceneManager::ChangeScene(SceneType type)
{

	if (_sceneType == type)
		return;


	switch (type)
	{
	case::SceneType::StartScene:
		_scene = make_shared<StartScene>();
		break;
	case SceneType::STAGE1:
		_scene = make_shared<Stage1>();
		break;
	default:
		break;
	}

	_sceneType = type;
	_scene->Init();
}


