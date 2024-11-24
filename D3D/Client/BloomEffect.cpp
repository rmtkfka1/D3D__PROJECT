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

	_GBufferTexture = GRAPHICS->GetGBuffer()->GetTexture(2);

	ResourceManager::GetInstance()->Add<Texture>(L"BloomTexture", _texture);
}

//GBUFFER �� ������ ����.
void BloomEffect::Render(int32 disPatchX, int32 disPatchY, int32 disPatchZ)
{

	COMPUTE->PrePareExcute();

	_shader->SetPipelineState();

	for (int i = 0; i < 10; ++i)
	{
		PingPongRender(disPatchX, disPatchY, disPatchZ);
	}

	COMPUTE->Excute();
}

void BloomEffect::PingPongRender(int32 disPatchX, int32 disPatchY, int32 disPatchZ)
{

	{
		
		_texture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		core->GetBufferManager()->GetComputeTableHeap()->CopyUAV(_texture->GetUAVCpuHandle(), UAV_REGISTER::u0);
		core->GetBufferManager()->GetComputeTableHeap()->CopySRV(GRAPHICS->GetGBuffer()->GetTexture(2)->GetSRVCpuHandle(), SRV_REGISTER::t0);

		SetInt(0, WINDOW_WIDTH);
		SetInt(1, WINDOW_HEIGHT);

		core->GetBufferManager()->GetMaterialParamsBufferPool()->PushComputeData(&_params, sizeof(_params));
		core->GetBufferManager()->GetComputeTableHeap()->SetComputeRootDescriptorTable();

		COMPUTE->GetCmdList()->Dispatch(disPatchX, disPatchY, disPatchZ);
		
	}

	{
		COMPUTE->PrePareExcute();
		
		_texture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
		GRAPHICS->GetGBuffer()->GetTexture(2)->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		core->GetBufferManager()->GetComputeTableHeap()->CopyUAV(GRAPHICS->GetGBuffer()->GetTexture(2)->GetUAVCpuHandle(), UAV_REGISTER::u0);
		core->GetBufferManager()->GetComputeTableHeap()->CopySRV(_texture->GetUAVCpuHandle(), SRV_REGISTER::t0);
		SetInt(0, WINDOW_WIDTH);
		SetInt(1, WINDOW_HEIGHT);

		core->GetBufferManager()->GetMaterialParamsBufferPool()->PushComputeData(&_params, sizeof(_params));
		core->GetBufferManager()->GetComputeTableHeap()->SetComputeRootDescriptorTable();

		COMPUTE->GetCmdList()->Dispatch(disPatchX, disPatchY, disPatchZ);
	
		GRAPHICS->GetGBuffer()->GetTexture(2)->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	}

}





