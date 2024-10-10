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
	static float f = 0;

	GetMaterial()->SetFloat(0, f);

	f += 0.02f * TimeManager::GetInstance()->GetDeltaTime();

	Super::Update();
}

void Sea::Render()
{
	Super::Render();
}
