#pragma once
#include "Graphics.h"

class RootSignature;
class RenderTargets;
class GBuffer;
class D3D12ResourceManager;

class DescriptorTable;
class TextureBufferPool;
class ConstantBufferPool;
class BufferManager;




class Core
{

public:
	Core();
	~Core();

	void Init(HWND hwnd, bool EnableDebugLayer, bool EnableGBV);

	//Device
	void CreateDevice(bool EnableDebugLayer, bool EnableGBV);
	void SetDebugLayerInfo(ComPtr<ID3D12Device> pD3DDevice);

	void WaitForAllFence() { _graphics->WaitForAllFence(); }

	HWND GetWindowHandle() { return _hwnd; }
	ComPtr<ID3D12Device5>& GetDevice() { return _device; }


	shared_ptr<D3D12ResourceManager>& GetResourceManager() { return _resourceManager; }
	shared_ptr<Graphics>& GetGraphics() { return _graphics; }
	shared_ptr<BufferManager>& GetBufferManager() { return _bufferManager; }


private:
	HWND _hwnd = nullptr;

	//Shared
	ComPtr<ID3D12Device5> _device = nullptr;
	ComPtr<IDXGIFactory4> _factory = nullptr;
	D3D_FEATURE_LEVEL	_FeatureLevel = D3D_FEATURE_LEVEL_11_0;
	DXGI_ADAPTER_DESC1	_adapterDesc = {};
	shared_ptr<BufferManager> _bufferManager = nullptr;
	shared_ptr<D3D12ResourceManager> _resourceManager=nullptr;

	//Graphics
	shared_ptr<Graphics> _graphics;

};

