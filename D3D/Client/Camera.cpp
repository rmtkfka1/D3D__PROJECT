#include "pch.h"
#include "Camera.h"
#include "Core.h"
#include "BufferPool.h"
#include "Transform.h"
#include "TimeManager.h"
#include "Player.h"
#include "TransformTree.h"
#include "KeyManager.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include <random>

std::default_random_engine generator;
std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

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


void Camera::SetData()
{
	Update(); //각자의 업데이트를 호출
	auto& bufferPool = core->GetBufferManager()->GetCameraBufferPool();
	bufferPool->SetData(1,&_params,sizeof(_params));
}

bool Camera::IsInFrustum(shared_ptr<BaseCollider>& collider)
{
	switch (collider->GetColliderType())
	{
	case ColliderType::Box:
		return _boundingFrsutum.Intersects(static_pointer_cast<BoxCollider>(collider)->GetBox());
	case ColliderType::Sphere:
		return _boundingFrsutum.Intersects(static_pointer_cast<SphereCollider>(collider)->GetSphere());
		break;
	}

	
}


void Camera::AddMove(const vec3& shift)
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

}

ThirdPersonCamera::~ThirdPersonCamera()
{
}

void ThirdPersonCamera::Update()
{
	Animation();
	GenViewMatrix();
	GenProjMatrix();
	GenBoundingFrustum();
}


void ThirdPersonCamera::GenViewMatrix()
{
	_params.matView = XMMatrixLookToLH(_cameraPos + _shake, _cameraLook , _cameraUp);
}

void ThirdPersonCamera::GenProjMatrix()
{
	_params.matProjection = XMMatrixPerspectiveFovLH(_fov, WINDOW_WIDTH / WINDOW_HEIGHT, _near, _far);
}

void ThirdPersonCamera::GenBoundingFrustum()
{
	_boundingFrsutum.CreateFromMatrix(_boundingFrsutum, _params.matProjection);
	_boundingFrsutum.Transform(_boundingFrsutum, _params.matView.Invert());
}

void ThirdPersonCamera::Rotate(const shared_ptr<Player>& player)
{

	//offset 을 플레어의 회전방향에 맞게 회전시킴.
	vec3 offset = vec3::TransformNormal(_offset, player->GetTransform()->GetWorldMatrix());

	//회전된 오프셋을 이용하여 카메라의 타켓 포즈를 정함.
	vec3 targetPos = player->GetTransform()->GetLocalPosition() + offset;

	//이동방향을 정함
	vec3 direction = vec3(targetPos - _cameraPos);
	_cameraPos = vec3::Lerp(_cameraPos, targetPos, 0.3f);

	//카메라의 Look 을 다시설정함. 
	Matrix resultMat = XMMatrixLookAtLH(_cameraPos, player->GetTransform()->GetLocalPosition(), player->GetTransform()->GetUp());

	_cameraRight = vec3(resultMat._11, resultMat._21, resultMat._31);
	_cameraUp = vec3(resultMat._12, resultMat._22, resultMat._32);
	_cameraLook = vec3(resultMat._13, resultMat._23, resultMat._33);

}

void ThirdPersonCamera::Animation()
{


	float dt = TimeManager::GetInstance()->GetDeltaTime();;

	if (_elaspedTime > 2.0f)
	{
		_animationflag = false;
		_elaspedTime = 0;
		_shake = vec3(0, 0, 0);
	}

	if (_animationflag)
	{
		float intenisty = 200.0f;
		_shake.x = distribution(generator) * intenisty * dt;
		_shake.y = distribution(generator) * intenisty * dt;
		_elaspedTime += dt;
	}

}

/*************************
*                        *
*     ObserveCamera      *
*                        *
**************************/

ObserveCamera::ObserveCamera():Camera(CameraType::OBSERVE)
{

}

ObserveCamera::~ObserveCamera()
{
}

void ObserveCamera::GenViewMatrix()
{
	_params.matView = XMMatrixLookToLH(_cameraPos, _cameraLook, _cameraUp);
}

void ObserveCamera::GenProjMatrix()
{
	_params.matProjection = XMMatrixPerspectiveFovLH(_fov, WINDOW_WIDTH / WINDOW_HEIGHT, _near, _far);
}

void ObserveCamera::GenBoundingFrustum()
{
	_boundingFrsutum.CreateFromMatrix(_boundingFrsutum, _params.matProjection);
	_boundingFrsutum.Transform(_boundingFrsutum, _params.matView.Invert());
}

void ObserveCamera::Update()
{
	MouseUpdate();
	KeyUpdate();
	GenViewMatrix();
	GenProjMatrix();
	GenBoundingFrustum();
}

void ObserveCamera::KeyUpdate()
{
	float dt = TimeManager::GetInstance()->GetDeltaTime();


	if (KeyManager::GetInstance()->GetButton(KEY_TYPE::W))
	{
		_cameraPos += _cameraSpeed * 50.0f * _cameraLook * dt;
	}

	if (KeyManager::GetInstance()->GetButton(KEY_TYPE::S))
	{
		_cameraPos -= _cameraSpeed * 50.0f * _cameraLook * dt;
	}

	if (KeyManager::GetInstance()->GetButton(KEY_TYPE::D))
	{
		_cameraPos += _cameraSpeed * 50.0f * _cameraRight * dt;
	}

	if (KeyManager::GetInstance()->GetButton(KEY_TYPE::A))
	{
		_cameraPos -= _cameraSpeed * 50.0f * _cameraRight * dt;

	}
}

void ObserveCamera::MouseUpdate()
{
	vec2 pos = KeyManager::GetInstance()->GetDeletaPos();

	vec3 cameraLook = vec3(0, 0, 1.0f);
	vec3 caemraRight = vec3(1.0f, 0, 0.0f);

	static vec2 accmulate = vec2(0, 0);
	accmulate.x += pos.x;
	accmulate.y += pos.y;

	_cameraLook = _cameraLook.TransformNormal(cameraLook, Matrix::CreateFromYawPitchRoll(-XMConvertToRadians(accmulate.x), XMConvertToRadians(accmulate.y), 0));
	_cameraRight = _cameraRight.TransformNormal(caemraRight, Matrix::CreateFromYawPitchRoll(-XMConvertToRadians(accmulate.x), XMConvertToRadians(accmulate.y), 0));
}

/*************************
*                        *
*     UiCamera           *
*                        *
**************************/


UiCamera::UiCamera() :Camera(CameraType::UI)
{
	_cameraPos = vec3(0, 0, -100.0f);
	_cameraLook = vec3(0, 0, 1.0f);
	_cameraUp = vec3(0, 1.0f, 0);
	_near = 1.0f;
	_far = 3000.0f;
};

UiCamera::~UiCamera()
{

}

void UiCamera::Update()
{
	GenViewMatrix();
	GenProjMatrix();
}


void UiCamera::GenViewMatrix()
{
	_params.matView = XMMatrixLookToLH(_cameraPos, _cameraLook, _cameraUp);
}

void UiCamera::GenProjMatrix()
{
	_params.matProjection = XMMatrixOrthographicLH(1920, 1080, _near, _far);
}

void UiCamera::GenBoundingFrustum()
{
}

