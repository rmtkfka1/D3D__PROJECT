#pragma once
#include "GameObject.h"

class Transform;
class Animator;

class AnimationObject :public GameObject
{
public:
	AnimationObject();
	~AnimationObject();


	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void ShaderNoSetRender() override;
	virtual shared_ptr<Transform> GetTransform();

	vector<shared_ptr<Material>>& GetMatrial();
	void SetModel(shared_ptr< Model> model);
	const shared_ptr<Model>& GetModel() { return _model; }

public:
	virtual void OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider>other);
	virtual void OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other);

private:
	shared_ptr<Animator> _animator;
	shared_ptr<Model> _model;
	shared_ptr<Transform> _transform;


};

