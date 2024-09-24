#include "pch.h"
#include "Core.h"
#include "RootSignature.h"
#include "RenderTargets.h"
Core::Core()
{
}

Core::~Core()
{
}

void Core::Init(HWND hwnd, bool EnableDebugLayer, bool EnableGBV)
{

	_hwnd = hwnd;

	RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	::SetWindowPos(hwnd, 0, 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

	CreateDevice(EnableDebugLayer, EnableGBV);
	CreateCmdQueue();
	CreateSwapChain();
	CreateFence();
	CreateRootSignature();

}

void Core::WaitSync()
{
	_fenceValue++;
	_cmdQueue->Signal(_fence.Get(), _fenceValue);

	if (_fence->GetCompletedValue() < _fenceValue)
	{
		_fence->SetEventOnCompletion(_fenceValue, _fenceEvent);

		::WaitForSingleObject(_fenceEvent, INFINITE);
	}

}

void Core::RenderBegin()
{
	if (FAILED(_cmdMemory->Reset()))
		__debugbreak();

	if (FAILED(_cmdList->Reset(_cmdMemory.Get(), nullptr)))
		__debugbreak();

	_cmdList->SetGraphicsRootSignature(_rootsignature->GetSignature().Get());

	_renderTargets->RenderBegin();
}

void Core::RenderEnd()
{

	_renderTargets->RenderEnd();

	ID3D12CommandList* ppCommandLists[] = { _cmdList.Get()};
	_cmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	Present();
}

void Core::Present()
{

	HRESULT hr = _swapChain->Present(1, 0);

	if (DXGI_ERROR_DEVICE_REMOVED == hr)
	{
		__debugbreak();
	}

	// for next frame
	_renderTargets->SetIndex(_swapChain->GetCurrentBackBufferIndex());

	WaitSync();
}

void Core::UpdateWindowSize(DWORD BackBufferWidth, DWORD BackBufferHeight)
{

	WaitSync();

	_renderTargets->Resize(BackBufferWidth, BackBufferHeight, _swapChain, m_dwSwapChainFlags);

}


void Core::CreateDevice(bool EnableDebugLayer, bool EnableGBV)
{
	ComPtr<ID3D12Debug> pDebugController = nullptr;
	ComPtr<IDXGIAdapter1> pAdapter = nullptr;
	DXGI_ADAPTER_DESC1 AdapterDesc = {};

	DWORD dwCreateFlags = 0;
	DWORD dwCreateFactoryFlags = 0;

	// if use debug Layer...
	if (EnableDebugLayer)
	{
		// Enable the D3D12 debug layer.
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pDebugController))))
		{
			pDebugController->EnableDebugLayer();
		}

		dwCreateFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

		if (EnableGBV)
		{
			ID3D12Debug5* pDebugController5 = nullptr;
			if (S_OK == pDebugController->QueryInterface(IID_PPV_ARGS(&pDebugController5)))
			{
				pDebugController5->SetEnableGPUBasedValidation(TRUE);
				pDebugController5->SetEnableAutoName(TRUE);
				pDebugController5->Release();
			}
		}
	}


	CreateDXGIFactory2(dwCreateFactoryFlags, IID_PPV_ARGS(&_factory));

	D3D_FEATURE_LEVEL	featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	DWORD	FeatureLevelNum = _countof(featureLevels);
	

	for (DWORD featerLevelIndex = 0; featerLevelIndex < FeatureLevelNum; featerLevelIndex++)
	{
		UINT adapterIndex = 0;

		while (DXGI_ERROR_NOT_FOUND != _factory->EnumAdapters1(adapterIndex, &pAdapter))
		{
			pAdapter->GetDesc1(&AdapterDesc);

			if (SUCCEEDED(D3D12CreateDevice(pAdapter.Get(), featureLevels[featerLevelIndex], IID_PPV_ARGS(&_device))))
			{
				break;
			}

			adapterIndex++;
		}

		if (_device)
		{
			break;
		}
	}

	_adapterDesc = AdapterDesc;

	if (pDebugController)
	{
		SetDebugLayerInfo(_device);
	}

}

void Core::CreateCmdQueue()
{
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	ThrowIfFailed(_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&_cmdQueue)));
	ThrowIfFailed(_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdMemory)));
	ThrowIfFailed(_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdMemory.Get(), nullptr, IID_PPV_ARGS(&_cmdList)));


	_cmdList->Close();

	ThrowIfFailed(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence)));
	_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);

}

void Core::CreateSwapChain()
{


	RECT	rect;
	::GetClientRect(_hwnd, &rect);
	DWORD	dwWndWidth = rect.right - rect.left;
	DWORD	dwWndHeight = rect.bottom - rect.top;
	UINT	dwBackBufferWidth = rect.right - rect.left;
	UINT	dwBackBufferHeight = rect.bottom - rect.top;


	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = dwBackBufferWidth;
	swapChainDesc.Height = dwBackBufferHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = SWAP_CHAIN_FRAME_COUNT;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Scaling = DXGI_SCALING_NONE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	m_dwSwapChainFlags = swapChainDesc.Flags;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
	fsSwapChainDesc.Windowed = TRUE;

	IDXGISwapChain1* pSwapChain1 = nullptr;
	if (FAILED(_factory->CreateSwapChainForHwnd(_cmdQueue.Get(), _hwnd, &swapChainDesc, &fsSwapChainDesc, nullptr, &pSwapChain1)))
	{
		__debugbreak();
	}

	pSwapChain1->QueryInterface(IID_PPV_ARGS(&_swapChain));
	pSwapChain1->Release();
	pSwapChain1 = nullptr;

	_renderTargets = make_shared<RenderTargets>();
	_renderTargets->Init(dwWndWidth, dwWndHeight, _swapChain);



};

void Core::CreateFence()
{
	if (FAILED(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence))))
	{
		__debugbreak();
	}

	_fenceValue = 0;
	_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void Core::CreateRootSignature()
{
	_rootsignature = make_shared<RootSignature>();
	_rootsignature->Init();

}

void Core::SetDebugLayerInfo(ComPtr<ID3D12Device> pD3DDevice)
{

	ID3D12InfoQueue* pInfoQueue = nullptr;
	pD3DDevice->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
	if (pInfoQueue)
	{
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);

		D3D12_MESSAGE_ID hide[] =
		{
			D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
			D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
			// Workarounds for debug layer issues on hybrid-graphics systems
			D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};

		D3D12_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumIDs = (UINT)_countof(hide);
		filter.DenyList.pIDList = hide;
		pInfoQueue->AddStorageFilterEntries(&filter);

		pInfoQueue->Release();
		pInfoQueue = nullptr;
	}

}





