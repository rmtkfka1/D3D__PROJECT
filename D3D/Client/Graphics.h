#pragma once

class RootSignature;
class RenderTargets;
class GBuffer;
class D3D12ResourceManager;

class GraphicsDescriptorTable;
class TextureBufferPool;
class ConstantBufferPool;
class BufferManager;


class Graphics
{
public:
	Graphics();
	~Graphics();

	void Init(HWND hwnd, ComPtr<ID3D12Device5> device, ComPtr<IDXGIFactory4> factory , shared_ptr<BufferManager> bufferManager );
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

	ComPtr<ID3D12GraphicsCommandList>& GetCmdLIst() { return _cmdList[_currentContextIndex]; }

	shared_ptr<RenderTargets>& GetRenderTarget() { return _renderTargets; }
	shared_ptr<GBuffer>& GetGBuffer() { return _GBuffer; }


	uint64 GetCurrentFrameIndex() { return _currentContextIndex; }

private:

	void CreateCmdQueue();
	void CreateSwapChain();
	void CreateFence();
	


private:
	bool FullScreen = false;
	HWND	_hwnd = nullptr;

	ComPtr<ID3D12Device5> _device = nullptr;
	ComPtr<IDXGIFactory4> _factory = nullptr;
	ComPtr<ID3D12CommandQueue> _cmdQueue = nullptr;

	ComPtr<ID3D12CommandAllocator> _cmdMemory[MAX_FRAME_COUNT] = {};
	ComPtr<ID3D12GraphicsCommandList> _cmdList[MAX_FRAME_COUNT] = {};
	uint64  _lastFenceValue[MAX_FRAME_COUNT] = { 0 };
	uint64	_fenceValue = 0;

	UINT	_swapChainFlags = 0;
	uint64  _currentContextIndex = 0;

private:
	ComPtr<IDXGISwapChain3> _swapChain = nullptr;
	HANDLE	_fenceEvent = nullptr;
	ComPtr<ID3D12Fence> _fence = nullptr;

private:
	shared_ptr<BufferManager> _bufferManager;
	shared_ptr<RenderTargets> _renderTargets;
	shared_ptr<GBuffer> _GBuffer;
};

