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
private:
	void AnimateUpdate();
	void MoveUpdate();
	void RotateUpdate();
	void CameraPushData();

	shared_ptr<Terrain> _terrain;
	shared_ptr<ThirdPersonCamera> _camera;
	float _speed = 200.0f;
	
};

