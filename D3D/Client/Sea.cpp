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

	static float time = 0;

	GetMaterial()->SetFloat(4, time);
	time += TimeManager::GetInstance()->GetDeltaTime();


	Super::Update();
};

void Sea::Render()
{
	Super::Render();
}
