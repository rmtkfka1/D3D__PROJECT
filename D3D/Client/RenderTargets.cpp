#include "pch.h"
#include "RenderTargets.h"
#include "Core.h"

RenderTargets::RenderTargets()
{
}

RenderTargets::~RenderTargets()
{
}

void RenderTargets::Init(DWORD WndWidth, DWORD WndHeight, ComPtr<IDXGISwapChain3> swapchain)
{
	CreateRenderTarget(WndWidth, WndHeight, swapchain);
	CreateDepthStencil();

}

void RenderTargets::Resize(DWORD BackBufferWidth, DWORD BackBufferHeight , ComPtr<IDXGISwapChain3> swapchain , UINT	_swapChainFlags )
{

	CreateDepthStencil();

	for (UINT n = 0; n < SWAP_CHAIN_FRAME_COUNT; n++)
	{
		_RenderTargets[n].Reset();
	}


	if (FAILED(swapchain->ResizeBuffers(SWAP_CHAIN_FRAME_COUNT, BackBufferWidth, BackBufferHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _swapChainFlags)))
	{
		__debugbreak();
	}

	_RenderTargetIndex = swapchain->GetCurrentBackBufferIndex();
	int32 rtvHeapSize = core->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = _RTVHeap->GetCPUDescriptorHandleForHeapStart();


	for (int32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; i++)
		swapchain->GetBuffer(i, IID_PPV_ARGS(&_RenderTargets[i]));

	for (int i = 0; i < SWAP_CHAIN_FRAME_COUNT; ++i)
	{
		_rtvHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeapBegin, i * rtvHeapSize);
		core->GetDevice()->CreateRenderTargetView(_RenderTargets[i].Get(), nullptr, _rtvHandle[i]);
	}




	_Width = BackBufferWidth;
	_height = BackBufferHeight;
	_viewport.Width = (float)_Width;
	_viewport.Height = (float)_height;
	_scissorRect.left = 0;
	_scissorRect.top = 0;
	_scissorRect.right = _Width;
	_scissorRect.bottom = _height;
}

void RenderTargets::CreateRenderTarget(DWORD WndWidth, DWORD WndHeight, ComPtr<IDXGISwapChain3> swapchain)
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = SWAP_CHAIN_FRAME_COUNT;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	ThrowIfFailed((core->GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_RTVHeap))));

	_viewport.Width = (float)WndWidth;
	_viewport.Height = (float)WndHeight;
	_viewport.MinDepth = 0.0f;
	_viewport.MaxDepth = 1.0f;

	_scissorRect.left = 0;
	_scissorRect.top = 0;
	_scissorRect.right = WndWidth;
	_scissorRect.bottom = WndHeight;


	int32 rtvHeapSize = core->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = _RTVHeap->GetCPUDescriptorHandleForHeapStart();

	for (int32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; i++)
		swapchain->GetBuffer(i, IID_PPV_ARGS(&_RenderTargets[i]));

	for (int i = 0; i < SWAP_CHAIN_FRAME_COUNT; ++i)
	{
		_rtvHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeapBegin, i * rtvHeapSize);
		core->GetDevice()->CreateRenderTargetView(_RenderTargets[i].Get(), nullptr, _rtvHandle[i]);
	}


	_RenderTargetIndex = swapchain->GetCurrentBackBufferIndex();
}

void RenderTargets::CreateDepthStencil()
{

	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, WINDOW_WIDTH, WINDOW_HEIGHT);
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
	ComPtr<ID3D12GraphicsCommandList> cmdList = core->GetCmdLIst();
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_RenderTargets[_RenderTargetIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	const float BackColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	cmdList->RSSetViewports(1, &_viewport);
	cmdList->RSSetScissorRects(1, &_scissorRect);
	cmdList->ClearRenderTargetView(_rtvHandle[_RenderTargetIndex], BackColor, 0, nullptr);
	cmdList->ClearDepthStencilView(_dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	cmdList->OMSetRenderTargets(1, &_rtvHandle[_RenderTargetIndex], FALSE, &_dsvHandle);

}

void RenderTargets::RenderEnd()
{
	ComPtr<ID3D12GraphicsCommandList> cmdList = core->GetCmdLIst();

	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_RenderTargets[_RenderTargetIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	cmdList->Close();
}



