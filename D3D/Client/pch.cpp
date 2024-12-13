#include "pch.h"
#include "Core.h"

unique_ptr<Core> core = make_unique<Core>();

int WINDOW_WIDTH =1280;
int WINDOW_HEIGHT = 800;

shared_ptr<ModelKeyframe> ModelAnimation::GetKeyframe(const wstring& name)
{

	auto findIt = keyframes.find(name);
	if (findIt == keyframes.end())
		return nullptr;

	return findIt->second;

};
