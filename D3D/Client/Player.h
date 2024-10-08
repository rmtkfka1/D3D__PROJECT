#pragma once
#include "HireacyObject.h"

class ThirdPersonCamera;
class Terrain;

class Player :public HireacyObject
{
public:
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
	void CollisonUpdate();
	void CollisonRotate(vec3 look ,vec3 dir);

	shared_ptr<Terrain> _terrain;
	shared_ptr<ThirdPersonCamera> _camera;

	float _speed = 200.0f;
	bool _collisionDected = false;
	vec3 _dir = vec3(1.0f,0,0);


	float _totalAngle;
	float _sumAngle;
	float _addAngle =10.0f;

	
};

