#pragma once

struct ModelKeyframeData
{
	float time;
	vec3 scale;
	Quaternion rotation;
	vec3 translation;
};

struct ModelKeyframe
{
	wstring boneName;
	vector<ModelKeyframeData> transforms;
};

struct ModelAnimation
{
	shared_ptr<ModelKeyframe> GetKeyframe(const wstring& name);
	wstring name;
	float frameRate = 0.f;
	uint32 frameCount = 0;
	unordered_map<wstring, shared_ptr<ModelKeyframe>> keyframes; 
};

