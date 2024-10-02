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

	// Children
	for (const shared_ptr<Transform>& child : _children)
		child->Update();
}

void Transform::PushData()
{
	TransformParams transformParams = {};
	transformParams.matWorld = _matWorld;

	core->GetWorldBufferPool()->PushData(&transformParams, sizeof(transformParams));
}

void Transform::MoveShift(const vec3& shift)
{
	_localPosition += shift;
}

void Transform::RotateShift(const vec3 shift)
{
	Quaternion currentRotation = Quaternion::CreateFromYawPitchRoll(
		XMConvertToRadians(_localRotation.y),
		XMConvertToRadians(_localRotation.x),
		XMConvertToRadians(_localRotation.z)
	);

	// ���� �߰��� ȸ������ ���
	float pitch = XMConvertToRadians(shift.x);  // X-axis rotation
	float yaw = XMConvertToRadians(shift.y);    // Y-axis rotation
	float roll = XMConvertToRadians(shift.z);   // Z-axis rotation

	// �� ȸ���� ���ʹϾ����� ��ȯ
	Quaternion deltaRotation = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);

	_localRotation = currentRotation * deltaRotation;

}

void Transform::SetLocalRotation(const vec3 localRotation)
{
	float pitch = XMConvertToRadians(localRotation.x); // X-axis rotation
	float yaw = XMConvertToRadians(localRotation.y);   // Y-axis rotation
	float roll = XMConvertToRadians(localRotation.z);  // Z-axis rotation

	_localRotation = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);

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
