#pragma once
class RenderTargets
{

public:
	RenderTargets();
	~RenderTargets();

public:
	void Init(DWORD WndWidth, DWORD WndHeight , ComPtr<IDXGISwapChain3> swapchain);
	void Resize(DWORD BackBufferWidth, DWORD BackBufferHeight, ComPtr<IDXGISwapChain3> swapchain, UINT	_swapChainFlags);

	void CreateRenderTarget(DWORD WndWidth, DWORD WndHeight, ComPtr<IDXGISwapChain3> swapchain);
	void CreateDepthStencil();

	void RenderBegin();
	void RenderEnd();

	void SetIndex(UINT index) { _RenderTargetIndex = index; }
	uint32 GetIndex() { return _RenderTargetIndex; }

private:

	ComPtr<ID3D12Resource> _RenderTargets[SWAP_CHAIN_FRAME_COUNT] = {};
	ComPtr<ID3D12Resource> _depthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap> _RTVHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> _DSVHeap = nullptr;

	D3D12_VIEWPORT	_viewport = {};
	D3D12_RECT		_scissorRect = {};
	DWORD			_Width = 0;
	DWORD			_height = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE		_rtvHandle[SWAP_CHAIN_FRAME_COUNT];
	D3D12_CPU_DESCRIPTOR_HANDLE     _dsvHandle;
	uint32	_RenderTargetIndex = 0;


};

