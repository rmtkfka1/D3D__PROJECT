#pragma once
#include "CustomObject.h"

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

	shared_ptr<Terrain> _terrain;
	float _speed =30.0f;
};

