#pragma once
#include "CustomObject.h"


class Player :public CustomObject
{

	using Super = CustomObject;
	
public:
	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;

private:

	void MoveUpdate();

	float _speed =3.0f;
};

