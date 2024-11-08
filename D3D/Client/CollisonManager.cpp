#include "pch.h"
#include "CollisonManager.h"
#include "BaseCollider.h"
#include "BoxCollider.h"
#include "GameObject.h"
#include <varargs.h>
#include <iostream>
#include "Core.h"
#include "CameraManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "KeyManager.h"

void CollisonManager::Init()
{
}

void CollisonManager::Update()
{
	CheckObjectCollusion();
	//CheckRayCollusion();
	LateUpdate();
}

void CollisonManager::Clear()
{
	_activeColliders.clear();
	_passiveColliders.clear();
}

void CollisonManager::CheckObjectCollusion()
{

	for (int32 i = 0; i < _activeColliders.size(); i++)
	{
		for (int32 j = 0; j < _passiveColliders.size(); j++)
		{
			shared_ptr<BaseCollider> src = _activeColliders[i];
			shared_ptr<BaseCollider> dest = _passiveColliders[j];

			if (src == nullptr || dest == nullptr)
			{
				continue;
			}

			if (src->CheckCollusion(dest))
			{
				if (src->_collisionMap.contains(dest.get()) == false)
				{
					src->GetOwner()->OnComponentBeginOverlap(src, dest);
					dest->GetOwner()->OnComponentBeginOverlap(dest, src);
					src->_collisionMap.insert(dest.get());
					dest->_collisionMap.insert(src.get());
				}
			}

			else
			{
				if (src->_collisionMap.contains(dest.get()))
				{
					src->GetOwner()->OnComponentEndOverlap(src, dest);
					dest->GetOwner()->OnComponentEndOverlap(dest, src);
					src->_collisionMap.erase(dest.get());
					dest->_collisionMap.erase(src.get());
				}
			}
		}
	}


	for (int32 i = 0; i < _activeColliders.size(); i++)
	{
		for (int32 j = i+1; j < _activeColliders.size(); j++)
		{
			shared_ptr<BaseCollider> src = _activeColliders[i];
			shared_ptr<BaseCollider> dest = _activeColliders[j];

			if (src == nullptr || dest == nullptr)
			{
				continue;
			}

			if (src->CheckCollusion(dest))
			{
				if (src->_collisionMap.contains(dest.get()) == false)
				{
					src->GetOwner()->OnComponentBeginOverlap(src, dest);
					dest->GetOwner()->OnComponentBeginOverlap(dest, src);
					src->_collisionMap.insert(dest.get());
					dest->_collisionMap.insert(src.get());
				}
			}

			else
			{
				if (src->_collisionMap.contains(dest.get()))
				{
					src->GetOwner()->OnComponentEndOverlap(src, dest);
					dest->GetOwner()->OnComponentEndOverlap(dest, src);
					src->_collisionMap.erase(dest.get());
					dest->_collisionMap.erase(src.get());
				}
			}
		}
	}

}

void CollisonManager::CheckRayCollusion()
{
	

}

bool CollisonManager::CheckRayCollusion(Ray ray, shared_ptr<BaseCollider> collider)
{

	float distance = FLT_MAX;

	if (collider->CheckCollusion(ray, distance))
	{
		return true;
	}

	return false;

};

void CollisonManager::LateUpdate()
{
	while (_deleteColliders.empty() ==false)
	{
		shared_ptr<BaseCollider> collider=_deleteColliders.front();
		RemoveCollider(collider);
		_deleteColliders.pop();
	}
}

vec3 CollisonManager::ToWorldPosition(const vec3& pos, const Matrix& W, const Matrix& V, const Matrix& P)
{
	vec3 p = pos;

	//NDC 로 바꿈
	p.x = 2.f * (p.x - 0) / WINDOW_WIDTH - 1.f;
	p.y = -2.f * (p.y - 0) / WINDOW_HEIGHT + 1.f;
	p.z = ((p.z - 0) / (1 - 0));

	Matrix wvp = W * V * P;

	//모델좌표계 로 변환이지만,  W에다가 단위행렬을 넣으면 그냥 월드좌표계인셈.
	Matrix wvpInv = wvp.Invert();

	p = vec3::Transform(p, wvpInv);
	return p;
}


void CollisonManager::ReserveDeleteCollider(shared_ptr<BaseCollider>& collider)
{
	_deleteColliders.push(collider);
}

void CollisonManager::AddCollider(const shared_ptr<BaseCollider>& collider, ColliderBehave behave)
{
	switch (behave)
	{
	case ColliderBehave::Active:
		_activeColliders.push_back(collider);
		break;
	case ColliderBehave::Passive:
		_passiveColliders.push_back(collider);
		break;
	default:
		break;
	}
}


void CollisonManager::Reset()
{
	for (auto& ele : _activeColliders)
	{
		ele->Clear();
	}

	for (auto& ele : _passiveColliders)
	{
		ele->Clear();
	}
}

void CollisonManager::RemoveCollider(shared_ptr<BaseCollider>& collider)
{

	auto it = std::remove(_activeColliders.begin(), _activeColliders.end(), collider);
	_activeColliders.erase(it, _activeColliders.end());

	auto it2 = std::remove(_passiveColliders.begin(), _passiveColliders.end(), collider);
	_passiveColliders.erase(it2, _passiveColliders.end());


}


