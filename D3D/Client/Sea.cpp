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
	if (_bMove)
	{
		static float time = 0;

		GetMaterial()->SetFloat(4, time);
		time += TimeManager::GetInstance()->GetDeltaTime();
	}

	else
	{
		static float time = 0;
		GetMaterial()->SetInt(0,1);
		GetMaterial()->SetFloat(4, time);
		time += TimeManager::GetInstance()->GetDeltaTime();
	}

	Super::Update();
}

void Sea::Render()
{
	Super::Render();
}
