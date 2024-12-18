#pragma once
#include "BaseCollider.h"
class GraphicsShader;
class Mesh;

class SphereCollider : public BaseCollider
{

public:

	SphereCollider();
	virtual ~SphereCollider();

	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;
	virtual bool CheckCollusion(shared_ptr<BaseCollider>& other) override;
	virtual bool CheckCollusion(Ray& ray, OUT float& distance) override;

	BoundingSphere& GetSphere() { return _sphere; }
	void MakeBoundingSphere();
	void SetRadius(float radius) { _totalRadius = radius; }

public:
	float GetRadius() { return _totalRadius; }

private:
	shared_ptr<Mesh> _mesh; //디버깅용 그려주는 스피어
	BoundingSphere _sphere;
	BoundingSphere _orginsphere;
	float _totalRadius{};

	static shared_ptr<GraphicsShader> _shader;
};

