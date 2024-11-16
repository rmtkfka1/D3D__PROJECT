#pragma once

class RootSignature;
class RenderTargets;
class GBuffer;
class D3D12ResourceManager;

class DescriptorTable;
class TextureBufferPool;
class ConstantBufferPool;
class StreamOutputBuffer;


class Core
{

public:
	Core();
	~Core();

	void Init(HWND hwnd, bool EnableDebugLayer, bool EnableGBV);
	void Fence();
	void WaitForFenceValue(uint64 ExpectedFenceValue);
	void WaitForAllFence();
	void Exit();
	void SetFullScreen();

	void RenderBegin();
	void RenderEnd();
	void Present();

	void UpdateWindowSize(DWORD BackBufferWidth, DWORD BackBufferHeight);

	HWND GetWindowHandle() { return _hwnd; }
	ComPtr<ID3D12Device5>& GetDevice() { return _device; } 
	ComPtr<ID3D12GraphicsCommandList>& GetCmdLIst() { return _cmdList[_currentContextIndex]; }
	shared_ptr<RootSignature>& GetRootSignature() { return _rootsignature; }
	shared_ptr<RenderTargets>& GetRenderTarget() { return _renderTargets; }
	shared_ptr<GBuffer>& GetGBuffer() { return _GBuffer; }
	shared_ptr<D3D12ResourceManager>& GetResourceManager() { return _resourceManager; }
	shared_ptr<DescriptorTable>& GetTableHeap() { return _table[_currentContextIndex]; }
	shared_ptr<ConstantBufferPool>& GetWorldBufferPool() { return _WorldBufferPool[_currentContextIndex]; }
	shared_ptr<ConstantBufferPool>& GetCameraBufferPool() { return _CameraBufferPool[_currentContextIndex]; }
	shared_ptr<ConstantBufferPool>& GetLIghtBufferPool() { return _lightBufferPool[_currentContextIndex]; }
	shared_ptr<ConstantBufferPool>& GetMaterialParamsBufferPool() { return _materialParamsBufferPool[_currentContextIndex]; }
	shared_ptr<TextureBufferPool>& GetTextureBufferPool() {return _textureBufferPool; }


	uint64 GetCurrentFrameIndex() { return _currentContextIndex; }

private:
	
	void CreateDevice(bool EnableDebugLayer, bool EnableGBV);
	void CreateCmdQueue();
	void CreateSwapChain();
	void CreateFence();
	void CreateRootSignature();
	void CreateBufferPool();
	void SetDebugLayerInfo(ComPtr<ID3D12Device> pD3DDevice);

private:
	bool FullScreen = false;
	HWND	_hwnd = nullptr;

	ComPtr<ID3D12Device5> _device = nullptr;
	ComPtr<IDXGIFactory4> _factory =nullptr;
	ComPtr<ID3D12CommandQueue> _cmdQueue = nullptr;

	ComPtr<ID3D12CommandAllocator> _cmdMemory[MAX_FRAME_COUNT] = {};
	ComPtr<ID3D12GraphicsCommandList> _cmdList[MAX_FRAME_COUNT] = {};
	uint64  _lastFenceValue[MAX_FRAME_COUNT] = { 0 };
	uint64	_fenceValue = 0;

	D3D_FEATURE_LEVEL	_FeatureLevel = D3D_FEATURE_LEVEL_11_0;
	DXGI_ADAPTER_DESC1	_adapterDesc = {};
	UINT	_swapChainFlags = 0;
	uint64  _currentContextIndex = 0;

private:
	shared_ptr<RootSignature> _rootsignature;
	shared_ptr<D3D12ResourceManager> _resourceManager;

	ComPtr<IDXGISwapChain3> _swapChain = nullptr;
	
	HANDLE	_fenceEvent = nullptr;
	ComPtr<ID3D12Fence> _fence = nullptr;

private:
	//Buffer pool
	array<shared_ptr<DescriptorTable>,MAX_FRAME_COUNT>  _table;
	array<shared_ptr<ConstantBufferPool>, MAX_FRAME_COUNT> _lightBufferPool;
	array<shared_ptr<ConstantBufferPool>, MAX_FRAME_COUNT> _WorldBufferPool;
	array<shared_ptr<ConstantBufferPool>, MAX_FRAME_COUNT> _CameraBufferPool;
	array<shared_ptr<ConstantBufferPool>, MAX_FRAME_COUNT> _materialParamsBufferPool;
	shared_ptr<TextureBufferPool>  _textureBufferPool;

	shared_ptr<RenderTargets> _renderTargets;
	shared_ptr<GBuffer> _GBuffer;
};

