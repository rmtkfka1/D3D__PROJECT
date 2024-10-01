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
#include "CameraManager.h"
#include "TimeManager.h"


void Game::Init(HWND hwnd)
{

	_hwnd = hwnd;

	//다이렉트 코어 초기화
	core->Init(hwnd, true, true);
	KeyManager::GetInstance()->Init(hwnd);
	SceneManager::GetInstance()->ChangeScene(SceneType::STAGE1);
	TimeManager::GetInstance()->Init(hwnd);
	CameraManager::GetInstance()->Init();

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
	CameraManager::GetInstance()->Update();

}

void Game::Render()
{
	core->RenderBegin();

	CameraParams param;
	param.matView = CameraManager::S_MatView;
	param.matView = CameraManager::S_MatProjection;

	core->GetConstantBufferPool2()->PushData(&param, sizeof(CameraParams));
	core->GetTableHeap()->SetGraphicsRootDescriptorTable(0);


	SceneManager::GetInstance()->Run();
	SceneManager::GetInstance()->LateUpdate();
	core->RenderEnd();
}
