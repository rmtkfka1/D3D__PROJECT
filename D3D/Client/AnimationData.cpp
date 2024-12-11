#include "pch.h"
#include "AnimationData.h"

shared_ptr<ModelKeyframe> ModelAnimation::GetKeyframe(const wstring& name)
{

	auto findIt = keyframes.find(name);
	if (findIt == keyframes.end())
		return nullptr;

	return findIt->second;

};
