#pragma once
#include "BaseCollider.h"
#include "Material.h"
class Mesh;
class GraphicsShader;



class BoxCollider :public BaseCollider
{

public:
	BoxCollider();
	virtual ~BoxCollider();

	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;
	virtual bool CheckCollusion(shared_ptr<BaseCollider>& other) override;
	virtual bool CheckCollusion(Ray& ray, OUT float& distance) override;


	BoundingOrientedBox& GetBox() { return _box; }
	vec3 GetSize() { return _totalSize; }
	void SetSize(vec3 size) { _totalSize = size; }
	void MakeBoundingBox();



private:
	BoundingOrientedBox _box; // 실제 연산에사용되는 박스
	BoundingOrientedBox _orginbox; // 처음에 원본박스

	shared_ptr<Mesh> _mesh; //디버깅용 그려주는 박스
	vec3 _totalSize;

	static shared_ptr<GraphicsShader> _shader;

public:

};

