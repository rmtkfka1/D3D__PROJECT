#pragma once
#include "GameObject.h"
#include "BaseCollider.h"

class Model;
class Transform;
class BaseCollider;
class GraphicsShader;

class ModelObject : public GameObject
{

public:
	ModelObject(PlayerType type);
	virtual ~ModelObject();

	virtual void Init() override;;
	virtual void Update() override;
	virtual void Render() override;
	virtual shared_ptr<Transform> GetTransform() override { return _transform; }

	void SetModel(shared_ptr< Model> model) { _model = model; }

	void AddBoxColliderWithModel(string name, ColliderBehave behave, vec3 offsetSize = vec3(0, 0, 0), vec3 offsetCeneter = vec3(0, 0, 0));
	void AddSphereColliderWithModel(string name, ColliderBehave behave,float offsetSize = 0, vec3 offsetCeneter = vec3(0, 0, 0));

	virtual void OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider>other);
	virtual void OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other);

protected:
	shared_ptr<Model> _model;
	shared_ptr<Transform> _transform;

};

