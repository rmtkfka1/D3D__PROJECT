#pragma once
#include "Scene.h"
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

};

