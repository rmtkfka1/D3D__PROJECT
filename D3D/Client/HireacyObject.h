#pragma once
#include "GameObject.h"
#include "BaseCollider.h"
class Model;
class TransformTree;
class Transform;
class Shader;


class HireacyObject :public GameObject
{

public:
	HireacyObject(PlayerType type);
	virtual ~HireacyObject();

	virtual void Init() override;;
	virtual void Update() override;
	virtual void Render() override;

	virtual void AddCollider(string name, ColliderType type, vec3 offsetSize= vec3(0, 0, 0), vec3 offsetCeneter=vec3(0,0,0)) override;
	virtual void AddBoxCollider(string name, vec3 size, vec3 center) override;

	virtual shared_ptr<Transform> GetTransform() override;
	void SetModel(shared_ptr< Model> model);
	void SetShader(shared_ptr<Shader> shader);

	virtual void OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider>other);
	virtual void OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other);

public:
	shared_ptr<Model> _model;
	shared_ptr<TransformTree> _transform;	

};

