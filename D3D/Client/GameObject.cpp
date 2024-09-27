#include "pch.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
GameObject::GameObject()
{
}
GameObject::~GameObject()
{

}

void GameObject::Init()
{
}

void GameObject::Update()
{
	for (auto& ele : _meshRenderer)
	{
		ele->Update();
	}
}

void GameObject::Render()
{
	for (auto& ele : _meshRenderer)
	{
		ele->Render();
	}
}
