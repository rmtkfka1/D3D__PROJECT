#include "pch.h"
#include "LightManager.h"
#include "Core.h"
#include "BufferPool.h"
#include "CameraManager.h"
#include "Transform.h"
#include "Player.h"
#include "TransformTree.h"
void LightManager::PushLight(Light light)
{
	_lightParmas.light[_lightParmas.lightCount] = light;
	_lightParmas.lightCount++;
}

void LightManager::SetData()
{
	Update();
	core->GetBufferManager()->GetLIghtBufferPool()->SetData(0,&_lightParmas, sizeof(LightParams));
}

void LightManager::Update()
{


	for (int i = 0; i < _lightParmas.lightCount; ++i)
	{

		if (_lightParmas.light[i].material.lightType == static_cast<int32>(LIGHT_TYPE::SPOT_LIGHT))
		{
			_lightParmas.light[i].direction = _player->GetTransform()->GetLook();
			_lightParmas.light[i].position = _player->GetTransform()->GetLocalPosition();

		}

	}

	_lightParmas.eyeWorldPos = _player->GetTransform()->GetLocalPosition();

}
