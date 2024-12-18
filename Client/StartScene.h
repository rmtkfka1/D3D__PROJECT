#pragma once
#include "Scene.h"

class CustomObject;


class StartScene : public Scene
{

public:

	virtual void Init() override;
	virtual void Run() override;
	virtual void LateUpdate() override;

private:
	void BulidObject();
	void BulidCamera();
	void MouseUpdate();
	void UiRender();

public:

	shared_ptr<CustomObject> temp;
};

