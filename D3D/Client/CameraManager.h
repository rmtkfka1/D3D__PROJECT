#pragma once
#include "Camera.h"

class CameraManager
{
public:
    static CameraManager* GetInstance()
    {
        static CameraManager instance;
        return &instance;
    }
  
    void AddCamera(CameraType type, std::shared_ptr<Camera> camera){_cameras[type] = camera;}
    shared_ptr<Camera> GetCamera(CameraType type);
    void SetActiveCamera(CameraType type);
    shared_ptr<Camera> GetActiveCamera() {return _activeCamera;}

    void PushData();

private:
    std::unordered_map<CameraType, std::shared_ptr<Camera>> _cameras;
    std::shared_ptr<Camera> _activeCamera = nullptr;
};