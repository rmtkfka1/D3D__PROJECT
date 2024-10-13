#include "pch.h"
#include "Box.h"
#include "Transform.h"
#include <random>


default_random_engine dre;
uniform_real_distribution<double> random_xz(-5000.0f, 5000.0f);
uniform_real_distribution<double> random_y(3000.0f, 5000.0f);
uniform_real_distribution<double> random_sclae(10.0f, 180.0f);

Box::Box():ModelObject(PlayerType::Box)
{

}

Box::~Box()
{
}

void Box::Init()
{
	Super::Init();

	float x = random_xz(dre);
	float z = random_xz(dre);
	float y = random_y(dre);
	float scale = random_sclae(dre);

	_transform->SetLocalScale(vec3(scale, scale, scale));
	_transform->SetLocalPosition(vec3(x, y, z));
}

void Box::Update()
{
	Super::Update();
}

void Box::Render()
{
	Super::Render();
}

void Box::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{

}

void Box::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{

}
