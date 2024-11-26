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


	static int count = -1;

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




void BloomEffect::PostProcess(int32 disPatchX, int32 disPatchY, int32 disPatchZ)
{

	//_texture �� ���±��� �׷��Դ� �����麹��.
	_texture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	_interMediateTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);

	GRAPHICS->GetCmdList()->CopyResource(_texture->GetResource().Get(), _interMediateTexture->GetResource().Get());


	for (int i = 0; i < 10; ++i)
	{
		PingPongRender(disPatchX, disPatchY, disPatchZ);
	}

	//���� �����  _interMediateTexture �� �ٽú���
	_interMediateTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	_texture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	GRAPHICS->GetCmdList()->CopyResource(_interMediateTexture->GetResource().Get(), _texture->GetResource().Get());
	_texture->ResourceBarrier(D3D12_RESOURCE_STATE_COMMON);
	_interMediateTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);

}

void BloomEffect::PingPongRender(int32 disPatchX, int32 disPatchY, int32 disPatchZ)
{
	// Common ���� ����� ����̹��� �ڵ���������ֳ�???

	{
		_xblurShader->SetPipelineState();

		_texture->ResourceBarrier(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		_texture2->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		core->GetBufferManager()->GetGraphicsTableHeap()->CopyUAV(_texture2->GetUAVCpuHandle(), UAV_REGISTER::u0);
		core->GetBufferManager()->GetGraphicsTableHeap()->CopySRV(_texture->GetSRVCpuHandle(), SRV_REGISTER::t0);

		SetInt(0, WINDOW_WIDTH);
		SetInt(1, WINDOW_HEIGHT);

		core->GetBufferManager()->GetMaterialParamsBufferPool()->PushGraphicsData(&_params, sizeof(_params));
		core->GetBufferManager()->GetGraphicsTableHeap()->SetComputeRootDescriptorTable();

		GRAPHICS->GetCmdList()->Dispatch(disPatchX, disPatchY, disPatchZ);

	}

	{

		_yblurShader->SetPipelineState();

		core->GetBufferManager()->GetGraphicsTableHeap()->CopyUAV(_texture->GetUAVCpuHandle(), UAV_REGISTER::u0);
		core->GetBufferManager()->GetGraphicsTableHeap()->CopySRV(_texture2->GetSRVCpuHandle(), SRV_REGISTER::t0);

		SetInt(0, WINDOW_WIDTH);
		SetInt(1, WINDOW_HEIGHT);

		core->GetBufferManager()->GetMaterialParamsBufferPool()->PushGraphicsData(&_params, sizeof(_params));
		core->GetBufferManager()->GetGraphicsTableHeap()->SetComputeRootDescriptorTable();

		_texture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		_texture2->ResourceBarrier(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

		GRAPHICS->GetCmdList()->Dispatch(disPatchX, disPatchY, disPatchZ);
	
	}

}




