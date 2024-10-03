#include "pch.h"
#include "Camera.h"
#include "Core.h"
#include "BufferPool.h"
#include "Player.h"
#include "Transform.h"

/*************************
*                        *
*       Camera           *
*                        *
**************************/

Camera::Camera(CameraType type):_type(type)
{

}

Camera::~Camera()
{

}

void Camera::GenViewMatrix()
{
	_params.matView = XMMatrixLookToLH(_cameraPos, _cameraLook, _cameraUp);
}

void Camera::GenProjMatrix()
{
	_params.matProjection = XMMatrixPerspectiveFovLH(_fov, WINDOW_WIDTH / WINDOW_HEIGHT, _near, _far);
}


void Camera::PushData()
{
	GenViewMatrix();
	GenProjMatrix();

	auto& bufferPool = core->GetCameraBufferPool();
	bufferPool->PushData(&_params,sizeof(_params));
	core->GetTableHeap()->SetGraphicsRootDescriptorTable(1);
}

void Camera::MoveShift(const vec3& shift)
{
	_cameraPos += shift;
}

/*************************
*                        *
*  ThirdPersonCamera     *
*                        *
**************************/

ThirdPersonCamera::ThirdPersonCamera():Camera(CameraType::THIRDVIEW)
{
	_cameraPos += _offset;
}

ThirdPersonCamera::~ThirdPersonCamera()
{
}

void ThirdPersonCamera::Rotate(const shared_ptr<Player>& player)
{

	//offset �� �÷����� ȸ�����⿡ �°� ȸ����Ŵ.
	vec3 offset = vec3::TransformNormal(_offset, player->GetTransform()->GetWorldMatrix());

	//ȸ���� �������� �̿��Ͽ� ī�޶��� Ÿ�� ��� ����.
	vec3 targetPos = player->GetTransform()->GetLocalPosition() + offset;

	//�̵������� ����
	vec3 direction = vec3(targetPos - _cameraPos);

	float length = direction.Length();
	direction.Normalize();

	//�������Ӵ� ������ �� //����� ��ǥ���� 10/1 �� ������ �������Ӵ�
	float velocity = length * 0.1f;
	_cameraPos = _cameraPos + velocity * direction;

	
	//ī�޶��� Look �� �ٽü�����. 
	Matrix resultMat = XMMatrixLookAtLH(_cameraPos, player->GetTransform()->GetLocalPosition(), player->GetTransform()->GetUp());

	_cameraRight = vec3(resultMat._11, resultMat._21, resultMat._31);
	_cameraUp = vec3(resultMat._12, resultMat._22, resultMat._32);
	_cameraLook = vec3(resultMat._13, resultMat._23, resultMat._33);


}


