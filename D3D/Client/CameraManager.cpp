#include "pch.h"
#include "CameraManager.h"

shared_ptr<Camera> CameraManager::GetCamera(CameraType type)
{

    if (_cameras.find(type) != _cameras.end())
    {
        return _cameras[type];
    }

    assert(false);

}
CameraType CameraManager::GetCameraType()
{
    return _activeCamera->GetCameraType();
}


void CameraManager::SetActiveCamera(CameraType type)
{
    if (type == CameraType::OBSERVE)
    {
        shared_ptr<Camera> camera = GetCamera(CameraType::OBSERVE);
        shared_ptr<Camera> thirdCamera = GetCamera(CameraType::THIRDVIEW);
        camera->SetCameraPos(thirdCamera->GetCameraPos()); 
    }

    if (_cameras.find(type) != _cameras.end())
    {
        _activeCamera = _cameras[type];
    }

    else
    {
        assert(false);
    }
}

void CameraManager::PushData()
{
    _activeCamera->PushData();
}
