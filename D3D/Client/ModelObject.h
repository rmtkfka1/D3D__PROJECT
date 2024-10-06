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
	virtual shared_ptr<Transform> GetTransform() override { return _transform; }
	void SetModel(shared_ptr< Model> model) { _model = model; }


public:
	shared_ptr<Model> _model;
	shared_ptr<Transform> _transform;

};

