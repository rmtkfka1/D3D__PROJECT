#include "pch.h"
#include "BloomEffect.h"
#include "Shader.h"
#include "Core.h"
#include "BufferPool.h"
#include "BufferManager.h"
#include "Texture.h"
#include "RenderTargets.h"

namespace ENUM
{
	enum PostProceesType
	{
		None,
		Bloom,
		Blurring,
		Depth
	};
};

BloomEffect::BloomEffect()
{
	{
		shared_ptr<ComputeShader> shader = make_shared<ComputeShader>();
		shader->Init(L"xblur.hlsl");
		_xblurShader = shader;
	}
	{
		shared_ptr<ComputeShader> shader = make_shared<ComputeShader>();
		shader->Init(L"yblur.hlsl");
		_yblurShader = shader;
	}

	{
		shared_ptr<ComputeShader> shader = make_shared<ComputeShader>();
		shader->Init(L"blackShader.hlsl");
		_blackShader = shader;

	}
	{
		shared_ptr<ComputeShader> shader = make_shared<ComputeShader>();
		shader->Init(L"bloomShader.hlsl");
		_bloomShader = shader;
	}
}
BloomEffect::~BloomEffect()
{

}
void BloomEffect::GenTexture()
{
	_texture = make_shared<Texture>();
	_texture->CreateTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON ,WINDOW_WIDTH, WINDOW_HEIGHT,
		TextureUsageFlags::SRV | TextureUsageFlags::UAV, false,true);

	_texture2 = make_shared<Texture>();
	_texture2->CreateTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT,
		TextureUsageFlags::SRV | TextureUsageFlags::UAV, false,true);

	_interMediateTexture = core->GetRenderTarget()->GetInterMediateTexture();

	_bloomTexture = make_shared<Texture>();
	_bloomTexture->CreateTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT,
		TextureUsageFlags::SRV | TextureUsageFlags::UAV, false,true);

}


void BloomEffect::DepthRender()
{

}

void BloomEffect::Render(int32 disPatchX, int32 disPatchY, int32 disPatchZ)
{
	
	static ENUM::PostProceesType type=ENUM::PostProceesType::None;


	if (KeyManager::GetInstance()->GetButtonDown(KEY_TYPE::SEVEN))
	{
		type = ENUM::None;
	}

	if (KeyManager::GetInstance()->GetButtonDown(KEY_TYPE::EIGHT))
	{
		type = ENUM::Bloom;
	}

	if (KeyManager::GetInstance()->GetButtonDown(KEY_TYPE::NINE))
	{
		type = ENUM::Blurring;
	}

	if (KeyManager::GetInstance()->GetButtonDown(KEY_TYPE::SIX))
	{
		type = ENUM::Depth;
	}

	if (type == ENUM::Depth)
	{
		DepthRender();
	}

	else if (type== ENUM::None)
	{
		return;
	}

	else if (type == ENUM::Bloom)
	{
		Bloom(disPatchX, disPatchY, disPatchZ);
	}
	else if (type == ENUM::Blurring)
	{
		Blurring(disPatchX, disPatchY, disPatchZ);
	}
	
}




void BloomEffect::Bloom(int32 disPatchX, int32 disPatchY, int32 disPatchZ)
{

	//원복테스쳐 블랙색상으로 
	BlackProcess(disPatchX, disPatchY, disPatchZ);

	for (int i = 0; i < 10; ++i)
	{
		BlurringProcess(disPatchX, disPatchY, disPatchZ);
	}
	
	//////////블룸효과처리
	BloomProcess(disPatchX,disPatchY,disPatchZ);

	//계산된 결과를  _interMediateTexture 에 다시복사
	_interMediateTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	_bloomTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	core->GetCmdList()->CopyResource(_interMediateTexture->GetResource().Get(), _bloomTexture->GetResource().Get());

}

void BloomEffect::Blurring(int32 disPatchX, int32 disPatchY, int32 disPatchZ)
{

	_texture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	_interMediateTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	core->GetCmdList()->CopyResource(_texture->GetResource().Get(), _interMediateTexture->GetResource().Get());

	for (int i = 0; i < 20; ++i)
	{
		BlurringProcess(disPatchX, disPatchY, disPatchZ);
	}

	_interMediateTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	_texture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	core->GetCmdList()->CopyResource(_interMediateTexture->GetResource().Get(), _texture->GetResource().Get());
}

void BloomEffect::BlurringProcess(int32 disPatchX, int32 disPatchY, int32 disPatchZ)
{
	// Common 으로 만들면 드라이버가 자동으로잡아주나???

	{
		_xblurShader->SetPipelineState();

		core->GetBufferManager()->GetTable()->CopyUAV(_texture2->GetUAVCpuHandle(), UAV_REGISTER::u0);
		core->GetBufferManager()->GetTable()->CopySRV(_texture->GetSRVCpuHandle(), SRV_REGISTER::t0);

		SetInt(0, WINDOW_WIDTH);
		SetInt(1, WINDOW_HEIGHT);

		core->GetBufferManager()->GetMaterialParamsBufferPool()->PushData(&_params, sizeof(_params));
		core->GetBufferManager()->GetTable()->SetComputeRootDescriptorTable();

		_texture->ResourceBarrier(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		_texture2->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		core->GetCmdList()->Dispatch(disPatchX, disPatchY, disPatchZ);

	}

	{

		_yblurShader->SetPipelineState();

		core->GetBufferManager()->GetTable()->CopyUAV(_texture->GetUAVCpuHandle(), UAV_REGISTER::u0);
		core->GetBufferManager()->GetTable()->CopySRV(_texture2->GetSRVCpuHandle(), SRV_REGISTER::t0);

		SetInt(0, WINDOW_WIDTH);
		SetInt(1, WINDOW_HEIGHT);

		core->GetBufferManager()->GetMaterialParamsBufferPool()->PushData(&_params, sizeof(_params));
		core->GetBufferManager()->GetTable()->SetComputeRootDescriptorTable();

		_texture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		_texture2->ResourceBarrier(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

		core->GetCmdList()->Dispatch(disPatchX, disPatchY, disPatchZ);
	
	}

}

void BloomEffect::BlackProcess(int32 disPatchX, int32 disPatchY, int32 disPatchZ)
{
	_texture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	_interMediateTexture->ResourceBarrier(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	_blackShader->SetPipelineState();

	core->GetBufferManager()->GetTable()->CopyUAV(_texture->GetUAVCpuHandle(), UAV_REGISTER::u0);
	core->GetBufferManager()->GetTable()->CopySRV(_interMediateTexture->GetSRVCpuHandle(), SRV_REGISTER::t0);

	SetInt(0, WINDOW_WIDTH);
	SetInt(1, WINDOW_HEIGHT);
	
	core->GetBufferManager()->GetMaterialParamsBufferPool()->PushData(&_params, sizeof(_params));
	core->GetBufferManager()->GetTable()->SetComputeRootDescriptorTable();

	core->GetCmdList()->Dispatch(disPatchX, disPatchY, disPatchZ);


}

void BloomEffect::BloomProcess(int32 disPatchX, int32 disPatchY, int32 disPatchZ)
{
	_bloomTexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	_texture->ResourceBarrier(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE); //블러된 검은이미지
	_interMediateTexture->ResourceBarrier(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE); //원본이미지

	_bloomShader->SetPipelineState();

	core->GetBufferManager()->GetTable()->CopyUAV(_bloomTexture->GetUAVCpuHandle(), UAV_REGISTER::u0);
	core->GetBufferManager()->GetTable()->CopySRV(_texture->GetSRVCpuHandle(), SRV_REGISTER::t0);
	core->GetBufferManager()->GetTable()->CopySRV(_interMediateTexture->GetSRVCpuHandle(), SRV_REGISTER::t1);

	SetInt(0, WINDOW_WIDTH);
	SetInt(1, WINDOW_HEIGHT);

	core->GetBufferManager()->GetMaterialParamsBufferPool()->PushData(&_params, sizeof(_params));
	core->GetBufferManager()->GetTable()->SetComputeRootDescriptorTable();

	core->GetCmdList()->Dispatch(disPatchX, disPatchY, disPatchZ);
}




