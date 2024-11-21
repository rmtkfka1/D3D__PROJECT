#include "pch.h"
#include "Transform.h"
#include "Core.h"
#include "BufferPool.h"

Transform::Transform()
    : _localRotation(0.0f, 0.0f, 0.0f)  // 초기 Euler 각도
{
}

Transform::~Transform()
{
}

void Transform::Update()
{

    Matrix matScale = Matrix::CreateScale(_localScale);
    Matrix matRotation = Matrix::CreateFromYawPitchRoll(
        XMConvertToRadians(_localRotation.y),  // Yaw
        XMConvertToRadians(_localRotation.x),  // Pitch
        XMConvertToRadians(_localRotation.z)   // Roll
    );
    Matrix matTranslation = Matrix::CreateTranslation(_localPosition);

    _matLocal = matScale * matRotation * matTranslation;

    if (HasParent())
    {
        _matWorld = _matLocal * _parent.lock()->GetWorldMatrix();
    }
    else
    {
        _matWorld = _matLocal;
    }

    for (const shared_ptr<Transform>& child : _children)
        child->Update();


}

void Transform::PushData()
{
    TransformParams transformParams = {};
    transformParams.matWorld = _matWorld;

    core->GetBufferManager()->GetWorldBufferPool()->PushGraphicsData(&transformParams, sizeof(transformParams));
}

void Transform::AddMove(const vec3& shift)
{
    _localPosition += shift;

    bUpdated = false;
}

void Transform::AddRotate(const vec3& Euler)
{
    // 기존의 로컬 회전 값에 추가 회전값을 더함
    _localRotation.x += Euler.x;
    _localRotation.y += Euler.y;
    _localRotation.z += Euler.z;

    bUpdated = false;
}



void Transform::SetWorldScale(const vec3 worldScale)
{
    if (HasParent())
    {
        vec3 parentScale = _parent.lock()->GetWorldScale();
        vec3 scale = worldScale;
        scale.x /= parentScale.x;
        scale.y /= parentScale.y;
        scale.z /= parentScale.z;
        SetLocalScale(scale);
    }
    else
    {
        SetLocalScale(worldScale);
    }
}

void Transform::SetWorldRotation(const vec3 worldRotation)
{
    // 월드 회전 값을 그대로 로컬 회전으로 설정
    SetLocalRotation(worldRotation);
}

void Transform::SetWorldPosition(const vec3 worldPosition)
{
    if (HasParent())
    {
        Matrix worldToParentLocalMatrix = _parent.lock()->GetWorldMatrix().Invert();

        vec3 position;
        position = position.Transform(worldPosition, worldToParentLocalMatrix);

        SetLocalPosition(position);
    }
    else
    {
        SetLocalPosition(worldPosition);
    }
}

vec3 Transform::GetRight()
{
    vec3 temp = _matWorld.Right();
    temp.Normalize();
    return -temp;
}

vec3 Transform::GetUp()
{
    vec3 temp = _matWorld.Up();
    temp.Normalize();
    return temp;
}

vec3 Transform::GetLook()
{
    vec3 temp = _matWorld.Backward();
    temp.Normalize();
    return temp;
}

