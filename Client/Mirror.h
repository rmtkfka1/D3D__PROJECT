#pragma once
#include "ModelObject.h"

class Shader;
class GameObject;
class AnimationObject;
class Mirror :public ModelObject
{

public:

	virtual void Init() override;;
	virtual void Update() override;
	virtual void Render() override;

	virtual void OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider>other);
	virtual void OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other);

	void PushObject(shared_ptr<GameObject> object) { _mirrorObjects.push_back(object); }
	void PushAniObject(shared_ptr<AnimationObject> object) { _animationObject = object; }
private:
	

private:

	vector<shared_ptr<GameObject>> _mirrorObjects;
	shared_ptr<AnimationObject> _animationObject;
	shared_ptr<GraphicsShader> _mirrorWriteShader;
	shared_ptr<GraphicsShader> _mirrorReadShader;
	shared_ptr<GraphicsShader> _mirrorReadAniShader;
	shared_ptr<GraphicsShader> _defaultShader;
	DirectX::SimpleMath::Plane  _plane;
	Matrix _reflectMat;
};

