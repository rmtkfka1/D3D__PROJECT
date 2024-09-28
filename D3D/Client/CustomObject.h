#pragma once
#include "GameObject.h"
//
// �޽ø� ���� ���� �������ִ� ������Ʈ ( �� ���  X)

class Mesh;
class Material;
class Transform;

class CustomObject :public GameObject
{
public:
	CustomObject();
	virtual ~CustomObject();

	virtual void Init() override;;
	virtual void Update() override;
	virtual void Render() override;

public:

	shared_ptr<Mesh>& GetMesh() { return _mesh; }
	shared_ptr<Material>& GetMaterial() { return _material; }
	shared_ptr<Transform>& GetTransform() { return _transform; }

	void SetMesh(shared_ptr<Mesh> mesh) { _mesh = mesh; }
	void SetMaterial(shared_ptr<Material> material) { _material = material; }
	void SetTransform(shared_ptr<Transform> transform) { _transform = transform; }

private:
	shared_ptr<Mesh> _mesh;
	shared_ptr<Material> _material;
	shared_ptr<Transform> _transform;
};

