#include "pch.h"
#include "Transform.h"
#include "Core.h"
#include "BufferPool.h"

Transform::Transform() 
{

}

Transform::~Transform()
{

}


void Transform::Update()
{
	Matrix matScale = Matrix::CreateScale(_localScale);
	Matrix matRotation = Matrix::CreateFromQuaternion(_localRotation);
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

	_matWorld.Decompose(_scale, _rotation, _position);

	for (const shared_ptr<Transform>& child : _children)
		child->Update();
}

void Transform::PushData()
{
	TransformParams transformParams = {};
	transformParams.matWorld = _matWorld;

	core->GetWorldBufferPool()->PushData(&transformParams, sizeof(transformParams));
}

void Transform::AddMove(const vec3& shift)
{
	_localPosition += shift;
}

void Transform::AddRotate(const vec3& Euler)
{
	//왼손좌표계로 이용하기위해 부호반전시킴
	Quaternion newRotation = Quaternion::CreateFromYawPitchRoll(
		XMConvertToRadians(-Euler.y),  // Yaw (Y축 회전)
		XMConvertToRadians(-Euler.x),  // Pitch (X축 회전)
		XMConvertToRadians(-Euler.z)   // Roll (Z축 회전)
	);

	// 기존 쿼터니언에 누적 회전 (새로운 회전 * 기존 회전)
	_localRotation = newRotation * _localRotation;

	// 쿼터니언을 정규화
	_localRotation.Normalize();

}

void Transform::SetLocalRotation(const vec3 localRotation)
{
	//왼손좌표계로 이용하기위해 부호반전시킴
	float pitch = XMConvertToRadians(-localRotation.x);
	float yaw = XMConvertToRadians(-localRotation.y);  
	float roll = XMConvertToRadians(-localRotation.z); 

	_localRotation = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
	_localRotation.Normalize();
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
	// 월드 회전 값을 이용해 쿼터니언 생성
	Quaternion worldRotationQuat = Quaternion::CreateFromYawPitchRoll(worldRotation.y, worldRotation.x, worldRotation.z);

	if (HasParent())
	{
		// 부모의 월드 행렬을 반전시킴 ex -30도 돌아가있으면  30도 돌아가는 행렬
		Matrix inverseParentMatrix = _parent.lock()->GetWorldMatrix().Invert();

		// 반전된 부모 행렬을 이용해 부모의 회전을 쿼터니언으로 변환
		Quaternion inverseParentRotationQuat = Quaternion::CreateFromRotationMatrix(inverseParentMatrix);

		// 월드 회전 쿼터니언을 부모의 반전된 회전 쿼터니언과 곱하여 로컬 회전 쿼터니언을 계산
		Quaternion localRotationQuat = worldRotationQuat * inverseParentRotationQuat;

		// 로컬 회전 설정
		SetLocalRotation(localRotationQuat);
	}
	else
	{
		// 부모가 없으면 월드 회전을 그대로 로컬 회전으로 설정
		SetLocalRotation(worldRotationQuat);
	}
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
	return temp;
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
