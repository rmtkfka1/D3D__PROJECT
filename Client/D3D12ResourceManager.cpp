#include "pch.h"
#include "D3D12ResourceManager.h"
#include "Core.h"

void D3D12ResourceManager::Init()
{
	_device = core->GetDevice();

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_cmdQueue)));
	ThrowIfFailed(_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdMemory)));
	ThrowIfFailed(_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdMemory.Get(), nullptr, IID_PPV_ARGS(&_cmdList)));


	///Create Fence

	ThrowIfFailed(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence)));
	_fenceValue = 0;
	_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

}

void D3D12ResourceManager::Fence()
{

	_fenceValue++;
	_cmdQueue->Signal(_fence.Get(), _fenceValue);

}

void D3D12ResourceManager::WaitForFenceValue()
{
	const uint64 ExpectedFenceValue = _fenceValue;

	
	if (_fence->GetCompletedValue() < ExpectedFenceValue)
	{
		_fence->SetEventOnCompletion(ExpectedFenceValue, _fenceEvent);
		WaitForSingleObject(_fenceEvent, INFINITE);
	}
}

void D3D12ResourceManager::Excute()
{
	_cmdList->Close();

	ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	Fence();
	WaitForFenceValue();

	_cmdMemory->Reset();
	_cmdList->Reset(_cmdMemory.Get(), nullptr);

}

