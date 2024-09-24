#pragma once

class RootSignature;
class RenderTargets;

class Core
{

public:
	Core();
	~Core();

	void Init(HWND hwnd, bool EnableDebugLayer, bool EnableGBV);
	void WaitSync();

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
	shared_ptr<RenderTargets> _renderTargets;

	ComPtr<IDXGISwapChain3> _swapChain = nullptr;
	
	HANDLE	_fenceEvent = nullptr;
	ComPtr<ID3D12Fence> _fence = nullptr;
};

