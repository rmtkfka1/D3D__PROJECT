#pragma once

class Mesh;
class Material;
class Transform;

class MeshRenderer
{

public:
	void Update();
	void Render();
	
	void SetMesh(shared_ptr<Mesh> mesh) { _mesh = mesh; }
	void SetTransform(shared_ptr<Transform> transform) { _transform = transform; }
	void SetMaterial(shared_ptr<Material> material) { _material = material; }

	shared_ptr<Mesh> GetMesh() { return _mesh; }
	shared_ptr<Transform> GetTransform()  { return _transform; }
	shared_ptr<Material> GetMaterial()  { return _material; }
private:
	shared_ptr<Mesh> _mesh;
	shared_ptr<Transform> _transform;
	shared_ptr<Material> _material;
};

