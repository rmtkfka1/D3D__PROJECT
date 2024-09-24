#pragma once
class RenderTargets
{

public:
	RenderTargets();
	~RenderTargets();

public:
	void Init(DWORD WndWidth, DWORD WndHeight , ComPtr<IDXGISwapChain3> swapchain);
	void Resize(DWORD BackBufferWidth, DWORD BackBufferHeight, ComPtr<IDXGISwapChain3> swapchain, UINT	m_dwSwapChainFlags);

	ComPtr<ID3D12Resource>* GetResource() { return _RenderTargets; }
	
	void RenderBegin();
	void RenderEnd();
	void SetIndex(UINT index) { _RenderTargetIndex = index; }
private:

	ComPtr<ID3D12Resource> _RenderTargets[SWAP_CHAIN_FRAME_COUNT] = {};
	ComPtr<ID3D12DescriptorHeap> _RTVHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> _DSVHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> _SRVHeap = nullptr;



	D3D12_VIEWPORT	_viewport = {};
	D3D12_RECT		_scissorRect = {};
	DWORD			_Width = 0;
	DWORD			_height = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE		_rtvHandle[SWAP_CHAIN_FRAME_COUNT];

	uint32	_RenderTargetIndex = 0;


};

