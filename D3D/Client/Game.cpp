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

ULONGLONG _prvFrameCheckTick = 0;
ULONGLONG g_PrvUpdateTick = 0;
DWORD	g_FrameCount = 0;

void Game::Init(HWND hwnd)
{

	_hwnd = hwnd;

	//���̷�Ʈ �ھ� �ʱ�ȭ
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
	SceneManager::GetInstance()->Run();
	SceneManager::GetInstance()->LateUpdate();
	core->RenderEnd();
}
