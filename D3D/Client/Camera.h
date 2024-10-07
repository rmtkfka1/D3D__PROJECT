#pragma once

class Player;

enum CameraType
{
	FIRSTVIEW,
	THIRDVIEW,
	OBSERVE,
	NONE
};

/*************************
*                        *
*       Camera           *
*                        *
**************************/

class Camera
{


public:
	Camera(CameraType type);
	virtual ~Camera();

	void GenViewMatrix();
	void GenProjMatrix();
	void PushData();

	virtual void Update();
	virtual void Rotate(const vec3& offset) {};
	void AddMove(const vec3& shift);
	void SetCameraPos(const vec3& pos) { _cameraPos = pos; }
	vec3 GetCameraPos() { return _cameraPos; }
	vec3 GetCameraLook() { return _cameraLook; }
	CameraType GetCameraType() { return _type; }

protected:
	CameraType _type = CameraType::NONE;
	CameraParams _params;

	vec3 _cameraPos = vec3(0,0,0);
	vec3 _cameraLook = vec3(0, 0, 1.0f);
	vec3 _cameraUp =vec3(0,1.0f,0);
	vec3 _cameraRight =vec3(1.0f,0,0);

	vec3 _offset = vec3(0, 3.0f, 20.0f);

	float _near = 0.1f;
	float _far = 5000.f;
	float _fov = XM_PI / 3.f;

};


/*************************
*                        *
*  ThirdPersonCamera     *
*                        *
**************************/

class ThirdPersonCamera :public Camera
{

public:
	ThirdPersonCamera();
	virtual ~ThirdPersonCamera();

	virtual void Update() override;
	virtual void Rotate(const shared_ptr<Player>& player);


private:

};


class ObserveCamera :public Camera
{

public:
	ObserveCamera();
	virtual ~ObserveCamera();

	virtual void Update() override;
private:
	void KeyUpdate();
	void MouseUpdate();

private:
	float _cameraSpeed = 50.0f;
};

