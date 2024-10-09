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
