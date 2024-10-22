#pragma once
#include "ModelObject.h"
class EnemyBullet : public ModelObject
{
public:

	using super = ModelObject;

	EnemyBullet();
	virtual ~EnemyBullet();

	virtual void Init() override;;
	virtual void Update() override;
	virtual void Render() override;

	virtual void OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider>other);
	virtual void OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other);


public:
	void Delete();

	float _sumTime=0;
	vec3 _direction;
};

