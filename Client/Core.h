#pragma once



class BufferManager;
class D3D12ResourceManager;
class RootSignature;
class RenderTargets;
class GBuffer;
class Shadow;

class Core
{
public:
    Core();
    ~Core();

    // Initialization and Cleanup
    void Init(HWND hwnd, bool EnableDebugLayer, bool EnableGBV);
    void Exit();
    void SetFullScreen();

    // Device Management
    void CreateDevice(bool EnableDebugLayer, bool EnableGBV);
    void SetDebugLayerInfo(ComPtr<ID3D12Device> pD3DDevice);

    // Command Queue and Swap Chain
    void CreateCmdQueue();
    void CreateSwapChain();

    // Synchronization
    void CreateFence();
    void Fence();
    void WaitForFenceValue(uint64_t ExpectedFenceValue);
    void WaitForAllFence();

    // Rendering
    void RenderBegin();
    void RenderEnd();
    void Present();

    // Window
    void UpdateWindowSize(DWORD BackBufferWidth, DWORD BackBufferHeight);

    // Accessors
    HWND GetWindowHandle() const { return _hwnd; }
    ComPtr<ID3D12GraphicsCommandList>& GetCmdList() { return _cmdList[_currentContextIndex]; }
    shared_ptr<RenderTargets>& GetRenderTarget() { return _renderTargets; }
    shared_ptr<GBuffer>& GetGBuffer() { return _GBuffer; }
    shared_ptr<Shadow>& GetShadow() { return _shadow; }
    uint64_t GetCurrentFrameIndex() const { return _currentContextIndex; }
    ComPtr<ID3D12Device5>& GetDevice() { return _device; }
    shared_ptr<D3D12ResourceManager>& GetResourceManager() { return _resourceManager; }
    shared_ptr<BufferManager>& GetBufferManager() { return _bufferManager; }
    shared_ptr<RootSignature>& GetRootSignature() { return _rootsignature; }

private:
    // Window
    HWND _hwnd = nullptr;
    bool FullScreen = false;

    // Device and Factory
    ComPtr<ID3D12Device5> _device = nullptr;
    ComPtr<IDXGIFactory4> _factory = nullptr;
    D3D_FEATURE_LEVEL _FeatureLevel = D3D_FEATURE_LEVEL_11_0;
    DXGI_ADAPTER_DESC1 _adapterDesc = {};

    // Managers
    shared_ptr<BufferManager> _bufferManager = nullptr;
    shared_ptr<D3D12ResourceManager> _resourceManager = nullptr;
    shared_ptr<RootSignature> _rootsignature = nullptr;
    shared_ptr<RenderTargets> _renderTargets = nullptr;
    shared_ptr<Shadow> _shadow = nullptr;
    shared_ptr<GBuffer> _GBuffer = nullptr;

    // Command Queue and List
    ComPtr<ID3D12CommandQueue> _cmdQueue = nullptr;
    ComPtr<ID3D12CommandAllocator> _cmdMemory[MAX_FRAME_COUNT] = {};
    ComPtr<ID3D12GraphicsCommandList> _cmdList[MAX_FRAME_COUNT] = {};
    uint64_t _lastFenceValue[MAX_FRAME_COUNT] = { 0 };

    // Synchronization
    ComPtr<ID3D12Fence> _fence = nullptr;
    HANDLE _fenceEvent = nullptr;
    uint64_t _fenceValue = 0;

    // Swap Chain
    ComPtr<IDXGISwapChain3> _swapChain = nullptr;
    UINT _swapChainFlags = 0;

    // Current State
    uint64_t _currentContextIndex = 0;

public:
    bool _bInit = false;
};
