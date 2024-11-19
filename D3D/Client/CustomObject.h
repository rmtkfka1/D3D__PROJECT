#pragma once
#include "GameObject.h"
//
// 메시를 직접 만들어서 연결해주는 오브젝트 ( 모델 사용  X)

class Mesh;
class Material;
class Transform;
class BaseCollider;
class GraphicsShader;

class CustomObject :public GameObject
{
public:
	CustomObject();
	CustomObject(PlayerType type);
	virtual ~CustomObject();

	virtual void Init() override;;
	virtual void Update() override;
	virtual void Render() override;
	virtual shared_ptr<Transform> GetTransform() override { return _transform; }


	virtual void OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider>other);
	virtual void OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other);
public:

	shared_ptr<Mesh>& GetMesh() { return _mesh; }
    shared_ptr<Material>& GetMaterial() { return _material; }


	void SetMesh(shared_ptr<Mesh> mesh) { _mesh = mesh; }
	void SetMaterial(shared_ptr<Material> material) { _material = material; }
	void SetTransform(shared_ptr<Transform> transform) { _transform = transform; }


protected:
	shared_ptr<Mesh> _mesh;
	shared_ptr<Material> _material;
	shared_ptr<Transform> _transform;
};

