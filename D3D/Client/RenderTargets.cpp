#include "pch.h"
#include "RenderTargets.h"
#include "Core.h"
#include "Texture.h"
#include "BufferManager.h"
#include "BufferPool.h"
#include "Material.h"
#include "BloomEffect.h"
RenderTargets::RenderTargets()
{
}

RenderTargets::~RenderTargets()
{
}

void RenderTargets::Init(DWORD WndWidth, DWORD WndHeight, ComPtr<IDXGISwapChain3> swapchain)
{

	for (int32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; i++)
	{
		_RenderTargets[i] = make_shared<Texture>();
	}

	_DSTexture = make_shared<Texture>();
	_InterMediateTexture = make_shared<Texture>();

	_viewport = D3D12_VIEWPORT{ 0.0f,0.0f,static_cast<float>(WndWidth),static_cast<float>(WndHeight), 0,1.0f };
	_scissorRect = D3D12_RECT{ 0,0, static_cast<LONG>(WndWidth),static_cast<LONG>(WndHeight) };

	for (int32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; i++)
		swapchain->GetBuffer(i, IID_PPV_ARGS(&_RenderTargets[i]->GetResource()));

	for (int32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; i++)
	{
		_RenderTargets[i]->CreateTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON,WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::RTV, true, true);
	}

	_InterMediateTexture->CreateTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::RTV
		|TextureUsageFlags::SRV, false, true);

	_RenderTargetIndex = swapchain->GetCurrentBackBufferIndex();
	_DSTexture->CreateTexture(DXGI_FORMAT_D32_FLOAT,D3D12_RESOURCE_STATE_DEPTH_WRITE ,WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::DSV, false, true);
}

void RenderTargets::Resize(DWORD BackBufferWidth, DWORD BackBufferHeight , ComPtr<IDXGISwapChain3> swapchain , UINT	_swapChainFlags )
{

	{
		shared_ptr<Material> material = make_shared<Material>();
		material->SetName(L"finalMaterial");
		material->SetDiffuseTexture(core->GetGBuffer()->GetTexture(0));
		material->SetNormalTexture(core->GetGBuffer()->GetTexture(1));
		material->SetSpecularTexture(core->GetGBuffer()->GetTexture(2)); //ALBEDO
		ResourceManager::GetInstance()->Add<Material>(L"finalMaterial", material);
	}

	core->GetBufferManager()->GetTextureBufferPool()->FreeDSVHandle(_DSTexture->GetSharedDSVHandle());
	core->GetBufferManager()->GetTextureBufferPool()->FreeRTVHandle(_InterMediateTexture->GetRTVCpuHandle());

	for (UINT n = 0; n < SWAP_CHAIN_FRAME_COUNT; n++)
	{
		core->GetBufferManager()->GetTextureBufferPool()->FreeRTVHandle(_RenderTargets[n]->GetRTVCpuHandle());
		_RenderTargets[n]->GetResource().Reset();
	}

	if (FAILED(swapchain->ResizeBuffers(SWAP_CHAIN_FRAME_COUNT, BackBufferWidth, BackBufferHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _swapChainFlags)))
	{
		__debugbreak();
	}

	_RenderTargetIndex = swapchain->GetCurrentBackBufferIndex();

	for (int32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; i++)
		swapchain->GetBuffer(i, IID_PPV_ARGS(&_RenderTargets[i]->GetResource()));

	for (int32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; ++i)
	{

		_RenderTargets[i]->CreateTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON,WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::RTV,true,true);
	}


	_viewport = D3D12_VIEWPORT{ 0.0f,0.0f,static_cast<float>(BackBufferWidth),static_cast<float>(BackBufferHeight), 0,1.0f };
	_scissorRect = D3D12_RECT{ 0,0, static_cast<LONG>(BackBufferWidth),static_cast<LONG>(BackBufferHeight) };

	_InterMediateTexture->CreateTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::RTV
		|TextureUsageFlags::SRV, false,true);

	ResourceManager::GetInstance()->Get<BloomEffect>(L"Bloom")->GenTexture();

	_DSTexture->CreateTexture(DXGI_FORMAT_D32_FLOAT, D3D12_RESOURCE_STATE_DEPTH_WRITE,WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::DSV, false,true);

}



void RenderTargets::RenderBegin()
{
	ComPtr<ID3D12GraphicsCommandList> cmdList = core->GetCmdList();

	_InterMediateTexture->ResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET);

	const float BackColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	cmdList->RSSetViewports(1, &_viewport);
	cmdList->RSSetScissorRects(1, &_scissorRect);
	cmdList->ClearRenderTargetView(_InterMediateTexture->GetRTVCpuHandle(), BackColor, 0, nullptr);
	cmdList->OMSetRenderTargets(1, &_InterMediateTexture->GetRTVCpuHandle(), FALSE, &_InterMediateTexture->GetSharedDSVHandle());

}

void RenderTargets::RenderEnd()
{
	ComPtr<ID3D12GraphicsCommandList> cmdList = core->GetCmdList();

	_RenderTargets[_RenderTargetIndex]->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	_InterMediateTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);

	cmdList->CopyResource(_RenderTargets[_RenderTargetIndex]->GetResource().Get(), _InterMediateTexture->GetResource().Get());

	_RenderTargets[_RenderTargetIndex]->ResourceBarrier(D3D12_RESOURCE_STATE_PRESENT);
	cmdList->Close();
}

void RenderTargets::ClearDepth()
{
	ComPtr<ID3D12GraphicsCommandList> cmdList = core->GetCmdList();
	cmdList->ClearDepthStencilView(_DSTexture->GetSharedDSVHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}


/*************************
*                        *
*         GBuffer        *
*                        *
**************************/

GBuffer::GBuffer()
{

}

GBuffer::~GBuffer()
{

}

void GBuffer::Init()
{

	//Resize �Ҷ� �̹��Ҵ�� BufferPool ���ִ°�� ������.
	for (int i = 0; i < _count; ++i)
	{
		if (_textures[i])
		{
			core->GetBufferManager()->GetTextureBufferPool()->FreeRTVHandle(_textures[i]->GetRTVCpuHandle());
			core->GetBufferManager()->GetTextureBufferPool()->FreeSRVHandle(_textures[i]->GetSRVCpuHandle());
			core->GetBufferManager()->GetTextureBufferPool()->FreeSRVHandle(_textures[i]->GetUAVCpuHandle());
		}
	}

	_viewport = D3D12_VIEWPORT{ 0.0f,0.0f,static_cast<float>(WINDOW_WIDTH),static_cast<float>(WINDOW_HEIGHT), 0,1.0f };
	_scissorRect = D3D12_RECT{ 0,0, static_cast<LONG>(WINDOW_WIDTH),static_cast<LONG>(WINDOW_HEIGHT) };

	for (int i = 0; i < _count; ++i)
	{
		_textures[i] = make_shared<Texture>();
	}
	
	//position ����
	_textures[0]->CreateTexture(DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_RESOURCE_STATE_COMMON,WINDOW_WIDTH, WINDOW_HEIGHT,TextureUsageFlags::RTV| TextureUsageFlags::SRV | TextureUsageFlags::UAV, false, true);
	//normal ����
	_textures[1]->CreateTexture(DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_RESOURCE_STATE_COMMON,WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::RTV | TextureUsageFlags::SRV | TextureUsageFlags::UAV, false,true);
	//color ����
	_textures[2]->CreateTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::RTV | TextureUsageFlags::SRV | TextureUsageFlags::UAV, false, true);

}

void GBuffer::RenderBegin()
{
	auto& list = core->GetCmdList();
	float arrFloat[4] = {0, 0, 0, 0 };

	for (uint32 i = 0; i < _count; i++)
	{
		_textures[i]->ResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	for (uint32 i = 0; i < _count; i++)
	{
		list->ClearRenderTargetView(_textures[i]->GetRTVCpuHandle(), arrFloat, 0, nullptr);
	}

	list->RSSetViewports(1, &_viewport);
	list->RSSetScissorRects(1, &_scissorRect);
	list->OMSetRenderTargets(_count, &_textures[0]->GetRTVCpuHandle(), TRUE, &_textures[0]->GetSharedDSVHandle()); //���߼�

}

void GBuffer::RenderEnd()
{
	auto& list = core->GetCmdList();

	for (uint32 i = 0; i < _count; i++)
	{
		_textures[i]->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	}
}


shared_ptr<Texture>& GBuffer::GetTexture(int32 index)
{
	assert(index < _count);

	return _textures[index];
}

Shadow::Shadow()
{
}

Shadow::~Shadow()
{
}

void Shadow::Init()
{

	_texture = make_shared<Texture>();


	_texture->CreateTexture(
		DXGI_FORMAT_D32_FLOAT,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		TextureUsageFlags::DSV,
		false,
		false,
		vec4(0.0f, 0, 0, 0)
		);

	_texture->CreateTexture(
		DXGI_FORMAT_R32_FLOAT,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		TextureUsageFlags::SRV,
		true,
		false,
		vec4(1.0f, 0, 0, 0)
	);

}

void Shadow::RenderBegin()
{
	auto& list = core->GetCmdList();
	_texture->ResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE);
	list->ClearDepthStencilView(_texture->GetDSVCpuHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	list->OMSetRenderTargets(0, nullptr, FALSE, &_texture->GetDSVCpuHandle());

}

void Shadow::RenderEnd()
{
	_texture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

}
