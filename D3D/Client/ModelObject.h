#pragma once
#include "GameObject.h"

class Model;
class Transform;

class ModelObject : public GameObject
{

public:
	ModelObject();
	virtual ~ModelObject();

	virtual void Init() override;;
	virtual void Update() override;
	virtual void Render() override;

	void SetModel(shared_ptr< Model> model) { _model = model; }
	shared_ptr<Transform> GetTransform() { return _transform; }

public:
	shared_ptr<Model> _model;
	shared_ptr<Transform> _transform;

};

