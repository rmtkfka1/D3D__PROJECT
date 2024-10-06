#pragma once
#include "GameObject.h"
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
	virtual shared_ptr<Transform> GetTransform() override;
	void SetModel(shared_ptr< Model> model);
	

public:
	shared_ptr<Model> _model;
	shared_ptr<TransformTree> _transform;
};

