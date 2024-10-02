#pragma once
#include "CustomObject.h"

class ThirdPersonCamera;
class Terrain;

class Player :public CustomObject
{

	using Super = CustomObject;
	
public:
	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;

	void SetTerrain(shared_ptr<Terrain> terrian) { _terrain = terrian; }
private:

	void MoveUpdate();
	void RotateUpdate();
	void CameraPushData();

	shared_ptr<Terrain> _terrain;
	shared_ptr<ThirdPersonCamera> _camera;
	float _speed =30.0f;
};

