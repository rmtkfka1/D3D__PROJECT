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
	void Move();
	void GetShoted();

	void SetPlayer(shared_ptr<Player> player) { _player = player; }

	virtual void OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider>other);
	virtual void OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other);


	shared_ptr<Player> _player;
	float _speed = 200.0f;

	void CollisonUpdate();
	vec3 CalculateNextDir(vec3 direction, float degree);
	void StartCollisionRotation(vec3 direction, int i);
	void CollisonRotate(vec3 look, vec3 dir, float angle, vec3 rotationAxis);
	void AvoidCollision(shared_ptr<BaseCollider>& collider, shared_ptr<BaseCollider>& other);

	float _rotationSpeed = 120.0f;
	bool _collisionDected = false;
	vec3 _dir = vec3(1.0f, 0, 0);
	vec3 _look;
	float _angle;
	vec3 _rotationAxis;
	float _addAngle = 0;


	bool _blocked = false;

};

