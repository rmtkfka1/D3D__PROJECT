#include "pch.h"
#include "Sea.h"
#include "Material.h"
#include "TimeManager.h"
Sea::Sea():CustomObject(PlayerType::Sea)
{

}

Sea::~Sea()
{
}

void Sea::Init()
{
	Super::Init();
}

void Sea::Update()
{



	GetMaterial()->SetFloat(0, _time);
	_time += TimeManager::GetInstance()->GetDeltaTime();


	Super::Update();
};

void Sea::Render()
{
	Super::Render();
}
