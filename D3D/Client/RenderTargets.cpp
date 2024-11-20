#include "pch.h"
#include "RenderTargets.h"
#include "Core.h"
#include "Texture.h"
#include "BufferManager.h"
#include "BufferPool.h"
RenderTargets::RenderTargets()
{
}

RenderTargets::~RenderTargets()
{
}

void RenderTargets::Init(DWORD WndWidth, DWORD WndHeight, ComPtr<IDXGISwapChain3> swapchain)
{
	CreateRenderTarget(WndWidth, WndHeight, swapchain);
	CreateDepthStencil(WndWidth, WndHeight);
}

void RenderTargets::Resize(DWORD BackBufferWidth, DWORD BackBufferHeight , ComPtr<IDXGISwapChain3> swapchain , UINT	_swapChainFlags )
{

	CreateDepthStencil(BackBufferWidth, BackBufferHeight);

	for (UINT n = 0; n < SWAP_CHAIN_FRAME_COUNT; n++)
	{
		core->GetBufferManager()->GetTextureBufferPool()->FreeRTVHandle(_rtvHandle[n]);
		_RenderTargets[n].Reset();
	}

	if (FAILED(swapchain->ResizeBuffers(SWAP_CHAIN_FRAME_COUNT, BackBufferWidth, BackBufferHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _swapChainFlags)))
	{
		__debugbreak();
	}

	_RenderTargetIndex = swapchain->GetCurrentBackBufferIndex();

	for (int32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; i++)
		swapchain->GetBuffer(i, IID_PPV_ARGS(&_RenderTargets[i]));

	for (int32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; ++i)
	{
		core->GetBufferManager()->GetTextureBufferPool()->AllocRTVDescriptorHandle(&_rtvHandle[i]);
		core->GetDevice()->CreateRenderTargetView(_RenderTargets[i].Get(), nullptr, _rtvHandle[i]);
	}

	_viewport = D3D12_VIEWPORT{ 0.0f,0.0f,static_cast<float>(BackBufferWidth),static_cast<float>(BackBufferHeight), 0,1.0f };
	_scissorRect = D3D12_RECT{ 0,0, static_cast<LONG>(BackBufferWidth),static_cast<LONG>(BackBufferHeight) };
}

void RenderTargets::CreateRenderTarget(DWORD WndWidth, DWORD WndHeight, ComPtr<IDXGISwapChain3> swapchain)
{

	_viewport = D3D12_VIEWPORT{ 0.0f,0.0f,static_cast<float>(WndWidth),static_cast<float>(WndHeight), 0,1.0f };
	_scissorRect = D3D12_RECT{ 0,0, static_cast<LONG>(WndWidth),static_cast<LONG>(WndHeight) };

	for (int32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; i++)
		swapchain->GetBuffer(i, IID_PPV_ARGS(&_RenderTargets[i]));

	for (int32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; i++)
	{
		core->GetBufferManager()->GetTextureBufferPool()->AllocRTVDescriptorHandle(&_rtvHandle[i]);
		core->GetDevice()->CreateRenderTargetView(_RenderTargets[i].Get(), nullptr, _rtvHandle[i]);
	}

	_RenderTargetIndex = swapchain->GetCurrentBackBufferIndex();
}

void RenderTargets::CreateDepthStencil(DWORD WndWidth, DWORD WndHeight)
{
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, WndWidth, WndHeight);
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optimizedClearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);

	core->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&optimizedClearValue,
		IID_PPV_ARGS(&_depthStencilBuffer));

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	core->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_DSVHeap));

	_dsvHandle = _DSVHeap->GetCPUDescriptorHandleForHeapStart();
	core->GetDevice()->CreateDepthStencilView(_depthStencilBuffer.Get(), nullptr, _dsvHandle);


}

void RenderTargets::RenderBegin()
{
	ComPtr<ID3D12GraphicsCommandList> cmdList = core->GetGraphics()->GetCmdLIst();
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_RenderTargets[_RenderTargetIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	const float BackColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	cmdList->RSSetViewports(1, &_viewport);
	cmdList->RSSetScissorRects(1, &_scissorRect);
	cmdList->ClearRenderTargetView(_rtvHandle[_RenderTargetIndex], BackColor, 0, nullptr);
	cmdList->OMSetRenderTargets(1, &_rtvHandle[_RenderTargetIndex], FALSE, &_dsvHandle);

}

void RenderTargets::RenderEnd()
{
	ComPtr<ID3D12GraphicsCommandList> cmdList = core->GetGraphics()->GetCmdLIst();
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_RenderTargets[_RenderTargetIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	cmdList->Close();
}

void RenderTargets::ClearDepth()
{
	ComPtr<ID3D12GraphicsCommandList> cmdList = core->GetGraphics()->GetCmdLIst();
	cmdList->ClearDepthStencilView(_dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
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

void GBuffer::Init(ComPtr<ID3D12DescriptorHeap> DSVHeap)
{
	_viewport = D3D12_VIEWPORT{ 0.0f,0.0f,static_cast<float>(WINDOW_WIDTH),static_cast<float>(WINDOW_HEIGHT), 0,1.0f };
	_scissorRect = D3D12_RECT{ 0,0, static_cast<LONG>(WINDOW_WIDTH),static_cast<LONG>(WINDOW_HEIGHT) };

	UINT rtvHeapIncrementsize = core->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	UINT srvHeapIncrementsize = core->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	_dsvHandle = DSVHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = _count;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;

	ThrowIfFailed(core->GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_RTVHeap)));

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = _count;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	ThrowIfFailed(core->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_SRVHeap)));

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = _RTVHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE srvHeapBegin = _SRVHeap->GetCPUDescriptorHandleForHeapStart();
	/*-------------------------------------------------------------------------------------------------------
										*���ҽ�����
	*-------------------------------------------------------------------------------------------------------*/

	//----------------------------------------Position ����  -------------------------------------------------/ 
#pragma region
	{
		D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, WINDOW_WIDTH, WINDOW_HEIGHT);
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
		float arrFloat[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		D3D12_CLEAR_VALUE optimizedClearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R32G32B32A32_FLOAT, arrFloat);

		HRESULT hr = core->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&desc,
			resourceStates,
			&optimizedClearValue,
			IID_PPV_ARGS(&_resources[0]));

		assert(SUCCEEDED(hr));

		_rtvHandle[0] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeapBegin, 0 * rtvHeapIncrementsize);
		core->GetDevice()->CreateRenderTargetView(_resources[0].Get(), nullptr, _rtvHandle[0]);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		_srvHandle[0] = CD3DX12_CPU_DESCRIPTOR_HANDLE(srvHeapBegin, 0 * srvHeapIncrementsize);
		core->GetDevice()->CreateShaderResourceView(_resources[0].Get(), &srvDesc, _srvHandle[0]);
	}

#pragma endregion

	//----------------------------------------NorMal ����  -------------------------------------------------// 
#pragma region
	{
		D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, WINDOW_WIDTH, WINDOW_HEIGHT);
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
		
		float arrFloat[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		D3D12_CLEAR_VALUE optimizedClearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R32G32B32A32_FLOAT, arrFloat);

		HRESULT hr = core->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&desc,
			resourceStates,
			&optimizedClearValue,
			IID_PPV_ARGS(&_resources[1]));

		assert(SUCCEEDED(hr));

		_rtvHandle[1] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeapBegin, 1 * rtvHeapIncrementsize);
		core->GetDevice()->CreateRenderTargetView(_resources[1].Get(), nullptr, _rtvHandle[1]);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		_srvHandle[1] = CD3DX12_CPU_DESCRIPTOR_HANDLE(srvHeapBegin, 1 * srvHeapIncrementsize);
		core->GetDevice()->CreateShaderResourceView(_resources[1].Get(), &srvDesc, _srvHandle[1]);
	}
#pragma endregion
	//----------------------------------------Diffuse ����  -------------------------------------------------// 
#pragma region
	{
		D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, WINDOW_WIDTH, WINDOW_HEIGHT);
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
		float arrFloat[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		D3D12_CLEAR_VALUE optimizedClearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, arrFloat);

		HRESULT hr = core->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&desc,
			resourceStates,
			&optimizedClearValue,
			IID_PPV_ARGS(&_resources[2]));

		assert(SUCCEEDED(hr));

		_rtvHandle[2] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeapBegin, 2 * rtvHeapIncrementsize);
		core->GetDevice()->CreateRenderTargetView(_resources[2].Get(), nullptr, _rtvHandle[2]);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		_srvHandle[2] = CD3DX12_CPU_DESCRIPTOR_HANDLE(srvHeapBegin, 2 * srvHeapIncrementsize);
		core->GetDevice()->CreateShaderResourceView(_resources[2].Get(), &srvDesc, _srvHandle[2]);
	}
#pragma endregion

	//SRV �ڵ� �ؽ��� Ŭ������ ä���ֱ� WHY? ���籸������ Material ���� Texture Ŭ������ �̿��ؼ� 
	//���ҽ� ���ε��� �������ְ��ֱ⶧����.
	//�ؽ��� ������ Resource �� �����Ұ���.

	_textrues.resize(3);
	
	for (int i = 0; i < _count; ++i)
	{
		if (_textrues[i] == nullptr) //ó�������ÿ��� �ʱ�ȭ ������� Resize Buffer �� �ڵ麹�縸�Ͼ����
		{
			_textrues[i] = make_shared<Texture>();
		}

		_textrues[i]->SetSrvHandle(_srvHandle[i]);
	}

}

void GBuffer::RenderBegin()
{
	auto& list = core->GetGraphics()->GetCmdLIst();
	float arrFloat[4] = {1, 1, 1, 1 };

	for (uint32 i = 0; i < _count; i++)
	{
		list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_resources[i].Get(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
	}

	for (uint32 i = 0; i < _count; i++)
	{
		list->ClearRenderTargetView(_rtvHandle[i], arrFloat, 0, nullptr);
	}

	list->RSSetViewports(1, &_viewport);
	list->RSSetScissorRects(1, &_scissorRect);
	list->OMSetRenderTargets(_count, &_rtvHandle[0], TRUE, &_dsvHandle); //���߼�

}

void GBuffer::RenderEnd()
{
	auto& list = core->GetGraphics()->GetCmdLIst();

	for (uint32 i = 0; i < _count; i++)
	{
		list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_resources[i].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE));
	}
}




shared_ptr<Texture> GBuffer::GetTexture(int32 index)
{
	assert(index < _count);

	return _textrues[index];
}
