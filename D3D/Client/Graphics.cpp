#include "pch.h"
#include "Graphics.h"
#include "RootSignature.h"
#include "RenderTargets.h"
#include "D3D12ResourceManager.h"
#include "BufferPool.h"
#include "KeyManager.h"
#include "LightManager.h"
#include "ResourceManager.h"
#include "Material.h"
#include "SteamOutputBuffer.h"


Graphics::Graphics()
{
};

Graphics::~Graphics()
{

};

void Graphics::Init(HWND hwnd, bool EnableDebugLayer, bool EnableGBV)
{

	_hwnd = hwnd;

	RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	::SetWindowPos(hwnd, 0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

	CreateDevice(EnableDebugLayer, EnableGBV);
	CreateCmdQueue();
	CreateSwapChain();
	CreateFence();
	CreateBufferPool();
	CreateRootSignature();

	_resourceManager = make_shared<D3D12ResourceManager>();
	_resourceManager->Init();


};

void Graphics::Fence()
{
	_fenceValue++;
	_cmdQueue->Signal(_fence.Get(), _fenceValue);
	_lastFenceValue[_currentContextIndex] = _fenceValue;

};


void Graphics::WaitForFenceValue(uint64 ExpectedFenceValue)
{

	if (_fence->GetCompletedValue() < ExpectedFenceValue)
	{
		_fence->SetEventOnCompletion(ExpectedFenceValue, _fenceEvent);

		::WaitForSingleObject(_fenceEvent, INFINITE);
	}

};

void Graphics::WaitForAllFence()
{
	Fence();

	for (DWORD i = 0; i < MAX_FRAME_COUNT; i++)
	{
		WaitForFenceValue(_lastFenceValue[i]);
	}
};

void Graphics::Exit()
{
	_swapChain->SetFullscreenState(FALSE, nullptr);
	WaitForAllFence();
};

void Graphics::SetFullScreen()
{
	if (FullScreen == false)
	{
		RECT rc;
		GetWindowRect(_hwnd, &rc);

		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(MonitorFromWindow(_hwnd, MONITOR_DEFAULTTOPRIMARY), &mi);

		// ��ü ȭ�� ��� ����
		SetWindowLong(_hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE); // ��Ÿ�� ����
		SetWindowPos(_hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
			mi.rcMonitor.right - mi.rcMonitor.left,
			mi.rcMonitor.bottom - mi.rcMonitor.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

		UpdateWindowSize(mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top);

		/*	_swapChain->SetFullscreenState(TRUE, nullptr);*/
	}

	else
	{
		//_swapChain->SetFullscreenState(FALSE, nullptr);

		SetWindowLong(_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
		// ���� ũ��� ������ ����
		SetWindowPos(_hwnd, HWND_TOP, 0, 0, 800, 600, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

		UpdateWindowSize(800, 600);
	}

	FullScreen = !FullScreen;
};


void Graphics::RenderBegin()
{

	ID3D12CommandAllocator* cmdMemory = _cmdMemory[_currentContextIndex].Get();
	ID3D12GraphicsCommandList* cmdList = _cmdList[_currentContextIndex].Get();

	ThrowIfFailed(cmdMemory->Reset());
	ThrowIfFailed(cmdList->Reset(cmdMemory, nullptr));

	cmdList->SetGraphicsRootSignature(_rootsignature->GetSignature().Get());
	cmdList->SetDescriptorHeaps(1, _table[_currentContextIndex]->GetDescriptorHeap().GetAddressOf());

};

void Graphics::RenderEnd()
{

	ID3D12CommandList* ppCommandLists[] = { _cmdList[_currentContextIndex].Get() };
	_cmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	Present();
};

void Graphics::Present()
{
	Fence();

	//Vsync on =1 Vsync off =0

	UINT SyncInterval = 1;

	UINT uiSyncInterval = SyncInterval;
	UINT uiPresentFlags = 0;

	if (!uiSyncInterval)
	{
		uiPresentFlags = DXGI_PRESENT_ALLOW_TEARING;
	}

	HRESULT hr = _swapChain->Present(uiSyncInterval, uiPresentFlags);

	_renderTargets->SetIndex(_swapChain->GetCurrentBackBufferIndex());
	uint64 nextContextIndex = (_currentContextIndex + 1) % MAX_FRAME_COUNT;
	WaitForFenceValue(_lastFenceValue[nextContextIndex]);

	_WorldBufferPool[nextContextIndex]->Clear();
	_CameraBufferPool[nextContextIndex]->Clear();
	_table[nextContextIndex]->Clear();
	_materialParamsBufferPool[nextContextIndex]->Clear();
	_currentContextIndex = nextContextIndex;

};

void Graphics::UpdateWindowSize(DWORD BackBufferWidth, DWORD BackBufferHeight)
{
	Fence();

	for (DWORD i = 0; i < MAX_FRAME_COUNT; i++)
	{
		WaitForFenceValue(_lastFenceValue[i]);
	}

	WINDOW_WIDTH = BackBufferWidth;
	WINDOW_HEIGHT = BackBufferHeight;

	POINT point{ BackBufferWidth / 2, BackBufferHeight / 2 };
	KeyManager::GetInstance()->SetCenterPos(point);

	_renderTargets->Resize(BackBufferWidth, BackBufferHeight, _swapChain, _swapChainFlags);
	_GBuffer->Init(_renderTargets->GetDSVHeap());
};


void Graphics::CreateDevice(bool EnableDebugLayer, bool EnableGBV)
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

	_adapterDesc = bestDesc;


	// ����� ���̾� ���� ����
	if (pDebugController)
	{
		SetDebugLayerInfo(_device);
	}


};

void Graphics::CreateCmdQueue()
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



};

void Graphics::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = WINDOW_WIDTH;
	swapChainDesc.Height = WINDOW_HEIGHT;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = SWAP_CHAIN_FRAME_COUNT;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Scaling = DXGI_SCALING_NONE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
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
	_renderTargets->Init(WINDOW_WIDTH, WINDOW_HEIGHT, _swapChain);

	_GBuffer = make_shared<GBuffer>();
	_GBuffer->Init(_renderTargets->GetDSVHeap());

};

void Graphics::CreateFence()
{
	if (FAILED(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence))))
	{
		__debugbreak();
	}

	_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
};

void Graphics::CreateRootSignature()
{
	_rootsignature = make_shared<RootSignature>();
	_rootsignature->Init();

};

void Graphics::CreateBufferPool()
{

	for (int i = 0; i < MAX_FRAME_COUNT; ++i)
	{
		_table[i] = make_shared<DescriptorTable>();
		_table[i]->Init(255);
	}

	for (int i = 0; i < MAX_FRAME_COUNT; ++i)
	{
		_CameraBufferPool[i] = make_shared<ConstantBufferPool>();
		_CameraBufferPool[i]->Init(CBV_REGISTER::b2, sizeof(CameraParams), 50); //b2 �� ��꿡 �̿��������
	}

	for (int i = 0; i < MAX_FRAME_COUNT; ++i)
	{
		_lightBufferPool[i] = make_shared<ConstantBufferPool>();
		_lightBufferPool[i]->Init(CBV_REGISTER::b2, sizeof(LightParams), 1); //b2 �� ��꿡 �̿��������
	}

	for (int i = 0; i < MAX_FRAME_COUNT; ++i)
	{
		_WorldBufferPool[i] = make_shared<ConstantBufferPool>();
		_WorldBufferPool[i]->Init(CBV_REGISTER::b2, sizeof(TransformParams), 255);
	}


	for (int i = 0; i < MAX_FRAME_COUNT; ++i)
	{
		_materialParamsBufferPool[i] = make_shared<ConstantBufferPool>();
		_materialParamsBufferPool[i]->Init(CBV_REGISTER::b3, sizeof(MaterialParams), 255); //b0 �� ��꿡 �̿��������
	}

	_textureBufferPool = make_shared<TextureBufferPool>();
	_textureBufferPool->Init(255);



};

void Graphics::SetDebugLayerInfo(ComPtr<ID3D12Device> pD3DDevice)
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

};
