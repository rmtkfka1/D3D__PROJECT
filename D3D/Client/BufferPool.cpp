#include "pch.h"
#include "BufferPool.h"
#include "Core.h"

void ConstantBufferPool::Init(CBV_REGISTER reg, uint32 size, uint32 count, bool useCamera)
{
	_reg = reg;
	_elementSize = (size + 255) & ~255;
	_elementCount = count;
	_useCamera = useCamera;

	//Buffer Pool 持失
	uint64 bufferSize = _elementSize * _elementCount;

	core->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_cbvBufferPool));

	_cbvBufferPool->Map(0, nullptr, reinterpret_cast<void**>(&_mappedBuffer));


	//琵持失
	D3D12_DESCRIPTOR_HEAP_DESC cbvDesc = {};
	cbvDesc.NumDescriptors = _elementCount;
	cbvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	cbvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	core->GetDevice()->CreateDescriptorHeap(&cbvDesc, IID_PPV_ARGS(&_cbvHeap));

	_cpuHandleBegin = _cbvHeap->GetCPUDescriptorHandleForHeapStart();
	_handleIncrementSize = core->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (uint32 i = 0; i < _elementCount; ++i)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_cpuHandleBegin, i * _handleIncrementSize);
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = _cbvBufferPool->GetGPUVirtualAddress() + static_cast<uint64>(_elementSize) * i;
		cbvDesc.SizeInBytes = _elementSize;
		core->GetDevice()->CreateConstantBufferView(&cbvDesc, cbvHandle);
	}

}

void ConstantBufferPool::PushData(void* buffer, uint32 size)
{
	assert(_currentIndex < _elementCount);

	::memcpy(&_mappedBuffer[_currentIndex * _elementSize], buffer, size);

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_cpuHandleBegin, _currentIndex * _handleIncrementSize);

	if (_useCamera==false)
	{
		core->GetTableHeap()->CopyCBV(cpuHandle, _reg);
	}
	else
	{
		core->GetTableHeap()->CopyCBV(cpuHandle);
	}

	_currentIndex++;
}

void ConstantBufferPool::SetData(void* buffer, uint32 size)
{
	assert(_elementSize == ((size + 255) & ~255));

	::memcpy(&_mappedBuffer[0], buffer, size);

	core->GetCmdLIst()->SetGraphicsRootConstantBufferView(0, _cbvBufferPool->GetGPUVirtualAddress());

}

void ConstantBufferPool::Clear()
{
	_currentIndex = 0;
}

/*************************
*                        *
*  TextureBuffer Pool    *
*                        *
**************************/


void TextureBufferPool::Init(int32 count)
{
	_heapSize = count;
	_handleIncrementSize = core->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	_indexGenerator.resize(_heapSize, -1);

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = _heapSize;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;  // Ensure shader visibility if needed

	ThrowIfFailed(core->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_srvHeap)));

}

void TextureBufferPool::Free(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE start = _srvHeap->GetCPUDescriptorHandleForHeapStart();
	DWORD index = (DWORD)(handle.ptr - start.ptr) / _handleIncrementSize;

	assert(index >= 0);

	_indexGenerator[index] = -1;
	_currentIndex--;
}

void TextureBufferPool::AllocDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE* handle)
{

	assert(_currentIndex < _heapSize);

	int32 index = Alloc();

	if (index == -1) {
		throw std::runtime_error("No free descriptor handles available");
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHandle(_srvHeap->GetCPUDescriptorHandleForHeapStart(), index, _handleIncrementSize);
	*handle = DescriptorHandle;
	_currentIndex++;

}

int32 TextureBufferPool::Alloc()
{
	for (int i = 0; i < _indexGenerator.size(); ++i)
	{
		if (_indexGenerator[i] == -1)
		{
			_indexGenerator[i] = 1;
			return i;
		}
	}

	return -1;  // No available slots
}


/*************************
*                        *
*    DescriptorTable     *
*                        *
**************************/

void DescriptorTable::Init(uint32 count , uint64 CameraMaxCount)
{
	_groupCount = count;
	_cameraMaxCount = CameraMaxCount;

	int32 RegisterCount = 1+3;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = count * RegisterCount + _cameraMaxCount;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	core->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_descHeap));

	_handleSize = core->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	_groupSize = _handleSize * (RegisterCount);

}

void DescriptorTable::Clear()
{
	_currentCameraIndex = 0;
	_currentGroupIndex = 0;
}

void DescriptorTable::CopyCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = _descHeap->GetCPUDescriptorHandleForHeapStart();
	destHandle.ptr += _currentCameraIndex * _handleSize;
	core->GetDevice()->CopyDescriptorsSimple(1, destHandle, srcHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void DescriptorTable::CopyCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg)
{
	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(reg);

	core->GetDevice()->CopyDescriptorsSimple(1, destHandle, srcHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void DescriptorTable::CopySRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg)
{
	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(reg);

	core->GetDevice()->CopyDescriptorsSimple(1, destHandle, srcHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void DescriptorTable::SetGraphicsRootDescriptorTable(int num)
{

	if (num == 1)
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handle = _descHeap->GetGPUDescriptorHandleForHeapStart();
		handle.ptr += _currentCameraIndex * _handleSize;
		core->GetCmdLIst()->SetGraphicsRootDescriptorTable(1, handle);
		_currentCameraIndex++;
	}

	else if(num==2)
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handle = _descHeap->GetGPUDescriptorHandleForHeapStart();
		handle.ptr += _currentGroupIndex * _groupSize + _cameraMaxCount * _handleSize;
		core->GetCmdLIst()->SetGraphicsRootDescriptorTable(2, handle);
		_currentGroupIndex++;
	}

	else
	{
		assert(false);
	}

	
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorTable::GetCPUHandle(CBV_REGISTER reg)
{
	return GetCPUHandle(static_cast<uint32>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorTable::GetCPUHandle(SRV_REGISTER reg)
{
	return GetCPUHandle(static_cast<uint32>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorTable::GetCPUHandle(uint32 reg)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = _descHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += _currentGroupIndex * _groupSize;
	handle.ptr += reg * _handleSize + _cameraMaxCount *_handleSize;
	return handle;
}
