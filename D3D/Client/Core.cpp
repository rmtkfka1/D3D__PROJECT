#include "pch.h"
#include "Core.h"
#include "RootSignature.h"
#include "RenderTargets.h"
#include "D3D12ResourceManager.h"
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

	_resourceManager = make_shared<D3D12ResourceManager>();
	_resourceManager->Init();
}

void Core::Fence()
{
	_fenceValue++;
	_cmdQueue->Signal(_fence.Get(), _fenceValue);
	_lastFenceValue[_currentContextIndex] = _fenceValue;

}


void Core::WaitForFenceValue(uint64 ExpectedFenceValue)
{

	if (_fence->GetCompletedValue() < ExpectedFenceValue)
	{
		_fence->SetEventOnCompletion(ExpectedFenceValue, _fenceEvent);

		::WaitForSingleObject(_fenceEvent, INFINITE);
	}

}

void Core::WaitForFenceValue()
{

	uint64 ExpectedFenceValue = _lastFenceValue[_currentContextIndex];

	if (_fence->GetCompletedValue() < ExpectedFenceValue)
	{
		_fence->SetEventOnCompletion(ExpectedFenceValue, _fenceEvent);

		::WaitForSingleObject(_fenceEvent, INFINITE);
	}

}

void Core::RenderBegin()
{
\
	ComPtr<ID3D12CommandAllocator> cmdMemory = _cmdMemory[_currentContextIndex];
	ComPtr<ID3D12GraphicsCommandList> cmdList = _cmdList[_currentContextIndex];

	ThrowIfFailed(cmdMemory->Reset());
	ThrowIfFailed(cmdList->Reset(_cmdMemory->Get(), nullptr));
	

	cmdList->SetGraphicsRootSignature(_rootsignature->GetSignature().Get());

	_renderTargets->RenderBegin();
}

void Core::RenderEnd()
{

	_renderTargets->RenderEnd();

	ID3D12CommandList* ppCommandLists[] = { _cmdList[_currentContextIndex].Get()};
	_cmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	Present();
}

void Core::Present()
{
	Fence();

	ThrowIfFailed(_swapChain->Present(1, 0));

	_renderTargets->SetIndex(_swapChain->GetCurrentBackBufferIndex());

	uint32 nextContextIndex = (_currentContextIndex + 1) % MAX_FRAME_COUNT;
	WaitForFenceValue(_lastFenceValue[nextContextIndex]);

	_currentContextIndex = nextContextIndex;

}

void Core::UpdateWindowSize(DWORD BackBufferWidth, DWORD BackBufferHeight)
{
	Fence();

	for (DWORD i = 0; i < MAX_FRAME_COUNT; i++)
	{
		WaitForFenceValue(_lastFenceValue[i]);
	}

	_renderTargets->Resize(BackBufferWidth, BackBufferHeight, _swapChain, _swapChainFlags);

}


void Core::CreateDevice(bool EnableDebugLayer, bool EnableGBV)
{
	ComPtr<ID3D12Debug> pDebugController = nullptr;
	ComPtr<IDXGIAdapter1> bestAdapter = nullptr;
	DXGI_ADAPTER_DESC1 bestDesc;
	ZeroMemory(&bestDesc, sizeof(bestDesc));
	size_t bestMemory = 0; // VRAM �뷮

	DWORD dwCreateFlags = 0;
	DWORD dwCreateFactoryFlags = 0;


	// ����� ���̾� Ȱ��ȭ
	if (EnableDebugLayer)
	{
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

	// DXGI ���丮 ����
	CreateDXGIFactory2(dwCreateFactoryFlags, IID_PPV_ARGS(&_factory));

	// �����Ǵ� ��� ���� ����
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	DWORD FeatureLevelNum = _countof(featureLevels);

	// ����� ����
	for (UINT adapterIndex = 0; ; ++adapterIndex)
	{
		ComPtr<IDXGIAdapter1> pAdapter;
		HRESULT hr = _factory->EnumAdapters1(adapterIndex, &pAdapter);

		// ����Ͱ� ������ ����
		if (hr == DXGI_ERROR_NOT_FOUND)
			break;

		DXGI_ADAPTER_DESC1 AdapterDesc;
		pAdapter->GetDesc1(&AdapterDesc);

		// VRAM�� �ް�����Ʈ ������ ��ȯ
		size_t memory = AdapterDesc.DedicatedVideoMemory / (1024 * 1024);
	
		// ���� ���� VRAM�� ���� ����� ����
		if (memory > bestMemory)
		{
			bestMemory = memory;
			bestAdapter = pAdapter;
			bestDesc = AdapterDesc;
		}
	}

	// ���õ� ����ͷ� D3D12 ��ġ ����
	for (DWORD featerLevelIndex = 0; featerLevelIndex < FeatureLevelNum; featerLevelIndex++)
	{
		if (bestAdapter)
		{
			HRESULT hr = D3D12CreateDevice(bestAdapter.Get(), featureLevels[featerLevelIndex], IID_PPV_ARGS(&_device));

			if (SUCCEEDED(hr))
			{
				break;
			}
		}
	}


	// ����� ���̾� ���� ����
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


	for (int32 i = 0; i < MAX_FRAME_COUNT; i++)
	{
		ThrowIfFailed(_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdMemory[i])));
		ThrowIfFailed(_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdMemory[i].Get(), nullptr, IID_PPV_ARGS(&_cmdList[i])));
		ThrowIfFailed(_cmdList[i]->Close());
	}


	//FENCE GEN
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
	_swapChainFlags = swapChainDesc.Flags;

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





