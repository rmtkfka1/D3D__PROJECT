#include "pch.h"
#include "LightManager.h"
#include "Core.h"
#include "BufferPool.h"
#include "CameraManager.h"
#include "Player.h"
#include "Transform.h"
void LightManager::PushLight(Light light)
{
	_lightParmas.light[_lightParmas.lightCount] = light;
	_lightParmas.lightCount++;
}

void LightManager::SetData()
{
	Update();
	core->GetLIghtBufferPool()->SetData(&_lightParmas, sizeof(LightParams));
}

void LightManager::Update()
{


	for (int i = 0; i < _lightParmas.lightCount; ++i)
	{

		if (_lightParmas.light[i].material.lightType == static_cast<int32>(LIGHT_TYPE::SPOT_LIGHT))
		{
			_lightParmas.light[i].direction = CameraManager::GetInstance()->GetActiveCamera()->GetCameraLook();
			_lightParmas.light[i].position = _player->GetTransform()->GetLocalPosition();
		}

	}

	_lightParmas.eyeWorldPos = _player->GetTransform()->GetLocalPosition();

}
