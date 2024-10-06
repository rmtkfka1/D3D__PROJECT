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
	//�޼���ǥ��� �̿��ϱ����� ��ȣ������Ŵ
	Quaternion newRotation = Quaternion::CreateFromYawPitchRoll(
		XMConvertToRadians(-Euler.y),  // Yaw (Y�� ȸ��)
		XMConvertToRadians(-Euler.x),  // Pitch (X�� ȸ��)
		XMConvertToRadians(-Euler.z)   // Roll (Z�� ȸ��)
	);

	// ���� ���ʹϾ� ���� ȸ�� (���ο� ȸ�� * ���� ȸ��)
	_localRotation = newRotation * _localRotation;

	// ���ʹϾ��� ����ȭ
	_localRotation.Normalize();

}

void Transform::SetLocalRotation(const vec3 localRotation)
{
	//�޼���ǥ��� �̿��ϱ����� ��ȣ������Ŵ
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
	// ���� ȸ�� ���� �̿��� ���ʹϾ� ����
	Quaternion worldRotationQuat = Quaternion::CreateFromYawPitchRoll(worldRotation.y, worldRotation.x, worldRotation.z);

	if (HasParent())
	{
		// �θ��� ���� ����� ������Ŵ ex -30�� ���ư�������  30�� ���ư��� ���
		Matrix inverseParentMatrix = _parent.lock()->GetWorldMatrix().Invert();

		// ������ �θ� ����� �̿��� �θ��� ȸ���� ���ʹϾ����� ��ȯ
		Quaternion inverseParentRotationQuat = Quaternion::CreateFromRotationMatrix(inverseParentMatrix);

		// ���� ȸ�� ���ʹϾ��� �θ��� ������ ȸ�� ���ʹϾ�� ���Ͽ� ���� ȸ�� ���ʹϾ��� ���
		Quaternion localRotationQuat = worldRotationQuat * inverseParentRotationQuat;

		// ���� ȸ�� ����
		SetLocalRotation(localRotationQuat);
	}
	else
	{
		// �θ� ������ ���� ȸ���� �״�� ���� ȸ������ ����
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
