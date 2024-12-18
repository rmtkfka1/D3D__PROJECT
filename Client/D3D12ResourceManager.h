#pragma once
class D3D12ResourceManager
{
public:
	void Init();
	ComPtr<ID3D12GraphicsCommandList>  GetCmdList() { return _cmdList; }
	ComPtr<ID3D12CommandQueue> GetCmdQueue() { return _cmdQueue; }
	ComPtr<ID3D12CommandAllocator> GetCmdMemory() { return _cmdMemory; }

public:
	void Fence();
	void WaitForFenceValue();
	void Excute();

private:

	ComPtr<ID3D12Device5> _device = nullptr;
	ComPtr<ID3D12CommandQueue> _cmdQueue = nullptr;
	ComPtr<ID3D12CommandAllocator> _cmdMemory = nullptr;
	ComPtr<ID3D12GraphicsCommandList> _cmdList = nullptr;

	HANDLE	_fenceEvent = nullptr;
	ComPtr<ID3D12Fence> _fence = nullptr;
	uint64	_fenceValue = 0;
};

