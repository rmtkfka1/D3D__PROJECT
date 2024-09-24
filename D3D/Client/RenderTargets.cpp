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
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = SWAP_CHAIN_FRAME_COUNT;	// SwapChain Buffer 0	| SwapChain Buffer 1
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	if (FAILED(core->GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_RTVHeap))))
	{
		__debugbreak();
	}

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

void RenderTargets::Resize(DWORD BackBufferWidth, DWORD BackBufferHeight , ComPtr<IDXGISwapChain3> swapchain , UINT	m_dwSwapChainFlags )
{


	for (UINT n = 0; n < SWAP_CHAIN_FRAME_COUNT; n++)
	{
		_RenderTargets[n].Reset();
	}



	if (FAILED(swapchain->ResizeBuffers(SWAP_CHAIN_FRAME_COUNT, BackBufferWidth, BackBufferHeight, DXGI_FORMAT_R8G8B8A8_UNORM, m_dwSwapChainFlags)))
	{
		__debugbreak();
	}

	_RenderTargetIndex = swapchain->GetCurrentBackBufferIndex();
	int32 rtvHeapSize = core->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = _RTVHeap->GetCPUDescriptorHandleForHeapStart();

	// Create frame resources.
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

void RenderTargets::RenderBegin()
{
	core->GetCmdLIst()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_RenderTargets[_RenderTargetIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	const float BackColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	core->GetCmdLIst()->RSSetViewports(1, &_viewport);
	core->GetCmdLIst()->RSSetScissorRects(1, &_scissorRect);
	core->GetCmdLIst()->ClearRenderTargetView(_rtvHandle[_RenderTargetIndex], BackColor, 0, nullptr);
	core->GetCmdLIst()->OMSetRenderTargets(1, &_rtvHandle[_RenderTargetIndex], FALSE, nullptr);
}

void RenderTargets::RenderEnd()
{
	core->GetCmdLIst()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_RenderTargets[_RenderTargetIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	core->GetCmdLIst()->Close();
}



