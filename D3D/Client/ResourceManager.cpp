#include "pch.h"
#include "ResourceManager.h"
#include "Shader.h"
void ResourceManager::Init()
{
	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->Init(L"default.hlsl");
		Add<Shader>(L"default.hlsl", shader);
	}

	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		ShaderInfo info;
		info.shaderType = ShaderType::DEFREED;
		shader->Init(L"deferred.hlsl",info);
		Add<Shader>(L"deferred.hlsl", shader);
	}

	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		ShaderInfo info;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS_EQUAL;
		shader->Init(L"sky.hlsl", info);
		Add<Shader>(L"sky.hlsl", shader);
	}


}
