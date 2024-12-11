#pragma once

class StructuredBuffer
{


public:
	void Init(uint32 size , uint32 count ,void* initData);

private:
	void CopyInitialData(uint64 bufferSize, void* initialData);

	ComPtr<ID3D12Resource> _resource;

	D3D12_CPU_DESCRIPTOR_HANDLE _srvHandle = {};

};

