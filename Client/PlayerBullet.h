#pragma once
#include "ModelObject.h"
class PlayerBullet : public ModelObject
{
public:

	using super = ModelObject;

	PlayerBullet();
	virtual ~PlayerBullet();

	virtual void Init() override;;
	virtual void Update() override;
	virtual void Render() override;

	virtual void OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider>other);
	virtual void OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other);

	void Delete();

public:
	vec3 _direction;
	float _sumTime = 0;
};

