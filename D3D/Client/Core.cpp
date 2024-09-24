#include "pch.h"
#include "Core.h"
#include "RootSignature.h"
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

void Core::WaitSynce()
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

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_RTVHeap->GetCPUDescriptorHandleForHeapStart(), m_uiRenderTargetIndex, m_rtvDescriptorSize);

	_cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargets[m_uiRenderTargetIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// Record commands.
	const float BackColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	_cmdList->SetGraphicsRootSignature(_rootsignature->GetSignature().Get());

	_cmdList->RSSetViewports(1, &m_Viewport);
	_cmdList->RSSetScissorRects(1, &m_ScissorRect);
	_cmdList->ClearRenderTargetView(rtvHandle, BackColor, 0, nullptr);
	_cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
}

void Core::RenderEnd()
{
	_cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargets[m_uiRenderTargetIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	_cmdList->Close();

	ID3D12CommandList* ppCommandLists[] = { _cmdList.Get()};
	_cmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	Present();
}

void Core::Present()
{

	// Back Buffer 화면을 Primary Buffer로 전송

	//UINT m_SyncInterval = 1;	// VSync On
	//UINT m_SyncInterval = 0;	// VSync Off

	//UINT uiSyncInterval = m_SyncInterval;
	//UINT uiPresentFlags = 0;

	//if (!uiSyncInterval)
	//{
	//	uiPresentFlags = DXGI_PRESENT_ALLOW_TEARING;
	//}

	HRESULT hr = _swapChain->Present(1, 0);

	if (DXGI_ERROR_DEVICE_REMOVED == hr)
	{
		__debugbreak();
	}

	// for next frame
	m_uiRenderTargetIndex = _swapChain->GetCurrentBackBufferIndex();

	WaitSynce();
}

void Core::UpdateWindowSize(DWORD BackBufferWidth, DWORD BackBufferHeight)
{

	if (!(BackBufferWidth * BackBufferHeight))
		return;

	if (m_dwWidth == BackBufferWidth && m_dwHeight == BackBufferHeight)
		return;

	WaitSynce();

	for (UINT n = 0; n < SWAP_CHAIN_FRAME_COUNT; n++)
	{
		m_pRenderTargets[n].Reset();
	}

	DXGI_SWAP_CHAIN_DESC1	desc;
	HRESULT	hr = _swapChain->GetDesc1(&desc);

	if (FAILED(hr))
		__debugbreak();

	if (FAILED(_swapChain->ResizeBuffers(SWAP_CHAIN_FRAME_COUNT, BackBufferWidth, BackBufferHeight, DXGI_FORMAT_R8G8B8A8_UNORM, m_dwSwapChainFlags)))
	{
		__debugbreak();
	}

	m_uiRenderTargetIndex = _swapChain->GetCurrentBackBufferIndex();

	// Create frame resources.
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_RTVHeap->GetCPUDescriptorHandleForHeapStart());

	// Create a RTV for each frame.
	for (UINT n = 0; n < SWAP_CHAIN_FRAME_COUNT; n++)
	{
		_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_pRenderTargets[n]));
		_device->CreateRenderTargetView(m_pRenderTargets[n].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, m_rtvDescriptorSize);
	}

	m_dwWidth = BackBufferWidth;
	m_dwHeight = BackBufferHeight;
	m_Viewport.Width = (float)m_dwWidth;
	m_Viewport.Height = (float)m_dwHeight;
	m_ScissorRect.left = 0;
	m_ScissorRect.top = 0;
	m_ScissorRect.right = m_dwWidth;
	m_ScissorRect.bottom = m_dwHeight;

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

	_swapChain.Reset();


	////1번방식 

	//DXGI_SWAP_CHAIN_DESC sd;
	//sd.BufferDesc.Width = dwWndWidth; // 버퍼의 해상도 너비
	//sd.BufferDesc.Height = dwWndHeight; // 버퍼의 해상도 높이
	//sd.BufferDesc.RefreshRate.Numerator = 240; // 화면 갱신 비율
	//sd.BufferDesc.RefreshRate.Denominator = 1; // 화면 갱신 비율
	//sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 버퍼의 디스플레이 형식
	//sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	//sd.SampleDesc.Count = 1; // 멀티 샘플링 OFF
	//sd.SampleDesc.Quality = 0;
	//sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 후면 버퍼에 렌더링할 것 
	//sd.BufferCount = 2; // 전면+후면 버퍼
	//sd.OutputWindow = _hwnd;
	//sd.Windowed = true;
	//sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // 전면 후면 버퍼 교체 시 이전 프레임 정보 버림
	//sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	//ComPtr<IDXGISwapChain> pSwapChain = nullptr;
	//_factory->CreateSwapChain(_cmdQueue.Get(), &sd, &pSwapChain);

	//pSwapChain->QueryInterface(IID_PPV_ARGS(&_swapChain));

	//for (int32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; i++)
	//	_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_pRenderTargets[i]));

	//m_uiRenderTargetIndex = _swapChain->GetCurrentBackBufferIndex();

	//2번방식

	{
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

		m_uiRenderTargetIndex = _swapChain->GetCurrentBackBufferIndex();
	}

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = SWAP_CHAIN_FRAME_COUNT;	// SwapChain Buffer 0	| SwapChain Buffer 1
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	if (FAILED(_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_RTVHeap))))
	{
		__debugbreak();
	}

	m_rtvDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	m_Viewport.Width = (float)dwWndWidth;
	m_Viewport.Height = (float)dwWndHeight;
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	m_ScissorRect.left = 0;
	m_ScissorRect.top = 0;
	m_ScissorRect.right = dwWndWidth;
	m_ScissorRect.bottom = dwWndHeight;

	m_dwWidth = dwWndWidth;
	m_dwHeight = dwWndHeight;


	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_RTVHeap->GetCPUDescriptorHandleForHeapStart());

	for (UINT n = 0; n < SWAP_CHAIN_FRAME_COUNT; n++)
	{
		_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_pRenderTargets[n]));
		_device->CreateRenderTargetView(m_pRenderTargets[n].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, m_rtvDescriptorSize);
	}

}

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





