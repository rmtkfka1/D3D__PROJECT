#include "pch.h"
#include "SphereCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "BoxCollider.h"
#include "Mesh.h"
#include "Shader.h"
#include "ResourceManager.h"
#include "Core.h"
#include "BufferPool.h"
std::shared_ptr<GraphicsShader> SphereCollider::_shader = nullptr;

SphereCollider::SphereCollider() :BaseCollider(ColliderType::Sphere)
{

    if (!_shader)  
    {
        _shader = make_shared<GraphicsShader>();
        ShaderInfo info;
        info.rasterizerType = RASTERIZER_TYPE::WIREFRAME;
        _shader = ResourceManager::GetInstance()->Load<GraphicsShader>(L"boundingbox.hlsl", info);
    }
}

SphereCollider::~SphereCollider()
{

	int a = 5;

}

void SphereCollider::Init()
{

}

void SphereCollider::Update()
{
    auto onwerTransform = GetOwner()->GetTransform();
    _orginsphere.Transform(_sphere, onwerTransform->GetWorldMatrix());

}

void SphereCollider::Render()
{
    GetOwner()->GetTransform()->PushData();
    _shader->SetPipelineState();
    core->GetBufferManager()->GetGraphicsTableHeap()->SetGraphicsRootDescriptorTable();
    _mesh->Render();

}

bool SphereCollider::CheckCollusion(shared_ptr<BaseCollider>& other)
{
	if (other->GetColliderType() == ColliderType::Box)
	{
		shared_ptr<BoxCollider> otherbox = static_pointer_cast<BoxCollider>(other);

		if (_sphere.Intersects(otherbox->GetBox()))
		{
			return true;
		}
	}

	if (other->GetColliderType() == ColliderType::Sphere)
	{
		shared_ptr<SphereCollider> otherSphere = static_pointer_cast<SphereCollider>(other);

		if (_sphere.Intersects(otherSphere->GetSphere()))
		{
			return true;
		}
	}

	return false;

}

bool SphereCollider::CheckCollusion(Ray& ray, OUT float& distance)
{
	return _sphere.Intersects(ray.position, ray.direction, OUT distance);
}

void SphereCollider::MakeBoundingSphere()
{

	_sphere.Center = this->GetCenter();
	_sphere.Radius = this->GetRadius();

	_orginsphere.Center = this->GetCenter();
	_orginsphere.Radius = this->GetRadius();

	Update();

    const int stackCount = 20;
    const int sliceCount = 20;

    vector<Vertex> vertices;
    vector<uint32> indices;

    Vertex topVertex = { XMFLOAT3(0.0f, +_totalRadius, 0.0f) };
    Vertex bottomVertex = { XMFLOAT3(0.0f, -_totalRadius, 0.0f) };

    vertices.push_back(topVertex);

    float phiStep = XM_PI / stackCount;
    float thetaStep = 2.0f * XM_PI / sliceCount;

    // 스택 사이의 버텍스
    for (int i = 1; i <= stackCount - 1; ++i)
    {
        float phi = i * phiStep;

        for (int j = 0; j <= sliceCount; ++j)
        {
            float theta = j * thetaStep;

            Vertex v;

            v.position.x = _totalRadius * sinf(phi) * cosf(theta);
            v.position.y = _totalRadius * cosf(phi);
            v.position.z = _totalRadius * sinf(phi) * sinf(theta);

            vertices.push_back(v);
        }
    }

    vertices.push_back(bottomVertex);

    // 인덱스 생성 (상단)
    for (int i = 1; i <= sliceCount; ++i)
    {
        indices.push_back(0);
        indices.push_back(i + 1);
        indices.push_back(i);
    }

    // 인덱스 생성 (중간 부분)
    int baseIndex = 1;
    int ringVertexCount = sliceCount + 1;

    for (int i = 0; i < stackCount - 2; ++i)
    {
        for (int j = 0; j < sliceCount; ++j)
        {
            indices.push_back(baseIndex + i * ringVertexCount + j);
            indices.push_back(baseIndex + i * ringVertexCount + j + 1);
            indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

            indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
            indices.push_back(baseIndex + i * ringVertexCount + j + 1);
            indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
        }
    }

    // 인덱스 생성 (하단)
    int southPoleIndex = (int)vertices.size() - 1;

    baseIndex = southPoleIndex - ringVertexCount;
    for (int i = 0; i < sliceCount; ++i)
    {
        indices.push_back(southPoleIndex);
        indices.push_back(baseIndex + i);
        indices.push_back(baseIndex + i + 1);
    }

    _mesh = make_shared<Mesh>();
    _mesh->Init(vertices, indices);

    Update();


}
