#pragma once
#include "Scene.h"

class ThirdPersonCamera;

class Stage1 :public Scene
{
public:

	Stage1();
	virtual ~Stage1();

	virtual void Init() override;
	virtual void Run() override;
	virtual void LateUpdate() override;

private:
	void BulidObject();

};

