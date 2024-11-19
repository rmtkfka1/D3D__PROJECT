#include "pch.h"
#include "StartScene.h"
#include "CameraManager.h"
#include "GameObject.h"
#include "Core.h"
#include "Camera.h"
#include "RenderTargets.h"
#include "CustomObject.h"
#include "ResourceManager.h"
#include "GeoMetryHelper.h"
#include "Material.h"
#include "Texture.h"
#include "Shader.h"
#include "Transform.h"
#include "Mesh.h"
#include "SceneManager.h"


void StartScene::Init()
{
	BulidObject();
	BulidCamera();
	Scene::Init();
}
void StartScene::BulidObject()
{

	{
		shared_ptr<CustomObject> object = make_shared<CustomObject>();
		object->SetMesh(GeoMetryHelper::LoadRectangleMesh(1.0f));
		object->GetTransform()->SetLocalPosition(vec3(0, 0, 30.0f));
		object->GetTransform()->SetLocalScale(vec3(1000.0f, 550.0f, 0.0f));
		object->GetMaterial()->SetDiffuseTexture(ResourceManager::GetInstance()->Load<Texture>(L"startScene.jpg"));
		object->SetShader(ResourceManager::GetInstance()->Load<GraphicsShader>(L"uishader.hlsl"));
		AddGameObject(object, RenderingType::Ui);
	}

	{
		shared_ptr<CustomObject> object = make_shared<CustomObject>();
		object->SetMesh(GeoMetryHelper::LoadRectangleMesh(1.0f));
		object->GetTransform()->SetLocalPosition(vec3(700.0f, 0, -0.5f));
		object->GetTransform()->SetLocalScale(vec3(300.0f, 300.0f, 0.0f));
		object->GetMaterial()->SetInt(0, 0);
		object->GetMaterial()->SetDiffuseTexture(ResourceManager::GetInstance()->Load<Texture>(L"start.jpg"));
		object->SetShader(ResourceManager::GetInstance()->Load<GraphicsShader>(L"uishaderclip.hlsl"));
		AddGameObject(object, RenderingType::Ui);

	}

	{
		shared_ptr<CustomObject> object = make_shared<CustomObject>();
		object->SetMesh(GeoMetryHelper::LoadRectangleMesh(1.0f));
		object->GetTransform()->SetLocalPosition(vec3(800.0f, -300.0f, -0.5f));
		object->GetTransform()->SetLocalScale(vec3(100.0f, 100.0f, 0.0f));
		object->GetMaterial()->SetInt(0, 1);
		object->GetMaterial()->SetDiffuseTexture(ResourceManager::GetInstance()->Load<Texture>(L"Info-Button.png"));
		object->SetShader(ResourceManager::GetInstance()->Load<GraphicsShader>(L"uishaderclip.hlsl"));
		AddGameObject(object, RenderingType::Ui);
	}

	{
		shared_ptr<CustomObject> object = make_shared<CustomObject>();
		temp = object;
		object->SetMesh(GeoMetryHelper::LoadRectangleMesh(1.0f));
		object->GetTransform()->SetLocalPosition(vec3(-200.0f, 0, 29.0f));
		object->GetTransform()->SetLocalScale(vec3(600.0f, 400.0f, 0.0f));
		object->SetFrustumCuling(false);
		object->GetMaterial()->SetDiffuseTexture(ResourceManager::GetInstance()->Load<Texture>(L"info.png"));
		object->SetShader(ResourceManager::GetInstance()->Load<GraphicsShader>(L"uishader.hlsl"));
		AddGameObject(object, RenderingType::Ui);
	}

}

void StartScene::BulidCamera()
{

	shared_ptr<ObserveCamera> observeCamera = make_shared<ObserveCamera>();
	CameraManager::GetInstance()->AddCamera(CameraType::OBSERVE, observeCamera);

	shared_ptr<UiCamera> uicamera = make_shared<UiCamera>();
	CameraManager::GetInstance()->AddCamera(CameraType::UI, uicamera);
}

void StartScene::MouseUpdate()
{
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(core->GetWindowHandle(), &mousePos);

	RECT clientRect;
	GetClientRect(core->GetWindowHandle(), &clientRect);
	int clientWidth = clientRect.right - clientRect.left;
	int clientHeight = clientRect.bottom - clientRect.top;

	float normalizedX = static_cast<float>(mousePos.x) / clientWidth;
	float normalizedY = static_cast<float>(mousePos.y) / clientHeight;


	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{

		if (normalizedX >= 0.76f && normalizedX <= 0.95f && normalizedY >= 0.43f && normalizedY <= 0.62f)
		{
			SceneManager::GetInstance()->ChangeScene(SceneType::STAGE1);
		}

		if (normalizedX >= 0.90 && normalizedX <= 1.0f && normalizedY >= 0.74f && normalizedY <= 0.90f)
		{
			temp->SetFrustumCuling(true);
		}

		if (normalizedX >= 0.61f && normalizedX <= 0.72f && normalizedY >= 0.14f && normalizedY <= 0.52f)
		{
			temp->SetFrustumCuling(false);
		}


	}


	//WCHAR wchTxt[64];
	//swprintf_s(wchTxt, 64, L"x: %d, y: %d (Normalized: %.2f, %.2f)", mousePos.x, mousePos.y, normalizedX, normalizedY);
	//SetWindowText(core->GetWindowHandle(), wchTxt);
	 
}

void StartScene::Run()
{
	core->GetGraphics()->GetRenderTarget()->ClearDepth();
	core->GetGraphics()->GetRenderTarget()->RenderBegin();
	UiRender();
	core->GetGraphics()->GetRenderTarget()->RenderEnd();
}

void StartScene::LateUpdate()
{
	MouseUpdate();

}

void StartScene::UiRender()
{
	CameraManager::GetInstance()->SetActiveCamera(CameraType::UI);
	CameraManager::GetInstance()->SetData();

	for (auto& ele : _uiObjects)
	{
		if (ele->GetFrustumCuling() == true)
		{
			ele->Update();
			ele->Render();
		}
	}
}

