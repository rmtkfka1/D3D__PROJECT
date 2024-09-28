#pragma once
 
class GameObject;
class CustomObject;


class CameraManager
{
public:

	static CameraManager* GetInstance()
	{
		static CameraManager manager;
		return &manager;
	}

	void Init();
	void Resize(POINT centerScreen) { _centerScreen = centerScreen; }
	void SetPlayer(shared_ptr<CustomObject> object) { _player = object; }
	void Update();

	vec3 GetWorldPos() { return _cameraPos; }
	vec3 GetLook() { return _cameraLook; }

private:
	void MouseUpdate();
	void KeyUpdate();

	float _near = 0.1f;
	float _far = 5000.f;
	float _fov = XM_PI / 3.f;
	float _scale = 1.f;

	vec3 _cameraPos = vec3(0, 0, -1.0f);
	vec3 _cameraLook = vec3(0, 0, 1.0f);
	vec3 _cameraUp = vec3(0, 1.0f, 0);
	vec3 _cameraRight = vec3(1, 0, 0);

	float _cameraYaw{};
	float _cameraPitch{};

	Matrix _matView = {};
	Matrix _matProjection = {};

	float _cameraSpeed = 60.0f;

	POINT _centerScreen{};
	POINT _mousePos{};

private:
	shared_ptr<CustomObject> _player;

public:
	static Matrix S_MatView;
	static Matrix S_MatProjection;
	BoundingFrustum _boundingFrsutum;

};

