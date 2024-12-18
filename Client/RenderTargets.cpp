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
	_DSTexture->CreateTexture(DXGI_FORMAT_D24_UNORM_S8_UINT,D3D12_RESOURCE_STATE_DEPTH_WRITE ,WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::DSV, false, true);
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

	_DSTexture->CreateTexture(DXGI_FORMAT_D24_UNORM_S8_UINT, D3D12_RESOURCE_STATE_DEPTH_WRITE,WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::DSV, false,true);

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
	cmdList->ClearDepthStencilView(_DSTexture->GetSharedDSVHandle(), D3D12_CLEAR_FLAG_DEPTH
	| D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
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

	//Resize 할때 이미할당된 BufferPool 이있는경우 놓아줌.
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
	
	//position 정보
	_textures[0]->CreateTexture(DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_RESOURCE_STATE_COMMON,WINDOW_WIDTH, WINDOW_HEIGHT,TextureUsageFlags::RTV| TextureUsageFlags::SRV | TextureUsageFlags::UAV, false, true);
	//normal 정보
	_textures[1]->CreateTexture(DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_RESOURCE_STATE_COMMON,WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::RTV | TextureUsageFlags::SRV | TextureUsageFlags::UAV, false,true);
	//color 정보
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
	list->OMSetRenderTargets(_count, &_textures[0]->GetRTVCpuHandle(), TRUE, &_textures[0]->GetSharedDSVHandle()); //다중셋

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

	_viewport = D3D12_VIEWPORT{ 0.0f,0.0f,static_cast<float>(WINDOW_WIDTH*3),static_cast<float>(WINDOW_WIDTH * 3), 0,1.0f };
	_scissorRect = D3D12_RECT{ 0,0, static_cast<LONG>(WINDOW_WIDTH * 3),static_cast<LONG>(WINDOW_WIDTH * 3) };

	_texture = make_shared<Texture>();

	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32_TYPELESS, WINDOW_WIDTH * 3, WINDOW_WIDTH * 3);
	desc.MipLevels = 1;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE cvalue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);

	auto hr = core->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&cvalue,
		IID_PPV_ARGS(&_texture->GetResource()));

	if (FAILED(hr))
	{
		throw std::runtime_error("텍스쳐 생성 실패");
	}


	core->GetBufferManager()->GetTextureBufferPool()->AllocDSVDescriptorHandle(&_texture->GetDSVCpuHandle());
	D3D12_DEPTH_STENCIL_VIEW_DESC depthDesc = {};
	depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthDesc.Flags = D3D12_DSV_FLAG_NONE;
	core->GetDevice()->CreateDepthStencilView(_texture->GetResource().Get(), &depthDesc, _texture->GetDSVCpuHandle());

	core->GetBufferManager()->GetTextureBufferPool()->AllocSRVDescriptorHandle(&_texture->GetSRVCpuHandle());
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;
	core->GetDevice()->CreateShaderResourceView(_texture->GetResource().Get(), &srvDesc, _texture->GetSRVCpuHandle());

	_texture->_state = D3D12_RESOURCE_STATE_DEPTH_WRITE;

}

void Shadow::RenderBegin()
{

	auto& list = core->GetCmdList();
	_texture->ResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE);
	list->RSSetViewports(1, &_viewport);
	list->RSSetScissorRects(1, &_scissorRect);
	list->OMSetRenderTargets(0, nullptr, FALSE, &_texture->GetDSVCpuHandle());
	list->ClearDepthStencilView(_texture->GetDSVCpuHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void Shadow::RenderEnd()
{
	_texture->ResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}