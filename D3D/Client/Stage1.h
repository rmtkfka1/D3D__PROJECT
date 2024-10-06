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
	void BulidObject();

public:
	//temp
	shared_ptr<Player> _player;
	shared_ptr<BoxCollider> _box;
};

