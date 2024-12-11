#include "pch.h"
#include "StructuredBuffer.h"
#include "Core.h"
#include "BufferPool.h"
#include "D3D12ResourceManager.h"
void StructuredBuffer::Init(uint32 size, uint32 count, void* initData)
{

	uint64 bufferSize = static_cast<uint64>(size) * count;
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
	D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	core->GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&_resource));

	if (initData)
	{
		CopyInitialData(size * count, initData);
	}

	core->GetBufferManager()->GetTextureBufferPool()->AllocSRVDescriptorHandle(&_srvHandle);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = count;
	srvDesc.Buffer.StructureByteStride = size;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	core->GetDevice()->CreateShaderResourceView(_resource.Get(), &srvDesc, _srvHandle);

}

void StructuredBuffer::PushData(SRV_REGISTER reg)
{
	core->GetBufferManager()->GetTable()->CopySRV(_srvHandle, reg);
}

void StructuredBuffer::CopyInitialData(uint64 bufferSize, void* initialData)
{
	ComPtr<ID3D12Resource> readBuffer = nullptr;

	core->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_NONE),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&readBuffer));

	uint8* dataBegin = nullptr;
	D3D12_RANGE readRange{ 0, 0 };
	readBuffer->Map(0, &readRange, reinterpret_cast<void**>(&dataBegin));
	memcpy(dataBegin, initialData, bufferSize);
	readBuffer->Unmap(0, nullptr);

	D3D12_RESOURCE_BARRIER berforeBarrier = CD3DX12_RESOURCE_BARRIER::Transition(_resource.Get(),D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);

	core->GetResourceManager()->GetCmdList()->ResourceBarrier(1, &berforeBarrier);

	core->GetResourceManager()->GetCmdList()->CopyBufferRegion(_resource.Get(),0, readBuffer.Get(),0, bufferSize);

	D3D12_RESOURCE_BARRIER afterBarrier = CD3DX12_RESOURCE_BARRIER::Transition(_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
	core->GetResourceManager()->GetCmdList()->ResourceBarrier(1, &afterBarrier);

	core->GetResourceManager()->Excute();

}
