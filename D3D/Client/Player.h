#pragma once
#include "HireacyObject.h"

class ThirdPersonCamera;
class Terrain;

class Player :public HireacyObject
{
public:

	Player();
	virtual ~Player();

	using Super = HireacyObject;

	virtual void Init() override;;
	virtual void Update() override;
	virtual void Render() override;

	void SetThirdPersonCamera(shared_ptr<ThirdPersonCamera> camera) { _camera = camera; }
	void SetTerrain(shared_ptr<Terrain> terrian) { _terrain = terrian; }

	virtual void OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider>other);
	virtual void OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other);

private:
	void AnimateUpdate();
	void MoveUpdate();
	void RotateUpdate();
	void CameraPushData();
	void Shot();
	void CollisonUpdate();
	vec3 CalculateNextDir(vec3 direction,float degree);
	void StartCollisionRotation(vec3 direction,int i);
	void CollisonRotate(vec3 look ,vec3 dir ,float angle , vec3 rotationAxis);

	void AvoidCollision(shared_ptr<BaseCollider>& collider, shared_ptr<BaseCollider>& other);

	shared_ptr<Terrain> _terrain;
	shared_ptr<ThirdPersonCamera> _camera;

	float _speed = 600.0f;


	float _rotationSpeed = 120.0f;
	bool _collisionDected = false;
	vec3 _dir = vec3(1.0f,0,0);
	vec3 _look;
	float _angle;
	vec3 _rotationAxis;
	float _addAngle = 0;


	
};

