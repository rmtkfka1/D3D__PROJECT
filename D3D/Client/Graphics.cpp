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
#include "BufferManager.h"
#include "Core.h"
#include "Compute.h"

Graphics::Graphics()
{
};

Graphics::~Graphics()
{

};

void Graphics::Init(HWND hwnd, ComPtr<ID3D12Device5> device , ComPtr<IDXGIFactory4> factory, shared_ptr<BufferManager> bufferManager)
{

	_hwnd = hwnd;
	_device = device;
	_factory = factory;
	_bufferManager = bufferManager;

	RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	::SetWindowPos(hwnd, 0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

	CreateCmdQueue();
	CreateSwapChain();
	CreateFence();

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

		// 전체 화면 모드 설정
		SetWindowLong(_hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE); // 스타일 변경
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
		// 원래 크기로 윈도우 설정
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

	cmdList->SetGraphicsRootSignature(core->GetRootSignature()->GetGraphicsRootSignature().Get());
	cmdList->SetDescriptorHeaps(1, _bufferManager->GetGraphicsTableHeap()->GetDescriptorHeap().GetAddressOf());

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

	int index = _swapChain->GetCurrentBackBufferIndex();

	_renderTargets->SetIndex(index);

	uint64 nextContextIndex = (_currentContextIndex + 1) % MAX_FRAME_COUNT;
	WaitForFenceValue(_lastFenceValue[nextContextIndex]);

	_currentContextIndex = nextContextIndex;
	_bufferManager->Clear(_currentContextIndex);
	_bufferManager->SetIndex(_currentContextIndex);

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
	_GBuffer->Init();
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
	_GBuffer->Init();


};

void Graphics::CreateFence()
{
	if (FAILED(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence))))
	{
		__debugbreak();
	}

	_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
};






