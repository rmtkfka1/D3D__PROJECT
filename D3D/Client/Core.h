#pragma once
const UINT SWAP_CHAIN_FRAME_COUNT = 2;

class RootSignature;

class Core
{

public:
	Core();
	~Core();

	void Init(HWND hwnd, bool EnableDebugLayer, bool EnableGBV);
	void WaitSynce();

	void RenderBegin();
	void RenderEnd();
	void Present();

	void UpdateWindowSize(DWORD BackBufferWidth, DWORD BackBufferHeight);


	ComPtr<ID3D12Device5> GetDevice() { return _device; }
	ComPtr<ID3D12GraphicsCommandList> GetCmdLIst() { return _cmdList; }
	shared_ptr<RootSignature> GetRootSignature() { return _rootsignature; }
private:

	void CreateDevice(bool EnableDebugLayer, bool EnableGBV);
	void CreateCmdQueue();
	void CreateSwapChain();
	void CreateFence();
	void CreateRootSignature();
	void SetDebugLayerInfo(ComPtr<ID3D12Device> pD3DDevice);

private:

	HWND	_hwnd = nullptr;

	ComPtr<ID3D12Device5> _device = nullptr;
	ComPtr<IDXGIFactory4> _factory;
	ComPtr<ID3D12CommandQueue> _cmdQueue = nullptr;
	ComPtr<ID3D12CommandAllocator> _cmdMemory = nullptr;
	ComPtr<ID3D12GraphicsCommandList> _cmdList = nullptr;
	UINT64	_fenceValue = 0;

	D3D_FEATURE_LEVEL	_FeatureLevel = D3D_FEATURE_LEVEL_11_0;
	DXGI_ADAPTER_DESC1	_adapterDesc = {};
	UINT	m_dwSwapChainFlags = 0;

private:
	shared_ptr<RootSignature> _rootsignature;


	ComPtr<IDXGISwapChain3> _swapChain = nullptr;
	D3D12_VIEWPORT	m_Viewport = {};
	D3D12_RECT		m_ScissorRect = {};
	DWORD			m_dwWidth = 0;
	DWORD			m_dwHeight = 0;

	ComPtr<ID3D12Resource> m_pRenderTargets[SWAP_CHAIN_FRAME_COUNT] = {};
	ComPtr<ID3D12DescriptorHeap> _RTVHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> _DSVHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> _SRVHeap = nullptr;

	UINT	m_rtvDescriptorSize = 0;
	UINT	m_uiRenderTargetIndex = 0;

	HANDLE	_fenceEvent = nullptr;
	ComPtr<ID3D12Fence> _fence = nullptr;
};

