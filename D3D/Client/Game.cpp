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


void Game::Init(HWND hnwd)
{
	//���̷�Ʈ �ھ� �ʱ�ȭ
	core->Init(hnwd, true, true);
	KeyManager::GetInstance()->Init(hnwd);
	SceneManager::GetInstance()->ChangeScene(SceneType::STAGE1);

}

void Game::Run()
{
	Update();
	Render();
}

void Game::Update()
{
	//Ű�Է� ó��
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
