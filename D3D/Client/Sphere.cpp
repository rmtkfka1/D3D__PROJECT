#include "pch.h"
#include "Sphere.h"
#include "Transform.h"
#include <random>

static default_random_engine dre;
static uniform_real_distribution<double> random_xz(-3000.0f, 3000.0f);
static normal_distribution<double> random_y(5500.0f, 500.0f);
static normal_distribution  <double> random_sclae(5.0f, 2.0f);


Sphere::Sphere() :ModelObject(PlayerType::Sphere)
{

}

Sphere::~Sphere()
{
}

void Sphere::Init()
{
	Super::Init();

	float x = random_xz(dre);
	float z = random_xz(dre);
	float y = random_y(dre);
	float scale = random_sclae(dre);

	_transform->SetLocalScale(vec3(scale, scale, scale));
	_transform->SetLocalPosition(vec3(x+3000.0f, y+1000.0f,  z));
}

void Sphere::Update()
{
	Super::Update();
}

void Sphere::Render()
{
	Super::Render();
}

void Sphere::OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{

}

void Sphere::OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other)
{

}
