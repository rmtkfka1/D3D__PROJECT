#pragma once

class Player;

enum CameraType
{
	FIRSTVIEW,
	THIRDVIEW,
	OBESRVE,
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
	void UpdateShaderVariables();

	virtual void Rotate(const vec3& offset) {};
	void Move(const vec3& shift);

protected:
	CameraType _type = CameraType::NONE;
	CameraParams _params;

	vec3 _cameraPos = vec3(0,0,0);
	vec3 _cameraLook = vec3(0, 0, 1.0f);
	vec3 _cameraUp =vec3(0,1.0f,0);
	vec3 _cameraRight =vec3(1.0f,0,0);

	vec3 _offset = vec3(0, 0, -20.0f);

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
	~ThirdPersonCamera();

	virtual void Rotate(const shared_ptr<Player>& player);

private:

};
