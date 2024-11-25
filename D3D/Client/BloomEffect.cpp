#include "pch.h"
#include "BloomEffect.h"
#include "Shader.h"
#include "Core.h"
#include "BufferPool.h"
#include "BufferManager.h"
#include "Texture.h"
#include "Graphics.h"
#include "RenderTargets.h"
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
}
BloomEffect::~BloomEffect()
{

}
void BloomEffect::GenTexture()
{
	_texture = make_shared<Texture>();
	_texture->CreateTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON ,WINDOW_WIDTH, WINDOW_HEIGHT,
		TextureUsageFlags::SRV | TextureUsageFlags::UAV, false);

	_texture2 = make_shared<Texture>();
	_texture2->CreateTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT,
		TextureUsageFlags::SRV | TextureUsageFlags::UAV, false);

	_interMediateTexture = GRAPHICS->GetRenderTarget()->GetInterMediateTexture();
}


void BloomEffect::Render(int32 disPatchX, int32 disPatchY, int32 disPatchZ)
{


	static int count = 1;

	if (KeyManager::GetInstance()->GetButtonDown(KEY_TYPE::Q))
	{
		count *= -1;
	}

	if (count == 1)
	{
		PostProcess(disPatchX, disPatchY, disPatchZ);
	}
	else
	{
		return;
	}

	
}

void BloomEffect::PingPongRender(int32 disPatchX, int32 disPatchY, int32 disPatchZ)
{
	// Common 으로 만들면 드라이버가 자동으로잡아주나???

	{
		_xblurShader->SetPipelineState();

		core->GetBufferManager()->GetComputeTableHeap()->CopyUAV(_texture2->GetUAVCpuHandle(), UAV_REGISTER::u0);
		core->GetBufferManager()->GetComputeTableHeap()->CopySRV(_texture->GetSRVCpuHandle(), SRV_REGISTER::t0);

		SetInt(0, WINDOW_WIDTH);
		SetInt(1, WINDOW_HEIGHT);

		core->GetBufferManager()->GetMaterialParamsBufferPool()->PushComputeData(&_params, sizeof(_params));
		core->GetBufferManager()->GetComputeTableHeap()->SetComputeRootDescriptorTable();

		COMPUTE->GetCmdList()->Dispatch(disPatchX, disPatchY, disPatchZ);
		
	}

	{

		_yblurShader->SetPipelineState();

		core->GetBufferManager()->GetComputeTableHeap()->CopyUAV(_texture->GetUAVCpuHandle(), UAV_REGISTER::u0);
		core->GetBufferManager()->GetComputeTableHeap()->CopySRV(_texture2->GetSRVCpuHandle(), SRV_REGISTER::t0);

		SetInt(0, WINDOW_WIDTH);
		SetInt(1, WINDOW_HEIGHT);

		core->GetBufferManager()->GetMaterialParamsBufferPool()->PushComputeData(&_params, sizeof(_params));
		core->GetBufferManager()->GetComputeTableHeap()->SetComputeRootDescriptorTable();

		COMPUTE->GetCmdList()->Dispatch(disPatchX, disPatchY, disPatchZ);
	
	}

}


void BloomEffect::PostProcess(int32 disPatchX, int32 disPatchY, int32 disPatchZ)
{
	COMPUTE->PrePareExcute();

	//_texture 에 여태까지 그려왔던 정보들복사.
	_texture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	_interMediateTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	GRAPHICS->GetCmdList()->CopyResource(_texture->GetResource().Get(), _interMediateTexture->GetResource().Get());

	for (int i = 0; i < 10; ++i)
	{
		PingPongRender(disPatchX, disPatchY, disPatchZ);
	}

	COMPUTE->Excute();

	//계산된 결과를  _interMediateTexture 에 다시복사
	_interMediateTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	_texture2->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	GRAPHICS->GetCmdList()->CopyResource(_interMediateTexture->GetResource().Get(), _texture2->GetResource().Get());
	_texture2->ResourceBarrier(D3D12_RESOURCE_STATE_COMMON);
	_interMediateTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);

}




