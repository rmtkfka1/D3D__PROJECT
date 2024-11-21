#include "pch.h"
#include "Game.h"
#include "Core.h"
#include "Mesh.h"
#include "Shader.h"
#include "BufferPool.h"
#include "Texture.h"
#include "GameObject.h"
#include "Material.h"
#include "Transform.h"
#include "SceneManager.h"
#include "KeyManager.h"
#include "TimeManager.h"
#include "CollisonManager.h"
#include "BoxCollider.h"
#include "CameraManager.h"
#include "ResourceManager.h"
#include "Material.h"
void Game::Init(HWND hwnd)
{

	_hwnd = hwnd;

	//다이렉트 코어 초기화
	core->Init(hwnd, true, true);
	ResourceManager::GetInstance()->Init();
	KeyManager::GetInstance()->Init(hwnd);
	SceneManager::GetInstance()->ChangeScene(SceneType::StartScene);
	TimeManager::GetInstance()->Init(hwnd);
	CollisonManager::GetInstance()->Init();


}

void Game::Run()
{
	Update();
	Render();

}

void Game::Update()
{


	TimeManager::GetInstance()->Update();
	KeyManager::GetInstance()->Update();

	if (KeyManager::GetInstance()->GetButtonDown(KEY_TYPE::Z))
	{
		BoxCollider::brender = !BoxCollider::brender;
	}


}

void Game::Render()
{
	COMPUTE->PrePareExcute();
	GRAPHICS->RenderBegin();
	SceneManager::GetInstance()->Run();
	GRAPHICS->RenderEnd();
	SceneManager::GetInstance()->LateUpdate();
	CollisonManager::GetInstance()->Update();

}
