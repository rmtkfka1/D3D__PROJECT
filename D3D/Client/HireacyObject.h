#pragma once
#include "GameObject.h"
#include "BaseCollider.h"
class Model;
class TransformTree;
class Transform;


class HireacyObject :public GameObject
{

public:
	HireacyObject();
	virtual ~HireacyObject();

	virtual void Init() override;;
	virtual void Update() override;
	virtual void Render() override;

	void AddCollider(string name, ColliderType type, vec3 offsetSize= vec3(0, 0, 0), vec3 offsetCeneter=vec3(0,0,0));
	void AddBoxCollider(string name, vec3 size, vec3 center);

	virtual shared_ptr<Transform> GetTransform() override;
	void SetModel(shared_ptr< Model> model);

	virtual void OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider>other);
	virtual void OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other);

public:
	shared_ptr<Model> _model;
	shared_ptr<TransformTree> _transform;	
	vector<shared_ptr<BaseCollider>> _colliders;
};

