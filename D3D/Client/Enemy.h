#pragma once

#include "HireacyObject.h"

class BaseCollider;
class Player;

class Enemy :public HireacyObject
{
public:

	using Super = HireacyObject;

	Enemy();
	virtual ~Enemy();

	virtual void Init() override;;
	virtual void Update() override;
	virtual void Render() override;

	void Animation();
	void LookatPlayer();
	void Shot();
	void SetPlayer(shared_ptr<Player> player) { _player = player; }

	virtual void OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider>other);
	virtual void OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other);


	shared_ptr<Player> _player;

};

