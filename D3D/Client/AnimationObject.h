#pragma once
#include "GameObject.h"

class Transform;
\
class AnimationObject :public GameObject
{
private:
	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void ShaderNoSetRender() override;

	virtual shared_ptr<Transform> GetTransform() override;


public:
	virtual void OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider>other);
	virtual void OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other);

private:
	shared_ptr<Model> _model;
	shared_ptr<Transform> _transform;


};

