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


	shared_ptr<CustomObject> object= make_shared<CustomObject>();
	object->SetMesh(GeoMetryHelper::LoadRectangleBox(50.0f));
	object->GetTransform()->SetLocalPosition(vec3(0, 0, 0.0f));
	object->GetMaterial()->SetDiffuseTexture(ResourceManager::GetInstance()->Load<Texture>(L"1.jpg"));
	object->SetShader(ResourceManager::GetInstance()->Load<Shader>(L"uishader.hlsl"));

	AddGameObject(object, RenderingType::Ui);

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

	// Normalize the mouse position based on the client area size
	float normalizedX = static_cast<float>(mousePos.x) / clientWidth;
	float normalizedY = static_cast<float>(mousePos.y) / clientHeight;

	// Check if the left mouse button is pressed
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		// Check if the normalized coordinates are within a certain range
		// For example, you can check if it's within a specific area to trigger scene change
		if (normalizedX >= 0.4f && normalizedX <= 0.6f && normalizedY >= 0.0f && normalizedY <= 1.0f)
		{
			SceneManager::GetInstance()->ChangeScene(SceneType::STAGE1);
		}
	}

	// Display mouse position in the window title
	WCHAR wchTxt[64];
	swprintf_s(wchTxt, 64, L"x: %d, y: %d (Normalized: %.2f, %.2f)", mousePos.x, mousePos.y, normalizedX, normalizedY);
	SetWindowText(core->GetWindowHandle(), wchTxt);


}

void StartScene::Run()
{
	core->GetRenderTarget()->ClearDepth();
	core->GetRenderTarget()->RenderBegin();
	UiRender();
	core->GetRenderTarget()->RenderEnd();
}

void StartScene::LateUpdate()
{
	MouseUpdate();


	if (KeyManager::GetInstance()->GetButtonDown(KEY_TYPE::SPACE))
	{
		SceneManager::GetInstance()->ChangeScene(SceneType::STAGE1);
	}
}

void StartScene::UiRender()
{
	CameraManager::GetInstance()->SetActiveCamera(CameraType::UI);
	CameraManager::GetInstance()->SetData();

	for (auto& ele : _uiObjects)
	{
		ele->Update();
		ele->Render();
	}
}

