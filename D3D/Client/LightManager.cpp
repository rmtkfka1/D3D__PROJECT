//#include "pch.h"
//#include "LightManager.h"
//#include "Core.h"
//#include "BufferPool.h"
//
//void LightManager::PushLight(Light light)
//{
//	_lightParmas.light[_lightParmas.lightCount] = light;
//	_lightParmas.lightCount++;
//}
//
//void LightManager::SetData()
//{
//
//	Update();
//	core->GetConstantBuffer(CBV_REGISTER::b0)->SetData(&_lightParmas, sizeof(LightParams));
//}
//
//void LightManager::Update()
//{
//
//	for (int i = 0; i < _lightParmas.lightCount; ++i)
//	{
//
//		if (_lightParmas.light[i].material.lightType == static_cast<int32>(LIGHT_TYPE::SPOT_LIGHT))
//		{
//			_lightParmas.light[i].direction = CameraManager::GetInstance()->GetLook();
//			_lightParmas.light[i].position = CameraManager::GetInstance()->GetWorldPos();
//		}
//
//	}
//
//	_lightParmas.eyeWorldPos = CameraManager::GetInstance()->GetWorldPos();
//
//}
