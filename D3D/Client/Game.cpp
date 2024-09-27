#include "pch.h"
#include "Game.h"
#include "Core.h"
#include "Mesh.h"
#include "Shader.h"
#include "BufferPool.h"
#include "Texture.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Transform.h"
#include "SceneManager.h"


void Game::Init(HWND hnwd)
{
	//���̷�Ʈ �ھ� �ʱ�ȭ
	core->Init(hnwd, true, true);

	SceneManager::GetInstance()->ChangeScene(SceneType::STAGE1);

	
}

void Game::Run()
{
	core->RenderBegin();
	SceneManager::GetInstance()->Run();
	SceneManager::GetInstance()->LateUpdate();
	core->RenderEnd();
}
