#include "pch.h"
#include "Core.h"
#include "RootSignature.h"
#include "RenderTargets.h"
#include "D3D12ResourceManager.h"
#include "BufferPool.h"
#include "KeyManager.h"
#include "LightManager.h"
#include "ResourceManager.h"
#include "Material.h"
#include "SteamOutputBuffer.h"
#include "Graphics.h"
#include "BufferManager.h"
#include "Compute.h"


Core::Core()
{
};

Core::~Core()
{

};

void Core::Init(HWND hwnd, bool EnableDebugLayer, bool EnableGBV)
{
	_hwnd = hwnd;

	CreateDevice(EnableDebugLayer, EnableGBV);

	_bufferManager = make_shared<BufferManager>();
	_bufferManager->Init();

	_graphics = make_shared<Graphics>();
	_graphics->Init(hwnd,_device,_factory, _bufferManager);

	_compute = make_shared<Compute>();
	_compute->Init(_device);

	_rootsignature = make_shared<RootSignature>();
	_rootsignature->Init();

	_resourceManager = make_shared<D3D12ResourceManager>();
	_resourceManager->Init();


	_bInit = true;
}

void Core::CreateDevice(bool EnableDebugLayer, bool EnableGBV)
{
	ComPtr<ID3D12Debug> pDebugController = nullptr;
	ComPtr<IDXGIAdapter1> bestAdapter = nullptr;
	DXGI_ADAPTER_DESC1 bestDesc;
	ZeroMemory(&bestDesc, sizeof(bestDesc));
	size_t bestMemory = 0; // VRAM 용량

	DWORD dwCreateFlags = 0;
	DWORD dwCreateFactoryFlags = 0;


	// 디버그 레이어 활성화
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

	// DXGI 팩토리 생성
	CreateDXGIFactory2(dwCreateFactoryFlags, IID_PPV_ARGS(&_factory));

	// 지원되는 기능 레벨 설정
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	DWORD FeatureLevelNum = _countof(featureLevels);

	// 어댑터 열거
	for (UINT adapterIndex = 0; ; ++adapterIndex)
	{
		ComPtr<IDXGIAdapter1> pAdapter;
		HRESULT hr = _factory->EnumAdapters1(adapterIndex, &pAdapter);

		// 어댑터가 없으면 종료
		if (hr == DXGI_ERROR_NOT_FOUND)
			break;

		DXGI_ADAPTER_DESC1 AdapterDesc;
		pAdapter->GetDesc1(&AdapterDesc);

		// VRAM을 메가바이트 단위로 변환
		size_t memory = AdapterDesc.DedicatedVideoMemory / (1024 * 1024);

		// 가장 높은 VRAM을 가진 어댑터 선택
		if (memory > bestMemory)
		{
			bestMemory = memory;
			bestAdapter = pAdapter;
			bestDesc = AdapterDesc;
		}
	}



	// 선택된 어댑터로 D3D12 장치 생성
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


	// 디버그 레이어 정보 설정
	if (pDebugController)
	{
		SetDebugLayerInfo(_device);
	}

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







