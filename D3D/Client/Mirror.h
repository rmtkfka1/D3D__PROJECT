#pragma once
#include "ModelObject.h"

class Shader;
class GameObject;

class Mirror :public ModelObject
{

public:

	virtual void Init() override;;
	virtual void Update() override;
	virtual void Render() override;

	virtual void OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider>other);
	virtual void OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other);

	void PushObject(shared_ptr<GameObject> object) { _mirrorObjects.push_back(object); }
	void SetCubeMap(shared_ptr<GameObject> object) { _cubeMap = object; }

private:
	

private:

	vector<shared_ptr<GameObject>> _mirrorObjects;
	shared_ptr<GameObject> _cubeMap;
	shared_ptr<GraphicsShader> _mirrorWriteShader;
	shared_ptr<GraphicsShader> _mirrorReadShader;
	shared_ptr<GraphicsShader> _mirrorCubeReadShader;
	shared_ptr<GraphicsShader> _defaultShader;
	DirectX::SimpleMath::Plane  _plane;
	Matrix _reflectMat;
};

