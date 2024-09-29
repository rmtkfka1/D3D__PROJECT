#pragma once
 
class GameObject;
class CustomObject;


enum CameraMode
{
	OBSERVER,
	THIRD_PERSON
};

struct Info
{
	vec3  cameraPos = vec3(0, 0, -15.0f);
	vec3  cameraLook = vec3(0, 0, 1.0f);
	vec3  cameraUp = vec3(0, 1.0f, 0);
	vec3  cameraRight = vec3(1, 0, 0);
};


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



private:
	void RegenerateMatrix(vec3 cameraPos, vec3 cameraLook, vec3 CameraUp);
	void MouseUpdate();
	void PlayerUpdate();
	void ThirdCameraUpdate();

	void ObserverUpdate();
	void ChangeCamera(CameraMode mode);
private:


	float _near = 0.1f;
	float _far = 5000.f;
	float _fov = XM_PI / 3.f;
	float _scale = 1.f;

	///OBSERVER
	Info _observer;


	//THRID_PERSON
	Info _thrid;
	vec3  _offset = vec3(0, 10.0f, -10.0f);
	float _TimeLag = 0.5f;

	
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
	static bool SetCursorMode;
	CameraMode _mode = THIRD_PERSON;
	BoundingFrustum _boundingFrsutum;

};

