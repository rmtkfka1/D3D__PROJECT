#pragma once
#include "Graphics.h"

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

	void CreateDevice(bool EnableDebugLayer, bool EnableGBV);
	void SetDebugLayerInfo(ComPtr<ID3D12Device> pD3DDevice);


	HWND GetWindowHandle() { return _hwnd; }
	ComPtr<ID3D12Device5>& GetDevice() { return _device; }

	ComPtr<ID3D12GraphicsCommandList>& GetCmdLIst() { return _graphics->GetCmdLIst(); }
	shared_ptr<RootSignature>& GetRootSignature() { return _graphics->GetRootSignature(); }
	shared_ptr<RenderTargets>& GetRenderTarget() { return _graphics->GetRenderTarget(); }
	shared_ptr<GBuffer>& GetGBuffer() { return _graphics->GetGBuffer(); }
	shared_ptr<D3D12ResourceManager>& GetResourceManager() { return _graphics->GetResourceManager(); }
	shared_ptr<DescriptorTable>& GetTableHeap() { return _graphics->GetTableHeap(); }
	shared_ptr<ConstantBufferPool>& GetWorldBufferPool() { return _graphics->GetWorldBufferPool(); }
	shared_ptr<ConstantBufferPool>& GetCameraBufferPool() { return _graphics->GetCameraBufferPool(); }
	shared_ptr<ConstantBufferPool>& GetLIghtBufferPool() { return _graphics->GetLIghtBufferPool(); }
	shared_ptr<ConstantBufferPool>& GetMaterialParamsBufferPool() { return _graphics->GetMaterialParamsBufferPool(); }
	shared_ptr<TextureBufferPool>& GetTextureBufferPool() { return _graphics->GetTextureBufferPool(); }
	shared_ptr<Graphics>& GetGraphics() { return _graphics; }


private:
	HWND _hwnd = nullptr;

	ComPtr<ID3D12Device5> _device = nullptr;
	ComPtr<IDXGIFactory4> _factory = nullptr;
	D3D_FEATURE_LEVEL	_FeatureLevel = D3D_FEATURE_LEVEL_11_0;
	DXGI_ADAPTER_DESC1	_adapterDesc = {};

	shared_ptr<Graphics> _graphics = nullptr;

};


