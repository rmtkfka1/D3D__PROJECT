#pragma once
#include "GameObject.h"
class Model;
class TransformTree;

class HireacyObject :public GameObject
{

public:
	HireacyObject();
	virtual ~HireacyObject();

	virtual void Init() override;;
	virtual void Update() override;
	virtual void Render() override;

	void SetModel(shared_ptr< Model> model);
	shared_ptr<TransformTree> GetTransform() { return _transform; }

public:
	shared_ptr<Model> _model;
	shared_ptr<TransformTree> _transform;
};

