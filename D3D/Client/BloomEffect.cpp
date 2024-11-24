#include "pch.h"
#include "BloomEffect.h"
#include "Shader.h"
#include "Core.h"
#include "BufferPool.h"
#include "BufferManager.h"
#include "Texture.h"
#include "Graphics.h"
#include "RenderTargets.h"
void BloomEffect::GenTexture()
{
	_texture = make_shared<Texture>();
	_texture->CreateTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE ,WINDOW_WIDTH, WINDOW_HEIGHT,
		TextureUsageFlags::SRV | TextureUsageFlags::UAV, false);

	ResourceManager::GetInstance()->Add<Texture>(L"BloomTexture", _texture);
}

//GBUFFER 의 내용을 복사.
void BloomEffect::FirstRender(int32 disPatchX, int32 disPatchY, int32 disPatchZ)
{
	_shader->SetPipelineState();

	GRAPHICS->GetCmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		_texture->GetResource().Get(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

	core->GetBufferManager()->GetComputeTableHeap()->CopyUAV(_texture->GetUAVCpuHandle(), UAV_REGISTER::u0);
	core->GetBufferManager()->GetComputeTableHeap()->CopySRV(GRAPHICS->GetGBuffer()->GetTexture(2)->GetSRVCpuHandle(), SRV_REGISTER::t0);

	static int PostProcess = 1;

	if (KeyManager::GetInstance()->GetButtonDown(KEY_TYPE::Q))
	{
		PostProcess *= -1;
	}

	SetInt(0, WINDOW_WIDTH);
	SetInt(1, WINDOW_HEIGHT);
	SetInt(2, PostProcess);

	core->GetBufferManager()->GetMaterialParamsBufferPool()->PushComputeData(&_params, sizeof(_params));

	core->GetBufferManager()->GetComputeTableHeap()->SetComputeRootDescriptorTable();

	COMPUTE->GetCmdList()->Dispatch(disPatchX, disPatchY, disPatchZ);
	COMPUTE->Excute();

	GRAPHICS->GetCmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		_texture->GetResource().Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE));
}

void BloomEffect::PingPongRender(int32 disPatchX, int32 disPatchY, int32 disPatchZ)
{
	_shader->SetPipelineState();


}





