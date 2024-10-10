#pragma once
#include "ModelObject.h"
class Sphere :public ModelObject
{
public:

	using Super = ModelObject;

	Sphere();
	virtual ~Sphere();

	virtual void Init() override;;
	virtual void Update() override;
	virtual void Render() override;

	virtual void OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider>other);
	virtual void OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other);

};

