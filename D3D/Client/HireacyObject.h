#pragma once
#include "GameObject.h"
#include "BaseCollider.h"
class Model;
class TransformTree;
class Transform;
class GraphicsShader;
class Material;

class HireacyObject :public GameObject
{

public:
	HireacyObject(PlayerType type);
	virtual ~HireacyObject();

	virtual void Init() override;;
	virtual void Update() override;
	virtual void Render() override;


	virtual shared_ptr<Transform> GetTransform() override;

	void AddBoxColliderWithModel(string name, ColliderBehave behave , vec3 offsetSize = vec3(0, 0, 0), vec3 offsetCeneter = vec3(0, 0, 0));
	void AddSphereColliderWithModel(string name, ColliderBehave behave, float offsetSize = 0, vec3 offsetCeneter = vec3(0, 0, 0));

	virtual void OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider>other);
	virtual void OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other);


	void SetModel(shared_ptr< Model> model);
	const shared_ptr<Model>& GetModel() { return _model; }

public:
	shared_ptr<Model> _model;
	shared_ptr<TransformTree> _transform;	

};

