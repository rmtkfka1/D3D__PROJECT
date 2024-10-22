#pragma once

class Player;
class BaseCollider;

enum CameraType
{
	FIRSTVIEW,
	THIRDVIEW,
	OBSERVE,
	UI,
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

	virtual void GenViewMatrix() =0; 
	virtual void GenProjMatrix() =0; 
	virtual void GenBoundingFrustum() =0; 

	virtual void SetData();

	bool IsInFrustum(shared_ptr<BaseCollider>& collider);

	virtual void Update() = 0;

	void AddMove(const vec3& shift);
	void SetCameraPos(const vec3& pos) { _cameraPos = pos; }
	void SetCameraLook(const vec3& look) { _cameraLook = look; }
	vec3 GetCameraPos() { return _cameraPos; }
	vec3 GetCameraLook() { return _cameraLook; }
	CameraType GetCameraType() { return _type; }

protected:
	BoundingFrustum _boundingFrsutum;
	CameraType _type = CameraType::NONE;
	CameraParams _params;

	vec3 _cameraPos = vec3(0,0,0);
	vec3 _cameraLook = vec3(0, 0, 1.0f);
	vec3 _cameraUp =vec3(0,1.0f,0);
	vec3 _cameraRight =vec3(1.0f,0,0);

	float _near = 1.0f;
	float _far = 10000.f;
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

	virtual void GenViewMatrix() override;
	virtual void GenProjMatrix() override;
	virtual void GenBoundingFrustum() override;
	virtual void Update() override;

	void Rotate(const shared_ptr<Player>& player);
	void Animation();
	void ShakeOn() { _animationflag = true; }

private:
	vec3 _shake = { 0,0,0 };
	bool _animationflag = false;
	float _elaspedTime = 0;
	vec3 _offset = vec3(0, 3.0f, 20.0f);
};

/*************************
*                        *
*     observeCamera      *
*                        *
**************************/


class ObserveCamera :public Camera
{

public:
	ObserveCamera();
	virtual ~ObserveCamera();


private:

	virtual void GenViewMatrix() override;
	virtual void GenProjMatrix() override;
	virtual void GenBoundingFrustum() override;
	virtual void Update() override;

	void KeyUpdate();
	void MouseUpdate();

private:
	float _cameraSpeed = 50.0f;


};

/*************************
*                        *
*       uiCamera         *
*                        *
**************************/

class UiCamera :public Camera
{

public:
	UiCamera();
	virtual ~UiCamera();

private:
	virtual void GenViewMatrix() override;
	virtual void GenProjMatrix() override;
	virtual void GenBoundingFrustum() override;
	virtual void Update() override;

private:
	float _cameraSpeed = 50.0f;


};