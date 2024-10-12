#pragma once
#include "Scene.h"

class Player;
class BoxCollider;


class Stage1 :public Scene
{
public:

	Stage1();
	virtual ~Stage1();

	virtual void Init() override;
	virtual void Run() override;
	virtual void LateUpdate() override;

private:
	void BulidLight();
	void BulidCamera();
	void BulidDeferred();
	void BulidForward();

public:
	shared_ptr<Player> _player;



private:
	void DeferredRender();
	void ForwardRender();
	void UiObjectRender();
	void FinalRender();
	void BoundingBoxRender();
	void CameraControl();
};

