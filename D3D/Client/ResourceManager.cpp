#include "pch.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "Mesh.h"
#include "GeoMetryHelper.h"
#include "Material.h"
#include "Core.h"
#include "RenderTargets.h"
#include "GameObject.h"
#include "CustomObject.h"
#include "Texture.h"
#include "BufferPool.h"
#include "BloomEffect.h"
void ResourceManager::Init()
{
	CreateDefaultShader();
	CreateDefaultMesh();
	CreateDefaultMaterial();
}

void ResourceManager::CreateDefaultMesh()
{
	{
		shared_ptr<Mesh> mesh = GeoMetryHelper::LoadRectangleMesh(1.0f);
		Add<Mesh>(L"finalMesh", mesh);
	}
}

void ResourceManager::CreateDefaultShader()
{

	{
		shared_ptr<GraphicsShader> shader = make_shared<GraphicsShader>();
		ShaderInfo info;
		info.shaderType = ShaderType::FORWARD;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		shader->Init(L"depthrender.hlsl", info);
		Add<GraphicsShader>(L"depthrender.hlsl", shader);
	}

	{
		shared_ptr<GraphicsShader> shader = make_shared<GraphicsShader>();
		ShaderInfo info;
		info.shaderType = ShaderType::DEFREED;
		shader->Init(L"deferredAnimation.hlsl", info);
		Add<GraphicsShader>(L"deferredAnimation.hlsl", shader);
	}

	
	{
		shared_ptr<GraphicsShader> shader = make_shared<GraphicsShader>();
		ShaderInfo info;
		info.shaderType = ShaderType::SHADOW;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		shader->Init(L"depthwrite.hlsl", info);
		Add<GraphicsShader>(L"depthwrite.hlsl", shader);
	}

	
	{
		shared_ptr<GraphicsShader> shader = make_shared<GraphicsShader>();
		ShaderInfo info;
		info.shaderType = ShaderType::FORWARD;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		shader->Init(L"seatest2.hlsl", info);
		Add<GraphicsShader>(L"seatest2.hlsl", shader);
	}

	{
		shared_ptr<GraphicsShader> shader = make_shared<GraphicsShader>();
		ShaderInfo info;
		info.shaderType = ShaderType::FORWARD;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.blendType = BLEND_TYPE::ONE_TO_ONE_BLEND;
		shader->Init(L"seatest2.hlsl", info);
		Add<GraphicsShader>(L"seatest2Blend.hlsl", shader);
	}

	{
		shared_ptr<GraphicsShader> shader = make_shared<GraphicsShader>();
		ShaderInfo info;
		info.shaderType = ShaderType::FORWARD;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		shader->Init(L"seatest.hlsl", info);
		Add<GraphicsShader>(L"seatest.hlsl", shader);
	}

	{
		shared_ptr<GraphicsShader> shader = make_shared<GraphicsShader>();
		ShaderInfo info;
		info.shaderType = ShaderType::FORWARD;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		shader->Init(L"sea.hlsl", info);
		Add<GraphicsShader>(L"sea.hlsl", shader);
	}


	{
		shared_ptr<GraphicsShader> shader = make_shared<GraphicsShader>();
		ShaderInfo info;
		info.shaderType = ShaderType::DEFREED;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		info.bActiveGSShader = true;
		shader->Init(L"Bilboard.hlsl",info);
		Add<GraphicsShader>(L"Bilboard.hlsl", shader);
	}

	//{
	//	shared_ptr<Shader> shader = make_shared<Shader>();
	//	ShaderInfo info;
	//	info.shaderType = ShaderType::DEFREED;
	//	info.bActiveGSShader = false;
	//	info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
	//	info.primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//	shader->Init(L"BilboardRender.hlsl",info);
	//	Add<Shader>(L"BilboardRender.hlsl", shader);
	//}

	//{
	//	shared_ptr<Shader> shader = make_shared<Shader>();
	//	ShaderInfo info;
	//	info.shaderType = ShaderType::FORWARD;
	//	info.bActvieStreamOutput = true;
	//	info.bActiveGSShader = true;
	//	info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
	//	info.primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	//	shader->Init(L"BilboardStreamOutput.hlsl", info);
	//	Add<Shader>(L"BilboardStreamOutput.hlsl", shader);
	//}




	{
		shared_ptr<GraphicsShader> shader = make_shared<GraphicsShader>();
		shader->Init(L"default.hlsl");
		Add<GraphicsShader>(L"default.hlsl", shader);
	}


	{
		shared_ptr<GraphicsShader> shader = make_shared<GraphicsShader>();
		ShaderInfo info;
		info.shaderType = ShaderType::DEFREED;
		shader->Init(L"deferred.hlsl", info);
		Add<GraphicsShader>(L"deferred.hlsl", shader);
	}

	{
		shared_ptr<GraphicsShader> shader = make_shared<GraphicsShader>();
		ShaderInfo info;
		info.shaderType = ShaderType::DEFREED;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS_EQUAL;

		shader->Init(L"sky.hlsl", info);
		Add<GraphicsShader>(L"sky.hlsl", shader);
	}


	{
		shared_ptr<GraphicsShader> shader = make_shared<GraphicsShader>();
		ShaderInfo info;
		info.shaderType = ShaderType::DEFREED;
		info.rasterizerType = RASTERIZER_TYPE::CULL_NONE;
		info.depthStencilType = DEPTH_STENCIL_TYPE::LESS_EQUAL;

		shader->Init(L"test.hlsl", info);
		Add<GraphicsShader>(L"test.hlsl", shader);
	}



	{
		shared_ptr<GraphicsShader> shader = make_shared<GraphicsShader>();
		ShaderInfo info =
		{
			ShaderType::FORWARD,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::DEFAULT
		};

		shader->Init(L"final.hlsl", info);
		Add<GraphicsShader>(L"final.hlsl", shader);
	}


}

void ResourceManager::CreateDefaultMaterial()
{
	
	{
		shared_ptr<BloomEffect> bloom = make_shared<BloomEffect>();
		bloom->GenTexture();
		Add<BloomEffect>(L"Bloom", bloom);
	};


	{
		shared_ptr<Material> material = make_shared<Material>();
		material->SetName(L"finalMaterial");
		material->SetDiffuseTexture(core->GetGBuffer()->GetTexture(0));
		material->SetNormalTexture(core->GetGBuffer()->GetTexture(1));
		material->SetSpecularTexture(core->GetGBuffer()->GetTexture(2)); //ALBEDO
		Add<Material>(L"finalMaterial", material);
	}


}

